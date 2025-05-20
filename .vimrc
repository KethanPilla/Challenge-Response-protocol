ettings
syntax enable                " Enable syntax highlighting
set number                   " Display line numbers
set expandtab                " Use spaces instead of tabs
set tabstop=4                " Tabs display as 4 spaces
set shiftwidth=4             " Indentation width is 4 spaces
set softtabstop=4            " Insert 4 spaces for a tab
set autoindent               " Automatically indent new lines
set smartindent              " Enable smart indentation based on syntax
set nowrap                   " Disable line wrapping by default
set colorcolumn=80           " Highlight line at 80 characters to mark line length
set ruler                    " Show the current cursor position
set showcmd                  " Show command in the status bar
syntax on                    " Enable syntax highlighting
set number                   " Display line numbers
set cursorline               " Highlight the current line
highlight CursorLine cterm=NONE ctermbg=darkgray ctermfg=white

" Ensure trailing whitespace is visible
highlight ExtraWhitespace ctermbg=red guibg=red
match ExtraWhitespace /\s\+$/

" Automatically set text width to enforce 80 character limit
set textwidth=80
set colorcolumn=81           " Visually mark the 80th column

" Show line numbers and relative numbers for easy navigation
set number relativenumber

" Enable syntax highlighting
syntax on

" Display line and column numbers in the status bar
set ruler

" Enable mouse support in all modes
set mouse=a

" Set consistent spacing around pointers (this is enforced manually by style)
" No automatic vim setting can enforce pointer spacing explicitly; consider
" a custom linting tool for pointer declaration enforcement.

" Better search highlighting
set hlsearch
set incsearch
set ignorecase
set smartcase

" Quick toggle line wrapping at 80 chars (useful for checking)
nnoremap <F5> :set wrap!<CR>

" Quickly toggle line numbering
nnoremap <F2> :set number!<CR>

" Status line settings
set laststatus=2
set statusline=%F%m%r%h%w\ [%{&ff}]\ [TYPE=%Y]\ [POS=%l,%v][%p%%]\ [LEN=%L]

" Disable swap files (optional but recommended in shared environments)
set noswapfile

" Visualize the 80-character line length limit
set colorcolumn=80

" Autocommands for convenience
augroup filetypedetect
    autocmd!
    autocmd BufNewFile,BufRead *.c set filetype=c
augroup END

" Enable 80 character line boundary highlighting
set colorcolumn=80
highlight ColorColumn ctermbg=darkgray

" Error handling reminder comment for easy insertion
iabbrev ehc /* Check return values and handle errors gracefully */

