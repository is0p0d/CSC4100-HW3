/**************************************
 *Jim Moroney              10.27.2021 *
 *boot2.c                        v2.0 *
 *HW2                         CSC4100 *
 **************************************/

#include "idt_def.h"
#include "kbd_def.h"

#define MAX_COL 80 //i dont like magic numbers
#define MAX_ROW 24 //and global variables are yucky

//Functions written in asm
void k_clearscr();
void k_print(char *string, int str_length, int row, int col);
void k_scroll();
//New to v2.0
void kbd_enter();

//Functions written in C
int primeTest(int p);
void println(char *string);
int convert_num_h(unsigned int num, char buf[]);
void convert_num(unsigned int num, char buf[]);
//New to v2.0
char k_getchar();
void kbd_handler();

int row = 0; // could use pointers to fix this.

int main()
{
    k_clearscr();
    println("JimOS 2.0a");
    println(" ");

    initIDT();
    setupPIC();

    __asm__("sti");

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

int convert_num_h(unsigned int num, char buf[]) 
{
    if (num == 0)
        return 0;

    int idx = convert_num_h((num / 10), buf);
    buf[idx] = num % 10 + '0';
    buf[idx+1] = '\0';
    return idx + 1;
}

void convert_num(unsigned int num, char buf[])
{
    if (num == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
    }
    else
        convert_num_h(num, buf);
}