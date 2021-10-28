#ifndef KBD_DEF_H
#define KBD_DEF_H

#include "idt_def.h"

//asm
void outportb(uint16 command, uint16 data);

//c
void setupPIC();

#endif //KBD_DEF_H