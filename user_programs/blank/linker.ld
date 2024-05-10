ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0x400000; /* the start of the user data segment*/
    .text : ALIGN(4096)
    {
        *(.text)
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }

    .data : ALIGN(4096)
    {
        *(.data)
    }

    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss)
    }

    .asm : ALIGN(4096) /* Ensures correct alignment */
    {
        *(.asm)
    }
}

