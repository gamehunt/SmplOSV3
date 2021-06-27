#include <interrupts.h>
#include <io.h>
#include <dev/log.h>
#include <string.h>
#include <types/registers.h>
#include <panic.h>

#define PIC_A 0x20
#define PIC_B 0xA0
#define PIC_A_DATA 0x21
#define PIC_B_DATA 0xA1
#define PIC_ENDOFINT 0x20

static const char *panic_messages[] = {
	"Division by zero",				
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Detected overflow",
	"Out-of-bounds",				
	"Invalid opcode",
	"No coprocessor",
	"Double fault",
	"Coprocessor segment overrun",
	"Bad TSS",						/* 10 */
	"Segment not present",
	"Stack fault",
	"General protection fault",
	"Page fault",
	"Unknown interrupt",			
	"Coprocessor fault",
	"Alignment check",
	"Machine check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

extern void idt_load();

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr127();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

struct idt_descriptor idt_entries[256];
struct idt_ptr        idt_ptr;

static interrupt_handler_t   interrupt_handlers[256];

void set_gate(uint8_t num, uint64_t base, uint8_t sel, uint8_t flags){
    idt_entries[num].zero = 0;
    idt_entries[num].ist  = 0;
    idt_entries[num].selector = sel;
    idt_entries[num].type_attr = flags;
    idt_entries[num].offset_1  =  base        & 0xFFFF;
    idt_entries[num].offset_2  = (base >> 16) & 0xFFFF;
    idt_entries[num].offset_3  = (base >> 32);
}

void setup_interrupts(){
    setup_pic();
    setup_idt();
}

void PIC1_mask_irq(unsigned irq_mask) {
  outb(PIC_A_DATA,inb(PIC_A_DATA) | irq_mask);
}

void PIC2_mask_irq(unsigned irq_mask) {
  outb(PIC_B_DATA,inb(PIC_B_DATA) | irq_mask);
}

void setup_pic(){
  uint8_t mask;
  uint8_t mask1;

  mask  = inb(PIC_A_DATA);
  mask1 = inb(PIC_B_DATA);

  outb(PIC_A, 0x11);          // init in cascade mode
  outb(PIC_B, 0x11);

  outb(PIC_A_DATA, 0x20);
  outb(PIC_B_DATA, 0x28);  // set offset for the 2 PICs

  outb(PIC_A_DATA, 4);                    // tells the master PIC there is a slave PIC at IRQ 2 (0000 0100)
  outb(PIC_B_DATA, 2);        // tells the slave PIC its on the second IRQ (0000 0010)

  outb(PIC_A_DATA, 0x01);
  outb(PIC_B_DATA, 0x01);

  outb(PIC_A_DATA, mask);
  outb(PIC_B_DATA, mask1);

  PIC1_mask_irq(0xF8); //F8
  PIC2_mask_irq(0x6F); //6F
}

void setup_idt(){
    memset(&idt_entries, 0, sizeof(struct idt_descriptor) * 256);

    idt_ptr.pad = 0;
    idt_ptr.offset = (uint64_t) idt_entries;
    idt_ptr.limit  = (uint16_t) (sizeof(struct idt_descriptor) * 256 - 1);

	set_gate(0, (uint64_t)  isr0,  0x08, 0x8E);
	set_gate(1, (uint64_t)  isr1,  0x08, 0x8E);
	set_gate(2, (uint64_t)  isr2,  0x08, 0x8E);
	set_gate(3, (uint64_t)  isr3,  0x08, 0x8E);
	set_gate(4, (uint64_t)  isr4,  0x08, 0x8E);
	set_gate(5, (uint64_t)  isr5,  0x08, 0x8E);
	set_gate(6, (uint64_t)  isr6,  0x08, 0x8E);
	set_gate(7, (uint64_t)  isr7,  0x08, 0x8E);
	set_gate(8, (uint64_t)  isr8,  0x08, 0x8E);
	set_gate(9, (uint64_t)  isr9,  0x08, 0x8E);
	set_gate(10, (uint64_t) isr10, 0x08, 0x8E);
	set_gate(11, (uint64_t) isr11, 0x08, 0x8E);
	set_gate(12, (uint64_t) isr12, 0x08, 0x8E);
	set_gate(13, (uint64_t) isr13, 0x08, 0x8E);
	set_gate(14, (uint64_t) isr14, 0x08, 0x8E);
	set_gate(15, (uint64_t) isr15, 0x08, 0x8E);
	set_gate(16, (uint64_t) isr16, 0x08, 0x8E);
	set_gate(17, (uint64_t) isr17, 0x08, 0x8E);
	set_gate(18, (uint64_t) isr18, 0x08, 0x8E);
	set_gate(19, (uint64_t) isr19, 0x08, 0x8E);
	set_gate(20, (uint64_t) isr20, 0x08, 0x8E);
	set_gate(21, (uint64_t) isr21, 0x08, 0x8E);
	set_gate(22, (uint64_t) isr22, 0x08, 0x8E);
	set_gate(23, (uint64_t) isr23, 0x08, 0x8E);
	set_gate(24, (uint64_t) isr24, 0x08, 0x8E);
	set_gate(25, (uint64_t) isr25, 0x08, 0x8E);
	set_gate(26, (uint64_t) isr26, 0x08, 0x8E);
	set_gate(27, (uint64_t) isr27, 0x08, 0x8E);
	set_gate(28, (uint64_t) isr28, 0x08, 0x8E);
	set_gate(29, (uint64_t) isr29, 0x08, 0x8E);
	set_gate(30, (uint64_t) isr30, 0x08, 0x8E);
	set_gate(31, (uint64_t) isr31, 0x08, 0x8E);
    
    set_gate(32, (uint64_t) irq0,  0x08, 0x8E);
	set_gate(33, (uint64_t) irq1,  0x08, 0x8E);
	set_gate(34, (uint64_t) irq2,  0x08, 0x8E);
	set_gate(35, (uint64_t) irq3,  0x08, 0x8E);
	set_gate(36, (uint64_t) irq4,  0x08, 0x8E);
	set_gate(37, (uint64_t) irq5,  0x08, 0x8E);
	set_gate(38, (uint64_t) irq6,  0x08, 0x8E);
	set_gate(39, (uint64_t) irq7,  0x08, 0x8E);
	set_gate(40, (uint64_t) irq8,  0x08, 0x8E);
	set_gate(41, (uint64_t) irq9,  0x08, 0x8E);
	set_gate(42, (uint64_t) irq10, 0x08, 0x8E);
	set_gate(43, (uint64_t) irq11, 0x08, 0x8E);
	set_gate(44, (uint64_t) irq12, 0x08, 0x8E);
	set_gate(45, (uint64_t) irq13, 0x08, 0x8E);
	set_gate(46, (uint64_t) irq14, 0x08, 0x8E);
	set_gate(47, (uint64_t) irq15, 0x08, 0x8E);

    idt_load();
}

void set_isr_handler(interrupt_handler_t handler, uint8_t interrupt){
	if(interrupt >= 32){
		warning("Tried to assign handler to out-of-range ISR %d", interrupt);
	}else{
		interrupt_handlers[interrupt] = handler;
	}
}
void set_irq_handler(interrupt_handler_t handler, uint8_t interrupt){
	if(interrupt > 15){
		warning("Tried to assign handler to out-of-range IRQ %d", interrupt);
	}else{
		interrupt_handlers[interrupt+32] = handler;
	}
}

void fault_handler(regs_t r){
	if(interrupt_handlers[r->int_no]){
		interrupt_handlers[r->int_no](r);
	}else{
		panic(r, panic_messages[r->int_no]);
	}
}

void irq_end(uint8_t int_no){
	if (int_no >= 8) {
		outb(0xA0, 0x20);
	}
	outb(0x20, 0x20);
}

void irq_handler(regs_t r){
	if(interrupt_handlers[r->int_no]){
		interrupt_handlers[r->int_no](r);
	}
	irq_end(r->int_no - 32);
}