nnoremap <F1> :Make<cr>
nnoremap <F2> :Make lint test<cr>
nnoremap <F3> :Termdebug _build/breakhack<cr>
nnoremap <F4> :ter ++close env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./_build/breakhack<cr>

packadd termdebug
let g:termdebug_wide = 1
let g:syntastic_c_include_dirs = [ '_build', '/usr/include/SDL2', 'steamworks_c_wrapper/src' ]
let g:syntastic_cpp_include_dirs = [ 'steamworks_c_wrapper/sdk/public/steam' ]
