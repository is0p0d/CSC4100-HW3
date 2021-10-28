#include "idt_def.h"

void defaultException()
{
    println("Exception!");
}

void initIDTEntry(idt_entry *entry, uint32 base, uint16 selector, uint8 access)
{
    entry->base_low16 = base;
    entry->base_high16 = (base >> 16);
    entry->always0 = 0;
    entry->access = access;
    entry->selector = selector;
}

void initIDT()
{
    for (int i = 0; i < 32; i++)
    {
        initIDTEntry(&idt[0], (uint32)&defaultException, 0x08, 0x8e);
    }
    initIDTEntry(&idt[32], 0, 0x08, 0x8e);
    initIDTEntry(&idt[33], (uint32)&kbd_enter(), 0x08, 0x8e);
    for (int i = 34; i < 256; i++)
    {
        initIDTEntry(&idt[i], 0, 0x08, 0x8e);
    }

    limitStruct.limit = sizeof(idt_entry) * 256 - 1;
    limitStruct.base = (uint32)&idt;

    lidtr(&limitStruct);
}