nnoremap <F1> :Make<cr>
nnoremap <F2> :Make clean<cr>
nnoremap <F3> :Make test<cr>
nnoremap <F4> :!./build/breakhack<cr>

let g:syntastic_c_include_dirs = [ 'build', '/usr/include/SDL2' ]
