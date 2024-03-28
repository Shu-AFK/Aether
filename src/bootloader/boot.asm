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
    cli             ; Clear interrupts
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti             ; Enables interrupts

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

; Loads the kernel into memory and jumps to it
[BITS 32]
load32:
    mov eax, 1          ; Starting sector we want to load from
    mov ecx, 100        ; total number of sectors to load
    mov edi, 0x0100000  ; Address of where the
    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read:
    mov ebx, eax        ; Backup the lba

    shr eax, 24         ; highest 8 bits of lba (Shifting right to 32 - 24 bits)
    or eax, 0xE0        ; Selects the master drive
    mov dx, 0x1f6
    out dx, al          ; sends the highest 8bits to the hard disk controller

                        ; sends the total sectors to read
    mov eax, ecx        ; Number of sectors
    mov dx, 0x1f2
    out dx, al

    mov eax, ebx        ; Restoring the backup lba
    mov dx, 0x1f3
    out dx, al          ; sends more bits to the lda

    mov dx, 0x1f4
    mov eax, ebx        ; Restoring the backuo lba
    shr eax, 8          ; Shifts right to send more bits of the lba
    out dx, al          ; Sending more bits

                        ; Sends upper 16 bits of the lba
    mov dx, 0x1f5
    mov eax, ebx
    shr eax, 16
    out dx, al

    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

    ; Read all sectors into memory
.next_sector:
    push ecx

    ; Checking if we need to read
.try_again:
    mov dx, 0x1f7
    in al, dx
    test al, 8
    jz .try_again

    ; Reading 256 Words at a time
    mov ecx, 256        ; Number of times a word should be read
    mov dx, 0x1f0       ; Port
    rep insw            ; Reading a word from the port specified in dx, storing it in edi
    pop ecx
    loop .next_sector

    ret ; The end of reading the amount of sectors specified in ecx

times 510-($ - $$) db 0 ; Fills the bytes of the file with 0 up to 510
dw 0xAA55               ; Writes the bios signature

