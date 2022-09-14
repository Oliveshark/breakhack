nnoremap <F1> :Make<cr>
nnoremap <F2> :Make lint test<cr>
nnoremap <F3> :Termdebug _build/debug/breakhack<cr>
nnoremap <F4> :ter ++close env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./ ./_build/debug/breakhack<cr>

packadd termdebug
let g:termdebug_wide = 1

" INDENTING:
set tabstop=4
set shiftwidth=4
set noexpandtab " tabs are tabs, not spaces

au FileType c,h setl tabstop=4 softtabstop=4 shiftwidth=4 noexpandtab
