nnoremap <F1> :Make<cr>
au FileType c setl makeprg=ninja\ -C\ build
let g:syntastic_c_include_dirs = [ 'linkedlist', 'hashtable' ]
