global set_pml

set_pml:
 mov cr3, rdi
 ret

global invl_page

invl_page:
 invlpg [rdi]
 ret 

global flush_tlb
flush_tlb:
 mov rax, cr3
 mov cr3, rax
 ret