nnoremap <F1> :Make<cr>
nnoremap <F2> :Make clean<cr>
nnoremap <F3> :Make lint test<cr>
nnoremap <F4> :!./_build/breakhack<cr>

let g:syntastic_c_include_dirs = [ '_build', '/usr/include/SDL2' ]
