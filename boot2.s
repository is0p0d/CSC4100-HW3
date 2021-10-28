# boot2.s
# Jim Moroney 
# due: 09.17.2021
# CSC4100 - Operating Systems with Dr. Rogers
# Part of HW1

# NOTE: dont take the flavor text too seriously,
#       it's just how I keep myself entertained while thinking

.intel_syntax noprefix
.global k_print
.global k_scroll
.global k_clearscr
.global kbd_enter
# .global lidtr
# .global outportb
# .global sti_enable

foundKey:   .asciz "Found key: "
numBuffer:  .asciz "------"

# .global k_sqr_root

# i shall rip this rock from its peaceful home
# and shall give it the ability to speak
# when it speaks, it will weep
# hearing its cries, i shall speak softly to it, "i386..."
k_print:
    # moving the stack pointer onto ebp to keep track of arguments
    push ebp
    mov ebp, esp
    # store registers onto the stack to keep everything kosher
    pushf
    push eax
    push ecx 
    push edx
    push edi
    push esi # i paid for every register, im gonna use every register

    # calculation of the screen offset
    # oops
    #                      # (offset = (row * (80 + col)) * 2)
    #  mov eax, [ebp+20]   # moving 4th var (col) into accumulator
    #  add eax, 80         # col + 80
    #  mov edx, [ebp+16]   # moving 3rd var (row) into data register
    #  imul eax, edx       # (col + 80) * row
    #  imul eax, 2         # ((col + 80) * row) * 2
    #                      # offset is now in eax
    #  add eax, 0xB8000    # add the beginning of the ISA hole to the offset
    #  mov edi, eax        # place it in edi because the slides told me to

    # correct equation :^)
                        # (offset = ((row * 80) + col) * 2)
    mov eax, [ebp+16]   # moving 3rd var (row) into accumulator
    mov edx, [ebp+20]   # moving 4th var (col) into data register
    imul eax, 80        # row * 80
    add eax, edx        # (row * 80) + col
    imul eax, 2         # ((row * 80) + col) * 2
                        # offset is now in eax
    add eax, 0xB8000    # add beginning of ISA hole to the offset
    mov edi, eax        # move into edi because the slides told me to

    # printing characters to screen
    mov esi, [ebp+8]    # moving address of string into esi
    mov ecx, [ebp+12]   # moving the second var (str_length)

_contPrint:
    # this rock begins to wail
    # its sentience was forced upon it
    # it did not ask to think, it did not ask to speak
    # loop for printing characters to screen
    cmp edi, 0xB8F9E    # compare edi to end of video mem
    jg _donePrint       # if we're past video memory, clean up and end# 
    cmp ecx, 0          # see if we've satisfied str_length
    je _donePrint       # if we have, clean up and end
    movsb               # move character from string into video memory
    movb es:[edi], 06   # give it some color
    inc edi             # increment to the next spot in video memory
    dec ecx
    jmp _contPrint      # if we're here, nothing else has caught - jump back to the top

_donePrint:
    # the waters begin to calm, the storm is over
    # all returns to normal, the winds are gentle
    # the rock speaks no longer
    # return all registers used to bring the system back to pre-call state
    pop esi
    pop edi
    pop edx
    pop ecx
    pop eax
    popf
    pop ebp
    ret


k_scroll:
    pushad              # push all general purpose registers
    pushf               # push flags
    mov esi, 80* 2 + 0xb8000 # copying a given row into esi
    mov edi, 0xb8000    # moving the beginning of video mem into edi 
    mov ecx, 80 * 24 * 2 # prepping the count register with max addressable video mem
    rep movsb           # repeatedly move ds:esi to es:edi
    mov ecx, 80
    mov al, ' '         # building AX-low with a blank space
    mov ah, 06          # building AX-high with a color
    rep stosw           # storing ax across every column
    popf
    popad
    ret

k_clearscr:
    # the rock, this hunk of silicon
    # it has spoken too much, it vexes me
    # i shall silence it, its thoughts shall be no more
    # write blank spaces onto every possible video memory location
    pushad              # push all general purpose registers
    pushf               # push flags
    mov edi, 0xB8000
    mov ecx, 80 * 25 * 2
    mov al, ' '
    mov ah, 06
    rep stosw
    popf
    popad
    ret

# k_sqr_root:
    # there was another, the rock was not alone
    # talented was this one, numbers it knew well
    # for but one purpose it served, living in the shadow of its friend.
    
    # was going to use the x87 floating point co processor to
    # calculate the minimum number to check for primality
    # but decided 20 primes wasnt worth it

kbd_enter:
    cli
    pushad
    pushf
    in al, 0x64         # Read keyboard controller signal
    and al, 0x01
    jz _kbd_skip
    in al, 0x60         # Read the keyboard's scancode

    push OFFSET foundKey
    call println
    add esp, 4

    push OFFSET numBuffer
    push eax
    call convert_num
    add esp, 4
    call println
    add esp, 4

_kbd_skip:
    mov al, 0x20
    out 0x20, al
    popf
    popad
    iret

# Functions of the broken and damned

# lidtr:
 #   push ebp
 #   mov ebp, esp
 #   pushf
 #   push eax

 #   mov eax, [esp+8]
 #   lidt [eax]

 #   pop eax
 #   popf
 #   pop ebp
 #   ret

# outportb:
 #   push ebp
 #   mov ebp, esp
 #   pushf
 #   push ax
 #   push dx

 #   mov dx, [esp+8]
 #   mov ax, [esp+12]
 #   out dx, al

 #   pop dx
 #   pop ax
 #   popf
 #   pop ebp
 #   ret

# sti_enable:
 #  sti
 #  ret
