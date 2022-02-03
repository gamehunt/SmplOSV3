.global __b_asm_set_pml
__b_asm_set_pml:
  movq %rdi, %cr3
  ret
