nnoremap <F1> :Make<cr>
nnoremap <F2> :Make lint test<cr>
nnoremap <F3> :Termdebug _build/debug/breakhack<cr>
nnoremap <F4> :ter ++close env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./_build/debug/breakhack<cr>

packadd termdebug
let g:termdebug_wide = 1
let g:syntastic_c_include_dirs = [ '_build/debug', '/usr/include/SDL2', 'steamworks_c_wrapper/src', 'physfs-3.0/src', 'bh_random/src', 'checksum/src' ]
let g:syntastic_cpp_include_dirs = [ 'steamworks_c_wrapper/sdk/public/steam', 'bh_random/src' ]
