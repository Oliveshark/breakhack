nnoremap <F1> :Make<cr>
nnoremap <F2> :Make lint test<cr>
nnoremap <F3> :Termdebug _build/breakhack<cr>
nnoremap <F4> :ter ++close ./_build/breakhack<cr>

packadd termdebug
let g:termdebug_wide = 1
let g:syntastic_c_include_dirs = [ '_build', '/usr/include/SDL2' ]
