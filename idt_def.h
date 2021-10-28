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
    uint8  acces;
    uint16 base_high16;
} __attribute__((packed));  //compress for memory efficiency
                            //safety isn't a concern because this all
                            //fits in a relatively neat spot in ram
typedef struct idt_entry_struct idt_entry_t;

#endif //IDT_DEF_H