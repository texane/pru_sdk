--host_image
--image
--host_image:target=ARM
--host_image:endianness=little
--host_image:hidden_symbols

ROMS {
                PAGE 0:
                .text: o = 0x0, l = 0x1000
                PAGE 1:
                .data: o = 0x0, l = 0x1000
}
