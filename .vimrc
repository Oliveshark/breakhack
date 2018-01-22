nnoremap <F1> :Make<cr>
nnoremap <F2> :Make clean<cr>

au FileType c,h setl makeprg=make\ -C\ build

let g:syntastic_c_include_dirs = [ 'build' ]
