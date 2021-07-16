#ifndef K_SMPLOS_INTERRUPTS_H
#define K_SMPLOS_INTERRUPTS_H

#include <stdint.h>
#include <util.h>

struct idt_descriptor{
   uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t ist;       // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t type_attr; // type and attributes
   uint16_t offset_2; // offset bits 16..31
   uint32_t offset_3; // offset bits 32..63
   uint32_t zero;     // reserved
} __attribute__((packed));

struct idt_ptr{
    uint16_t pad;
    uint16_t limit;
    uint64_t offset;
}  __attribute__((packed));

typedef void(* interrupt_handler_t) (struct registers *);

CH_START

void setup_interrupts();
void setup_idt();
void setup_pic();
void set_gate(uint8_t num, uint64_t base, uint8_t sel, uint8_t flags);

void set_isr_handler(uint8_t interrupt, interrupt_handler_t handler);
void set_irq_handler(uint8_t interrupt, interrupt_handler_t handler);

void irq_end(uint8_t int_no);

CH_END

#endif