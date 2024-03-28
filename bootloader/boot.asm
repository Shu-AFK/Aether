ORG 0
BITS 16

;; Avoids bios overwriting
_start:
    jmp short start
    nop

times 33 db 0

start:
    jmp 0x7c0:run

run:
    cli ;; Clear interrupts
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti ;; Enables interrupts

    ;; Read for the hard disk
    mov ah, 2 ;; Read sector
    mov al, 1 ;; One sector to read
    mov ch, 0 ;; Cylinder low eight bytes
    mov cl, 2 ;; Read sector 2
    mov dh, 0 ;; Head number
    mov bx, buffer ;; The address, where the content is getting stored in
    int 0x13 ;; Read from hard disk interrupt
    jc error

    mov si, buffer ;; The read content
    call print

    jmp $

error:
    mov si, error_message
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

error_message: db 'failed to load sector', 0

times 510-($ - $$) db 0 ;; Fills the bytes of the file with 0 up to 510
dw 0xAA55 ;; Writes the bios signature

buffer:

