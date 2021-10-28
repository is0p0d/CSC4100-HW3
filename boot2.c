/**************************************
 *Jim Moroney              10.27.2021 *
 *boot2.c                        v2.0 *
 *HW2                         CSC4100 *
 **************************************/

typedef unsigned int    uint32; //setting these up because it would be
typedef unsigned short  uint16; //confusing to see int short and char being used
typedef unsigned char   uint8;  //to set up an idt entry

#define MAX_COL 80 //i dont like magic numbers
#define MAX_ROW 24 //and global variables are yucky


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

//Functions written in asm
void k_clearscr();
void k_print(char *string, int str_length, int row, int col);
void k_scroll();
//new to v2.0
//void lidtr(idt_ptr *idtr);
void outportb(uint16 port, uint8 data);
void kbd_enter();
//void sti_enable();

//Functions written in C
int  primeTest(int p);
void println(char *string);
int  convert_num_h(unsigned int num, char buf[]);
void convert_num(unsigned int num, char buf[]);
//New to v2.0
char k_getchar();
void default_exception();
void initIDTEntry(idt_entry *entry, uint32 base, uint16 selector, uint8 access);
void initIDT();
void setupPIC();
void kbd_handler(uint32 scancode);

//global variables
int row = 0; // could use pointers to fix this.
idt_entry idt[256];
idt_ptr limitStruct;


int main()
{
    k_clearscr();
    println("JimOS 2.0a");
    println(" ");
    println("Initializing...");
    initIDT();
    //setupPIC();
    //sti_enable();
    asm volatile ("sti");
    println("Done!");
    println("Start typing:");


    while(1);
}

void println(char *string)
{
    int num_to_print = 0;

    while (*(string+num_to_print) != '\0')
        num_to_print++;

    while (num_to_print != 0)
    {
        if (num_to_print < MAX_COL)
        {
            k_print(string, num_to_print, row, 0);
            num_to_print = 0;
        }
        else
        {
            k_print(string, MAX_COL, row, 0);
            num_to_print -= MAX_COL;
        }
        row++;
        if (row > MAX_ROW)
        {
            k_scroll();
            row = MAX_ROW;
        }
    }
}

int convert_num_h(uint32 num, char buf[]) 
{
    if (num == 0)
        return 0;

    int idx = convert_num_h((num / 10), buf);
    buf[idx] = num % 10 + '0';
    buf[idx+1] = '\0';
    return idx + 1;
}

void convert_num(uint32 num, char buf[])
{
    if (num == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
    }
    else
        convert_num_h(num, buf);
}

void default_exception()
{
    println("Exception!");
}

void initIDTEntry(idt_entry *entry, uint32 base, uint16 selector, uint8 access)
{
    entry->base_low16 = (uint16)(base & 0xFFFF);
    entry->base_high16 = (uint16)((base >> 16) & 0xFFFF);
    entry->always0 = 0;
    entry->access = access;
    entry->selector = selector;
}

void initIDT()
{
    for (int i = 0; i < 256; i++)
    {
        if (i < 32)
            initIDTEntry(&idt[i], (uint32)&default_exception, 0x10, 0x8e);
        else if (i == 32)
            initIDTEntry(&idt[i], 0, 0x10, 0x8e);
        else if (i == 33)
            initIDTEntry(&idt[i], (uint32)&kbd_enter, 0x10, 0x8e);
        else    
            initIDTEntry(&idt[i], 0, 0x10, 0x8e);
    }

    setupPIC();

    limitStruct.limit = (sizeof(idt_entry) * 256) - 1;
    limitStruct.base = (uint32)&idt;

    asm volatile ("lidt (%0)" : : "r" (&limitStruct));

    //lidtr(&limitStruct);
}

//was easier to understand this in-line for some reason, was having trouble
//with moving stuff into small ports, will ask later.
void outportb(uint16 port, uint8 data)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (data));
}

void setupPIC()
{
    // set up cascading mode:
    outportb(0x20, 0x11); // start 8259 master initialization
    outportb(0xA0, 0x11); // start 8259 slave initialization
    outportb(0x21, 0x20); // set master base interrupt vector (idt 32-38)
    outportb(0xA1, 0x28); // set slave base interrupt vector (idt 39-45)
    // Tell the master that he has a slave:
    outportb(0x21, 0x04); // set cascade ...
    outportb(0xA1, 0x02); // on IRQ2
    // Enabled 8086 mode:
    outportb(0x21, 0x01); // finish 8259 initialization
    outportb(0xA1, 0x01);
    // Reset the IRQ masks
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
    // Now, enable the keyboard IRQ only 
    outportb(0x21, 0xfd); // Turn on the keyboard IRQ
    outportb(0xA1, 0xff); // Turn off all others
}