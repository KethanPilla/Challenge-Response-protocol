# Challenge/Response Protocol Client

## CS440 Computer Networking - Project 7

This project implements a client application that communicates with a server using a challenge/response protocol over TCP/IP. The client and server exchange encrypted messages as part of an authentication flow.

## Protocol Overview

The client-server communication follows a specific protocol:

1. Client sends a **challenge request** (code 105)
2. Server responds with an encrypted **challenge** (code 110)
3. Client decrypts the challenge, computes the result, and sends an encrypted **challenge response** (code 115)
4. Server verifies the response and sends a **success** (code 205) or **failure** (code 210) message

The server may also send an **error** message (code 305) if the protocol is violated or the messages are malformed.

## Message Format

Messages are byte streams with the following structure:

| Field           | Length   | Data Type      | Description                       |
|-----------------|----------|----------------|-----------------------------------|
| Message code    | 3 bytes  | char           | Protocol operation code           |
| User name       | 16 bytes | char           | Client username (null-terminated) |
| IV              | 16 bytes | unsigned char  | Initialization vector for AES     |
| Payload length  | 4 bytes  | uint32_t       | Length of encrypted payload       |
| Payload         | 128 bytes| unsigned char  | Encrypted data                    |

Different message types require different fields:
- Challenge (110) and challenge response (115) require all fields
- Challenge request (105), success (205), and failure (210) only require message code and username
- Error (305) only requires the message code

## Challenge Payload Format

The challenge payload contains the following structure:

| Field           | Length   | Data Type      | Description                       |
|-----------------|----------|----------------|-----------------------------------|
| Operation       | 1 byte   | char           | Mathematical operation (+,-,*,/,%) |
| Left operand    | 4 bytes  | uint32_t       | First operand                     |
| Right operand   | 4 bytes  | uint32_t       | Second operand                    |

## Challenge Response Payload Format

The challenge response payload contains:

| Field             | Length   | Data Type      | Description                       |
|-------------------|----------|----------------|-----------------------------------|
| Challenge response| 8 bytes  | uint64_t       | Result of the operation           |

## Encryption

The system uses AES-128 in CBC mode for encryption and decryption. The payload field is always encrypted, and appropriate initialization vectors (IVs) are used to ensure security.

## Implementation Details

- Implemented in C using BSD socket API
- Uses TCP communication on port 11000
- Handles all required message types and error conditions
- Properly manages memory allocation and cleanup
- Ensures all numeric values are sent in network byte order

## Building and Running

To compile the client:

```
make
```

To run the client:

```
./client <server-hostname-or-ip>
```

The client will output each step of the protocol, clearly indicating success or failure.

## Error Handling

The client implements robust error handling, including:
- Proper cleanup of resources
- Informative error messages
- Graceful handling of network errors
- Validation of server responses

## Dependencies

- BSD socket API
- AES encryption library (provided by course)

## Examples

Successful exchange:
```
send request

receive challenge

send challenge response

receive ID SUCCESS
```

Failed authentication:
```
send request

receive challenge

send challenge response

receive ID FAILURE
```

## Author

[Kethan Pilla]

## Course Information

CS440-SP25 - Computer Networking
