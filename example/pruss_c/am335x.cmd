--rom_model
--reread_libs
-b
-image
-cr
-stack 0x100
-heap 0x100
-m pru_main.map
-o pru_main.elf

MEMORY
{
    PAGE 0:
    PRUIMEM:   o = 0x00000000  l = 0x00001000
    PAGE 1:
    PRUDMEM:   o = 0x00000000  l = 0x00001000
}

SECTIONS
{
    .text          >  PRUIMEM, PAGE 0
    .stack         >  PRUDMEM, PAGE 1
    .stack         >  PRUDMEM, PAGE 1
    .bss           >  PRUDMEM, PAGE 1
    .cio           >  PRUDMEM, PAGE 1
    .const         >  PRUDMEM, PAGE 1
    .data          >  PRUDMEM, PAGE 1
    .switch        >  PRUDMEM, PAGE 1
    .sysmem        >  PRUDMEM, PAGE 1
    .cinit         >  PRUDMEM, PAGE 1
}
