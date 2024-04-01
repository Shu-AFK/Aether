section .asm

extern int21h_handler
extern no_interrupt_handler

global int21h
global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts

enable_interrupts:
    sti
    ret

disable_interrupts:
   cli
   ret

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]
    lidt [ebx]

    pop ebp
    ret

; Keyboard interrupt wrapper function
int21h:
    cli
    pushad
    call int21h_handler
    popad
    sti
    iret

; No interrupt handler function
no_interrupt:
    cli
    pushad
    call no_interrupt_handler
    popad
    sti
    iret

