/* Скрипт для STM32H745XI (Cortex-M7) с правильным стеком */
ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
    DTCM (rwx)  : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
    .isr_vector : {
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text : {
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .data : {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > DTCM AT > FLASH

    .bss : {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > DTCM

    /* Определяем стек в конце DTCM */
    ._user_heap_stack : {
        . = ALIGN(8);
        . = . + 0x1000;      /* 4KB стека */
        . = ALIGN(8);
        _estack = .;         /* Вершина стека */
    } > DTCM
}
