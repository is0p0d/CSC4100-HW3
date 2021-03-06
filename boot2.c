/**************************************
 *Jim Moroney              10.27.2021 *
 *boot2.c                        v2.0 *
 *HW2                         CSC4100 *
 **************************************/

typedef unsigned int    uint32; //setting these up because it would be
typedef unsigned short  uint16; //confusing to see int short and char being used
typedef unsigned char   uint8;  //to set up an idt entry

#define NULL ((void* )0) //null ptr
#define TRUE 1
#define FALSE 0
#define MAX_COL 80 //i dont like magic numbers
#define MAX_ROW 24 //and global variables are yucky
#define MAX_BUF 64
#define MAX_QUE 6


//character sets
//===========================================================================
enum CSET_1 {
  Q_PRESSED = 0x10, W_PRESSED = 0x11, E_PRESSED = 0x12, R_PRESSED = 0x13,
  T_PRESSED = 0x14, Y_PRESSED = 0x15, U_PRESSED = 0x16, I_PRESSED = 0x17,
  O_PRESSED = 0x18, P_PRESSED = 0x19
};
static char* cset_1_chars = "qwertyuiop";

enum CSET_2 {
  A_PRESSED = 0x1E, S_PRESSED = 0x1F, D_PRESSED = 0x20, F_PRESSED = 0x21,
  G_PRESSED = 0x22, H_PRESSED = 0x23, J_PRESSED = 0x24, K_PRESSED = 0x25,
  L_PRESSED = 0x26
};
static char *cset_2_chars = "asdfghjkl";

enum CSET_3 {
  Z_PRESSED = 0x2C, X_PRESSED = 0x2D, C_PRESSED = 0x2E, V_PRESSED = 0x2F,
  B_PRESSED = 0x30, N_PRESSED = 0x31, M_PRESSED = 0x32,
};
static char *cset_3_chars = "zxcvbnm";

enum CSET_NUMBERS {
  ONE_PRESSED = 0x2, TWO_PRESSED = 0x3, THREE_PRESSED = 0x4,
  FOUR_PRESSED = 0x5, FIVE_PRESSED = 0x6, SIX_PRESSED = 0x7,
  SEVEN_PRESSED = 0x8, EIGHT_PRESSED = 0x9, NINE_PRESSED = 0xA
};
static char *cset_num_chars = "123456789";

#define CSET_ZERO 0x0B

#define CSET_NL 0x1C
#define CSET_SPC 0x39
#define CSET_RET 0xE
#define CSET_POINT_PRESSED 0x34
#define CSET_SLASH_PRESSED 0x35

//idt structs
//===========================================================================
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


//idt function prototypes
//===========================================================================
void default_exception();
void initIDTEntry(idt_entry *entry, uint32 base, uint16 selector, uint8 access);
void initIDT();
void outportb(uint16 port, uint8 data);
void setupPIC();


//buffer structs
//===========================================================================
struct ring_buffer_struct
{
    char* buffer;
    uint8 first;
    uint8 last;
    uint8 buffMax;

}__attribute__((packed));
typedef struct ring_buffer_struct ring_buffer;

//buffer function prototypes
//===========================================================================
void ring_buff_init(ring_buffer* passedStruct, char* passedBuff, uint8 buffLength);
void ring_buff_push(ring_buffer* buff, char data);
void ring_buff_pop(ring_buffer* buff, char* data);
uint8 ring_buff_isfull(ring_buffer* buff);

//keyboard function prototypes
//===========================================================================
void kbd_enter();                                               //asm boot2.s
void kbd_handler(uint16 scancode);
char translate_scancode(int code);
char k_getchar();

//PCB structs
//===========================================================================
struct ProcessControlBlock_struct
{
    int* ESP;
    int PID;
}__attribute__((packed));
typedef struct ProcessControlBlock_struct PCB;

//PCB function prototypes
//===========================================================================
int create_process(uint32 processEntry);
uint32* allocStack();
PCB* allocPCB();

//queue structs
//===========================================================================
struct queue_struct
{
    PCB* pcb_queue[MAX_QUE];
    uint8 count;
    uint8 head;
    uint8 tail;
}__attribute__((packed));
typedef struct queue_struct queue;

//queue function prototypes
//===========================================================================
void enqueue(PCB* PCBptr);
PCB* dequeue();

//Timer functions
//===========================================================================
void init_timer_dev(uint32 timeDiv);
void dispatch();
void go();
//Utility function prototypes
//===========================================================================
void k_clearscr();                                              //asm boot2.s
void k_print(char *string, int str_length, int row, int col);   //asm boot2.s
void k_scroll();                                                //asm boot2.s
void println(char *string);
int  convert_num_h(unsigned int num, char buf[]);
void convert_num(unsigned int num, char buf[]);
void splashScreen();

//Processes
//===========================================================================
void p1();
void p2();
void p3();
void p4();
void p5();
void pIdle();

//global variables
//===========================================================================
int row = 0;
int col = 0;

idt_entry idt[256];
idt_ptr limitStruct;
ring_buffer kbd_buffer;
char charBuffer[MAX_BUF];

queue processQueue;
PCB* currentPCB;
PCB PCBpool[MAX_QUE];
uint32 progStacks[MAX_QUE][1024];
int process_count = 0;
int num_pid = 0;
int num_stack = 0;
int num_process = 5;

int main()
{
    char ch = '0';
    k_clearscr();
    splashScreen();
    println("Jim M 2021");
    println("==============================");
    println("!!Initializing...");
    initIDT();
    ring_buff_init(&kbd_buffer, charBuffer, MAX_BUF);
    //setupPIC();
    //sti_enable();
    //asm volatile ("sti");
    println("!!Done...");
    println("==============================");
    processQueue.count = 0;
    processQueue.head = 0;
    processQueue.tail = 0;
    int retval = create_process((uint32)&pIdle);

    retval = create_process((uint32)&p1);
    retval = create_process((uint32)&p2);
    retval = create_process((uint32)&p3);
    retval = create_process((uint32)&p4);
    retval = create_process((uint32)&p5);

    go();
    
/*
    println("Start typing:");
    while(1)
    {
        ch = k_getchar();
        if (ch != 0)
        {
            if (ch == '\n')
            {
                row++;
                    if (row > MAX_ROW)
                    {
                        k_scroll();
                        row = MAX_ROW;
                    }
                col = 0;
            }
            else
            {
                k_print(&ch, 1, row, col);
                col++;
                if (col > MAX_COL)
                {
                    row++;
                    if (row > MAX_ROW)
                    {
                        k_scroll();
                        row = MAX_ROW;
                    }
                    col = 0;
                }
            }
        }
    }
*/
}

//idt functions
//===========================================================================
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
            initIDTEntry(&idt[i], (uint32)&dispatch, 0x10, 0x8e);
        else if (i == 33)
            initIDTEntry(&idt[i], (uint32)&kbd_enter, 0x10, 0x8e);
        else    
            initIDTEntry(&idt[i], 0, 0x10, 0x8e);
    }

    setupPIC();
    init_timer_dev(50);

    limitStruct.limit = (sizeof(idt_entry) * 256) - 1;
    limitStruct.base = (uint32)&idt;

    asm volatile ("lidt (%0)" : : "r" (&limitStruct));

    return;

    //lidtr(&limitStruct);
}
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
    outportb(0x21, 0xfc); // Turn on the keyboard & timer IRQ
    outportb(0xA1, 0xff); // Turn off all others
}

//buffer functions
//===========================================================================
void ring_buff_init(ring_buffer* passedStruct, char* passedBuff, uint8 buffLength)
{
    passedStruct->buffer = passedBuff;
    passedStruct->first = 0;
    passedStruct->last = 0;
    passedStruct->buffMax = buffLength;
}
void ring_buff_push(ring_buffer* buff, char data)
{
    uint8 writeNext;
    writeNext = buff->first + 1; //where to point after writing
    //position logic
    if (writeNext >= buff->buffMax)
        writeNext = 0;
    if (writeNext == buff->last) //if buffer is full, overwrite the next position
        buff->last++;
    buff->buffer[buff->first] = data; //Write data to beginning of buffer
    buff->first = writeNext;
}
void ring_buff_pop(ring_buffer* buff, char* data)
{
    uint8 readNext;

    if (buff->first == buff->last) //buffer is empty if this is true
    {
        return;
        println("!!ERROR: ring buffer EMPTY");
    }

    readNext = buff->last + 1; //where to point after reading
    if(readNext >= buff->buffMax)
        readNext = 0;
    
    *data = buff->buffer[buff->last]; //read data
    buff->last = readNext; //move to next offset
}
uint8 ring_buff_isfull(ring_buffer* buff)
{
    if((buff->first)+1 == buff->last) // buff is full
        return 1;
    else if (buff->first == buff->last) // buff is empty
        return 2;
    else // buff has stuff in it
        return 0;
}

//keyboard functions
//===========================================================================
void kbd_handler(uint16 scancode)
{
    if (scancode < 0x2 || scancode > 0x80)
        return;
    else if(scancode == 0 || ring_buff_isfull(&kbd_buffer) == 1)
        return;
    ring_buff_push(&kbd_buffer, translate_scancode(scancode));
}
char translate_scancode(int code)
{
    if(code >= 0x2 && code <= 0xA)
        return cset_num_chars[code - 0x2];
    else if(code >= 0x10 && code <= 0x19)
        return cset_1_chars[code - 0x10];
    else if(code >= 0x1E && code <= 0x26)
        return cset_2_chars[code - 0x1E];
    else if(code >= 0x2C && code <= 0x32)
        return cset_3_chars[code - 0x2C];
    else
    {
        switch(code)
        {
            case 0x0B: 
                return '0';
                break;
            case 0x1C: 
                return '\n';
                break;
            case 0x39: 
                return ' ';
                break;
            case 0xE:  
                return '\n';
                break;
            case 0x34: 
                return '.';
                break;
            case 0x35: 
                return '/';
                break;
            default: 
                break;
        }
    }
}
char k_getchar()
{
    char temp;
    if(ring_buff_isfull(&kbd_buffer) == 2)
        return 0;
    else
    {
        ring_buff_pop(&kbd_buffer, &temp);
        return temp;
    }
}

//queue function prototypes
//===========================================================================
void enqueue(PCB* PCBptr)
{
    processQueue.pcb_queue[processQueue.tail] = PCBptr;
    processQueue.tail++;
    if(processQueue.tail >= MAX_QUE)
    {
        processQueue.tail = 0;
    }
    processQueue.count++;
}
PCB* dequeue()
{
    PCB* tempProc = processQueue.pcb_queue[processQueue.head];
    processQueue.head++;
    if(processQueue.head >= MAX_QUE)
    {
        processQueue.head = 0;
    }
    processQueue.count--;
    return tempProc;
}

//PCB function prototypes
//===========================================================================
int create_process(uint32 processEntry)
{
    if (process_count >= MAX_QUE || num_stack >= MAX_QUE)
        return 1; //error
    
    uint32* stackPtr = allocStack();
    uint32 ss = 24, cs = 16, ds = 8, es = 8, fs = 8, gs = 8;

    uint32* st = stackPtr+1024;
    st--;
    *st = (uint32)&go;
    st--;
    *st = 0x00000200;
    st--;
    *st = cs;
    st--;
    *st = processEntry;
    for (int i = 0; i < 8; i++)
    {
        st--;
        *st = 0; 
    }
    st--;
    *st = ds;
    st--;
    *st = es;
    st--;
    *st = fs;
    st--;
    *st = gs;

    PCB* tempProcess = allocPCB();
    tempProcess->ESP = st;
    tempProcess->PID = num_pid;

    enqueue(tempProcess);
    process_count++;
}
uint32* allocStack()
{
    return progStacks[num_stack++];
}
PCB* allocPCB()
{
    return &PCBpool[num_pid++];
}

//utility functions
//===========================================================================
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
void splashScreen()
{
    println("       .-.,     ,.-.");
    println(" '-.  /:::\\\\   //:::\\  .-'");
    println(" '-.\\|':':' `\"` ':':'|/.-\'");
    println(" `-./`. .-=-. .-=-. .`\\.-`");
    println("   /=- /     |     \\ -=\\");
    println("  ;   |      |      |   ;");
    println("  |=-.|______|______|.-=|");
    println("  |==  \\  0 /_\\ 0  /  ==|");
    println("  |=   /'---( )---'\\   =|");
    println("   \\   \\:   .'.   :/   /");
    println("    `\\= '--`   `--' =/'");
    println("GARF  `-=._     _.=-'");
    println("OS v2.0    `\"\"\"`");
}

//Process implementations
//=============================================================================
void p1()
{
    int i = 0;
    char intBuf[3] = {'0'};
    char* procmsg = NULL;
    while(1)
    {
        procmsg = "process p1: ";
        k_print(procmsg, 12, 19, 0);
        convert_num(i, intBuf);
        k_print(&intBuf, 3, 19, 13);
        i = ((i+1)%500);
    }
}
void p2()
{
    int i = 0;
    char intBuf[3] = {'0'};
    char* procmsg = NULL;
    while(1)
    {
        procmsg = "process p2: ";
        k_print(procmsg, 12, 20, 0);
        convert_num(i, intBuf);
        k_print(&intBuf, 3, 20, 13);
        i = ((i+1)%500);
    }
}
void p3()
{
    int i = 0;
    char intBuf[3] = {'0'};
    char* procmsg = NULL;
    while(1)
    {
        procmsg = "process p3: ";
        k_print(procmsg, 12, 21, 0);
        convert_num(i, intBuf);
        k_print(&intBuf, 3, 21, 13);
        i = ((i+1)%500);
    }
}
void p4()
{
    int i = 0;
    char intBuf[3] = {'0'};
    char* procmsg = NULL;
    while(1)
    {
        procmsg = "process p4: ";
        k_print(procmsg, 12, 22, 0);
        convert_num(i, intBuf);
        k_print(&intBuf, 3, 22, 13);
        i = ((i+1)%500);
    }
}
void p5()
{
    int i = 0;
    char intBuf[3] = {'0'};
    char* procmsg = NULL;
    while(1)
    {
        procmsg = "process p5: ";
        k_print(procmsg, 12, 23, 0);
        convert_num(i, intBuf);
        k_print(&intBuf, 3, 23, 13);
        i = ((i+1)%500);
    }
}
void pIdle()
{
    while(1);
}