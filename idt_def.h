#ifndef IDT_DEF_H
#define IDT_DEF_H

typedef unsigned int    uint32; //setting these up because it would be
typedef unsigned short  uint16; //confusing to see int short and char being used
typedef unsigned char   uint8;  //to set up an idt entry

struct idt_entry_struct
{
    uint16 base_low16;
    uint16 selector;
    uint8  always0;
    uint8  access;
    uint16 base_high16;
} __attribute__((packed));  //compress for memory efficiency
                            //safety isn't a concern because this all
                            //fits in a relatively neat spot in ram
typedef struct idt_entry_struct idt_entry;

struct idt_ptr_struct
{
    uint16 limit;
    uint32 base;
}__attribute__((packed));
typedef struct idt_ptr_struct idt_ptr;

idt_entry idt[256];
idt_ptr limitStruct;

//asm
void lidtr(idt_ptr *idtr);

//c
void defaultException();
void initIDTEntry(idt_entry *entry, uint32 base, uint16 selector, uint8 access);
void initIDT();

#endif //IDT_DEF_H