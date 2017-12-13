nnoremap <F1> :Make -C build --no-print-directory -l<cr>
set makeprg=make\ -C\ build\ --no-print-directory\ -l
let g:syntastic_c_include_dirs = [ 'linkedlist', 'hashtable' ]
