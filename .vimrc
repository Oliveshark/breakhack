nnoremap <F1> :Make<cr>
nnoremap <F2> :Make clean<cr>

au FileType c setl makeprg=ninja\ -C\ build
au FileType h setl makeprg=ninja\ -C\ build

let g:syntastic_c_include_dirs = [ 'build' ]
