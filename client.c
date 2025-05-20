/*
 * Name: Your Name
 * Course-Section: CS440-SP25
 * Assignment: Project 7
 * Date due: 04/30/2025
 * Collaborators: None
 * Resources: assignment-7.pdf, addendum-7.pdf, aes_crypto.h, bgnet_usl_c_1.pdf, Style guide.pdf
 * Description:
 *   TCP client implementing a challenge/response protocol:
 *   1) send code 105 request
 *   2) receive encrypted 110 challenge
 *   3) decrypt payload, compute result
 *   4) encrypt and send 115 response
 *   5) receive 205/210 status or 305 error and report SUCCESS or FAILURE
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

#include "aes_crypto.h"            /* crypt_aes(), DECRYPT/ENCRYPT, BLK_LEN */

#define SERVER_PORT     "11000"
#define USERNAME_MAX    16
#define PAYLOAD_MAX     128
#define IV_LEN          16            /* AES CBC IV length */

enum { MSG_LEN = 3 + USERNAME_MAX + IV_LEN + 4 + PAYLOAD_MAX };

static const unsigned char AES_KEY[16] = {
    /* fill with ASCII key of current user, e.g. for brubble: */
    'J','w','0','v','M','S','6','v',
    'G','6','k','k','a','0','g','l'
};

static void
send_all(int sock, const void *buf, size_t len)
{
    const unsigned char *p = buf;
    size_t           sent = 0;

    while (sent < len) {
        ssize_t n = send(sock, p + sent, len - sent, 0);
        if (n <= 0)
            err(1, "send");
        sent += n;
    }
}

static void
recv_all(int sock, void *buf, size_t len)
{
    unsigned char *p = buf;
    size_t         recvd = 0;

    while (recvd < len) {
        ssize_t n = recv(sock, p + recvd, len - recvd, 0);
        if (n < 0)
            err(1, "recv");
        if (n == 0)
            errx(1, "connection closed unexpectedly");
        recvd += n;
    }
}

int
main(int argc, char *argv[])
{
    int                     sock = -1;
    struct addrinfo         hints, *res = NULL;
    char                    username[USERNAME_MAX] = {0};
    char                   *login = getenv("USER");
    unsigned char           msg[MSG_LEN];
    unsigned char           iv_buf[BLK_LEN];
    unsigned char          *enc_payload = NULL, *dec_payload = NULL;
    size_t                  enc_len, dec_len;
    uint32_t                net_plen;
    char                    op;
    uint32_t                left, right;
    uint64_t                result, net_result;
    int                     status;

    if (argc != 2)
        errx(1, "usage: %s <server-hostname-or-ip>", getprogname());

    /* get username (null-terminated, max 15 chars + '\0') */
    if (login == NULL || *login == '\0')
        snprintf(username, sizeof username, "unknown");
    else
        strlcpy(username, login, sizeof username);

    /* lookup server address */
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((status = getaddrinfo(argv[1], SERVER_PORT, &hints, &res)) != 0)
        errx(1, "getaddrinfo: %s", gai_strerror(status));

    /* connect */
    for (struct addrinfo *p = res; p; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock < 0)
            continue;
        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0)
            break;
        close(sock);
        sock = -1;
    }
    if (sock < 0)
        errx(1, "unable to connect to %s:%s", argv[1], SERVER_PORT);
    freeaddrinfo(res);

    /* 1) send challenge request (105 + user) */
    printf("send request\n\n");
    memset(msg, 0, sizeof msg);
    memcpy(msg, "105", 3);
    memcpy(msg + 3, username, USERNAME_MAX);
    send_all(sock, msg, 3 + USERNAME_MAX);

    /* 2) receive full 110 or 305 message */
    printf("receive challenge\n\n");
    recv_all(sock, msg, MSG_LEN);
    /* handle possible error message */
    if (memcmp(msg, "305", 3) == 0)
        errx(1, "server error");
    if (memcmp(msg, "110", 3) != 0)
        errx(1, "expected code 110, got %.3s", msg);

    /* extract IV */
    memcpy(iv_buf, msg + 3 + USERNAME_MAX, IV_LEN);

    /* decrypt payload */
    memcpy(&net_plen, msg + 3 + USERNAME_MAX + IV_LEN, 4);
    uint32_t payload_len = ntohl(net_plen);
    if (payload_len > PAYLOAD_MAX || payload_len > 96)
        errx(1, "invalid payload length %u", payload_len);

    unsigned char *iv_ptr = iv_buf;
    dec_len = crypt_aes(
        msg + 3 + USERNAME_MAX + IV_LEN + 4,
        payload_len,
        &dec_payload,
        AES_KEY,
        &iv_ptr,
        DECRYPT
    );
    if (dec_len == 0)
        errx(1, "decryption failed");

    /* parse decrypted payload: op, left, right */
    op    = dec_payload[0];
    memcpy(&left,  dec_payload + 1, 4);
    memcpy(&right, dec_payload + 5, 4);
    free(dec_payload);
    left  = ntohl(left);
    right = ntohl(right);

    /* compute result */
    switch (op) {
    case '+': result = (uint64_t)left + right; break;
    case '-': result = (uint64_t)left - right; break;
    case '*': result = (uint64_t)left * right; break;
    case '/':
        if (right == 0) errx(1, "division by zero");
        result = (uint64_t)(left / right);
        break;
    case '%':
        if (right == 0) errx(1, "mod by zero");
        result = (uint64_t)(left % right);
        break;
    default:
        errx(1, "unknown operation '%c'", op);
    }

    /* 3) send challenge response (115 + user + IV + length + encrypted result) */
    printf("send challenge response\n\n");
    memset(msg, 0, sizeof msg);
    memcpy(msg, "115", 3);
    memcpy(msg + 3, username, USERNAME_MAX);

    /* prepare plaintext payload */
    unsigned char plain[8];
    net_result = htobe64(result);
    memcpy(plain, &net_result, 8);

    /* encrypt */
    unsigned char iv_out[BLK_LEN];
    unsigned char *iv_ptr2 = iv_out;
    enc_len = crypt_aes(
        plain, 8,
        &enc_payload,
        AES_KEY,
        &iv_ptr2,
        ENCRYPT
    );
    if (enc_len == 0)
        errx(1, "encryption failed");

    /* fill IV */
    memcpy(msg + 3 + USERNAME_MAX, iv_out, IV_LEN);
    /* fill payload length */
    net_plen = htonl(enc_len);
    memcpy(msg + 3 + USERNAME_MAX + IV_LEN, &net_plen, 4);
    /* fill encrypted payload */
    memcpy(msg + 3 + USERNAME_MAX + IV_LEN + 4, enc_payload, enc_len);
    free(enc_payload);

    send_all(sock, msg, MSG_LEN);

    /* 4) receive status or error message */
    printf("receive ID ");
    recv_all(sock, msg, 3 + USERNAME_MAX);
    if (memcmp(msg, "305", 3) == 0)
        errx(1, "server error");
    if (memcmp(msg, "205", 3) == 0)
        puts("SUCCESS");
    else if (memcmp(msg, "210", 3) == 0)
        puts("FAILURE");
    else
        errx(1, "unexpected response %.3s", msg);

    close(sock);
    return 0;
}
