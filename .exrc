let s:cpo_save=&cpo
set cpo&vim
cnoremap <silent> <Plug>(TelescopeFuzzyCommandSearch) e "lua require('telescope.builtin').command_history { default_text = [=[" . escape(getcmdline(), '"') . "]=] }"
imap <M-C-Right> <Plug>(copilot-accept-line)
imap <M-Right> <Plug>(copilot-accept-word)
imap <M-Bslash> <Plug>(copilot-suggest)
imap <M-[> <Plug>(copilot-previous)
imap <M-]> <Plug>(copilot-next)
imap <Plug>(copilot-suggest) <Cmd>call copilot#Suggest()
imap <Plug>(copilot-previous) <Cmd>call copilot#Previous()
imap <Plug>(copilot-next) <Cmd>call copilot#Next()
imap <Plug>(copilot-dismiss) <Cmd>call copilot#Dismiss()
inoremap <C-W> u
inoremap <C-U> u
nnoremap  h
nnoremap <NL> j
nnoremap  k
nnoremap  l
nmap  d
vnoremap <silent>   <Nop>
nnoremap <silent>   <Nop>
omap <silent> % <Plug>(MatchitOperationForward)
xmap <silent> % <Plug>(MatchitVisualForward)
nmap <silent> % <Plug>(MatchitNormalForward)
nnoremap & :&&
xnoremap <silent> <expr> @ mode() == 'V' ? ':normal! @'.getcharstr().'' : '@'
xnoremap <silent> <expr> Q mode() == 'V' ? ':normal! @=reg_recorded()' : 'Q'
nnoremap Y y$
omap <silent> [% <Plug>(MatchitOperationMultiBackward)
xmap <silent> [% <Plug>(MatchitVisualMultiBackward)
nmap <silent> [% <Plug>(MatchitNormalMultiBackward)
omap <silent> ]% <Plug>(MatchitOperationMultiForward)
xmap <silent> ]% <Plug>(MatchitVisualMultiForward)
nmap <silent> ]% <Plug>(MatchitNormalMultiForward)
xmap a% <Plug>(MatchitVisualTextObject)
omap <silent> g% <Plug>(MatchitOperationBackward)
xmap <silent> g% <Plug>(MatchitVisualBackward)
nmap <silent> g% <Plug>(MatchitNormalBackward)
xnoremap gb <Plug>(comment_toggle_blockwise_visual)
nnoremap gb <Plug>(comment_toggle_blockwise)
xnoremap gc <Plug>(comment_toggle_linewise_visual)
nnoremap gc <Plug>(comment_toggle_linewise)
nnoremap <silent> <expr> j v:count == 0 ? 'gj' : 'j'
nnoremap <silent> <expr> k v:count == 0 ? 'gk' : 'k'
xmap <silent> <Plug>(MatchitVisualTextObject) <Plug>(MatchitVisualMultiBackward)o<Plug>(MatchitVisualMultiForward)
onoremap <silent> <Plug>(MatchitOperationMultiForward) :call matchit#MultiMatch("W",  "o")
onoremap <silent> <Plug>(MatchitOperationMultiBackward) :call matchit#MultiMatch("bW", "o")
xnoremap <silent> <Plug>(MatchitVisualMultiForward) :call matchit#MultiMatch("W",  "n")m'gv``
xnoremap <silent> <Plug>(MatchitVisualMultiBackward) :call matchit#MultiMatch("bW", "n")m'gv``
nnoremap <silent> <Plug>(MatchitNormalMultiForward) :call matchit#MultiMatch("W",  "n")
nnoremap <silent> <Plug>(MatchitNormalMultiBackward) :call matchit#MultiMatch("bW", "n")
onoremap <silent> <Plug>(MatchitOperationBackward) :call matchit#Match_wrapper('',0,'o')
onoremap <silent> <Plug>(MatchitOperationForward) :call matchit#Match_wrapper('',1,'o')
xnoremap <silent> <Plug>(MatchitVisualBackward) :call matchit#Match_wrapper('',0,'v')m'gv``
xnoremap <silent> <Plug>(MatchitVisualForward) :call matchit#Match_wrapper('',1,'v'):if col("''") != col("$") | exe ":normal! m'" | endifgv``
nnoremap <silent> <Plug>(MatchitNormalBackward) :call matchit#Match_wrapper('',0,'n')
nnoremap <silent> <Plug>(MatchitNormalForward) :call matchit#Match_wrapper('',1,'n')
nnoremap <Plug>PlenaryTestFile :lua require('plenary.test_harness').test_file(vim.fn.expand("%:p"))
xnoremap <Plug>(comment_toggle_blockwise_visual) <Cmd>lua require("Comment.api").locked("toggle.blockwise")(vim.fn.visualmode())
xnoremap <Plug>(comment_toggle_linewise_visual) <Cmd>lua require("Comment.api").locked("toggle.linewise")(vim.fn.visualmode())
nnoremap <C-K> k
nnoremap <C-J> j
nnoremap <C-H> h
nmap <C-W><C-D> d
nnoremap <C-L> l
inoremap  u
inoremap  u
let &cpo=s:cpo_save
unlet s:cpo_save
set clipboard=unnamedplus
set completeopt=menuone,noselect
set grepformat=%f:%l:%c:%m
set grepprg=rg\ --vimgrep\ -uu\ 
set helplang=en
set nohlsearch
set ignorecase
set noloadplugins
set mouse=a
set packpath=/usr/share/nvim/runtime
set runtimepath=~/.config/nvim,~/.local/share/nvim/lazy/lazy.nvim,~/.local/share/nvim/lazy/indent-blankline.nvim,~/.local/share/nvim/lazy/nvim-lint,~/.local/share/nvim/lazy/lualine.nvim,~/.local/share/nvim/lazy/vim-sleuth,~/.local/share/nvim/lazy/vim-rhubarb,~/.local/share/nvim/lazy/gitsigns.nvim,~/.local/share/nvim/lazy/vim-fugitive,~/.local/share/nvim/lazy/nvim-treesitter-textobjects,~/.local/share/nvim/lazy/nvim-treesitter,~/.local/share/nvim/lazy/telescope-fzf-native.nvim,~/.local/share/nvim/lazy/plenary.nvim,~/.local/share/nvim/lazy/telescope.nvim,~/.local/share/nvim/lazy/nvim-web-devicons,~/.local/share/nvim/lazy/nvim-tree.lua,~/.local/share/nvim/lazy/which-key.nvim,~/.local/share/nvim/lazy/friendly-snippets,~/.local/share/nvim/lazy/cmp_luasnip,~/.local/share/nvim/lazy/LuaSnip,~/.local/share/nvim/lazy/nvim-cmp,~/.local/share/nvim/lazy/cmp-nvim-lsp,~/.local/share/nvim/lazy/neodev.nvim,~/.local/share/nvim/lazy/fidget.nvim,~/.local/share/nvim/lazy/mason-lspconfig.nvim,~/.local/share/nvim/lazy/nvim-lspconfig,~/.local/share/nvim/lazy/nvim-nio,~/.local/share/nvim/lazy/rust-tools.nvim,~/.local/share/nvim/lazy/nvim-dap-go,~/.local/share/nvim/lazy/mason-nvim-dap.nvim,~/.local/share/nvim/lazy/mason.nvim,~/.local/share/nvim/lazy/nvim-dap-ui,~/.local/share/nvim/lazy/nvim-dap,~/.local/share/nvim/lazy/Comment.nvim,~/.local/share/nvim/lazy/copilot.vim,~/.local/share/nvim/lazy/nvim-dap-python,~/.local/share/nvim/lazy/gruvbox,/usr/share/nvim/runtime,/usr/share/nvim/runtime/pack/dist/opt/matchit,/usr/lib64/nvim,~/.local/state/nvim/lazy/readme,~/.local/share/nvim/lazy/indent-blankline.nvim/after,~/.local/share/nvim/lazy/cmp_luasnip/after,~/.local/share/nvim/lazy/cmp-nvim-lsp/after
set smartcase
set statusline=%#Normal#
set tags=./tags;,tags,/usr/include/tags,./tags
set termguicolors
set timeoutlen=300
set undofile
set updatetime=250
set window=41
" vim: set ft=vim :
