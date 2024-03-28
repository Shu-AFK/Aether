ORG 0
BITS 16

;; Avoids bios overwriting
_start:
    jmp short start
    nop

times 33 db 0

start:
    jmp 0x7c0:run

handle_zero: ;; Interrupt 0
    mov ah, 0eh
    mov al, 'A'
    int 0x10
    iret

handle_one:
    mov ah, 0eh
    mov al, 'V'
    int 0x10
    iret

run:
    cli ;; Clear interrupts
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti ;; Enables interrupts

    mov word[ss:0x00], handle_zero
    mov word[ss:0x02], 0x7c0

    mov word[ss:0x04], handle_one
    mov word[ss:0x06], 0x7c0

    int 1

    mov ax, 0x00
    div ax

    mov si, message
    call print
    jmp $ ;; Infitinte loop


print:
    mov bx, 0
.loop:
    lodsb ;; Moves char of string into al and increments si pointer
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah, 0eh
    int 0x10
    ret

message: db 'Hello World!', 0

times 510-($ - $$) db 0 ;; Fills the bytes of the file with 0 up to 510
dw 0xAA55 ;; Writes the bios signature
