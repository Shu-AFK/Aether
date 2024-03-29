[BITS 32]
global _start
extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; Enabling the A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Remap the master PIC
    mov al, 00010001b   ; Init flags
    out 0x20, al        ; Tells the master PIC the flags

    mov al, 0x20        ; The new starting point of the interrupt table
    out 0x21, al        ; Tells the master PIC

    mov al, 00000001b   ; Tells the PIC to be in x86 mode
    out 0x21, al

    sti
    call kernel_main

    jmp $

times 512-($ - $$) db 0 ; Removes alignment issues
