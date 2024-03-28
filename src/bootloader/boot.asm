ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
; Avoids bios overwriting
_start:
    jmp short start
    nop

times 33 db 0

start:
    jmp 0:run

run:
    cli ; Clear interrupts
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enables interrupts

.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:load32

; GDT
gdt_start:

gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code:           ; CS should point here
    dw 0xffff       ; segment limit first 0 - 15 bits
    dw 0            ; base first 0 - 15 bits
    db 0            ; base 16 - 23 bits
    db 0x9a         ; access byte
    db 11001111b    ; High 4 bit flags and low 4b bit flags
    db 0            ; base 24 - 31 bits

; offset 0x10
gdt_data:           ; DS, SS, ES, FS, GS should point here
    dw 0xffff       ; segment limit first 0 - 15 bits
    dw 0            ; base first 0 - 15 bits
    db 0            ; base 16 - 23 bits
    db 0x92         ; access byte
    db 11001111b    ; High 4 bit flags and low 4b bit flags
    db 0            ; base 24 - 31 bits

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[BITS 32]
load32:
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
    out al, 0x92

    jmp $

times 510-($ - $$) db 0 ; Fills the bytes of the file with 0 up to 510
dw 0xAA55 ; Writes the bios signature

