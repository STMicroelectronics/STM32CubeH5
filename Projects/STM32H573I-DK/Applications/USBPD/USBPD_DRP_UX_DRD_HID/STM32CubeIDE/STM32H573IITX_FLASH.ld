/*
 ******************************************************************************
 **
 ** @file        : LinkerScript.ld
 **
 ** @brief       : Linker script for STM32H573xx Device from STM32H5 series
 **                      2048Kbytes FLASH
 **                      640Kbytes RAM
 **
 ******************************************************************************
 */

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM); /* end of "RAM" Ram type memory */

_Min_Heap_Size = 0x1000; /* required amount of heap */
_Min_Stack_Size = 0x12000; /* required amount of stack */

/* Memories definition */
MEMORY
{
  RAM    (xrw)      : ORIGIN = 0x20000000,   LENGTH = 640K
  FLASH    (rx)     : ORIGIN = 0x8000000,   LENGTH = 2048K
  OSPI1_REGION1(rx): ORIGIN = 0x90000000,   LENGTH = 4K
  OSPI1_REGION2(rx): ORIGIN = 0x90001000,   LENGTH = 4K
  OSPI1_REGION3(rx): ORIGIN = 0x90003000,   LENGTH = 4K
  OSPI1_REGION4(rx): ORIGIN = 0x90004000,   LENGTH = 4K
}

/* Sections */
SECTIONS
{
  /* The startup code into "FLASH" Rom type memory */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH
   
  /* The program code and other data into "FLASH" Rom type memory */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data into "FLASH" Rom type memory */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab (READONLY):
  {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
  } >FLASH

  .ARM (READONLY):
  {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } >FLASH

  .preinit_array (READONLY):
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } >FLASH

  .init_array (READONLY):
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } >FLASH

  .fini_array (READONLY):
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } >FLASH

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections into "RAM" Ram type memory */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.RamFunc)        /* .RamFunc sections */
    *(.RamFunc*)       /* .RamFunc* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */

  } >RAM AT> FLASH

  _ospi_ext_mem1_init_base = LOADADDR(.ospi_ext_mem1);
  _ospi_ext_mem1_init_length = SIZEOF(.ospi_ext_mem1);
  
   .ospi_ext_mem1 :
  {
    . = ALIGN(4);
    _ospi_ext_mem1_start = .;        /* create a global symbol at qspi start */
    *(.ospi_ext_mem1)         /* .ospi_nor sections */
    *(.ospi_ext_mem1*)        /* .ospi_nor* sections */
    . = ALIGN(4);
    _ospi_ext_mem1_ram = .;         /* define a global symbols at end of qspi */
    
  } >OSPI1_REGION1 AT> FLASH
 
 
  _ospi_ext_mem2_init_base = LOADADDR(.ospi_ext_mem2);
  _ospi_ext_mem2_init_length = SIZEOF(.ospi_ext_mem2);
  
   .ospi_ext_mem2 :
  {
    . = ALIGN(4);
    _ospi_ext_mem2_start = .;        /* create a global symbol at qspi start */
    *(.ospi_ext_mem2)         /* .ospi_nor sections */
    *(.ospi_ext_mem2*)        /* .ospi_nor* sections */
    . = ALIGN(4);
    _ospi_ext_mem2_ram = .;         /* define a global symbols at end of qspi */
    
  } >OSPI1_REGION3 AT> FLASH
  
 
  _ospi_ext_cipher_data_base = LOADADDR(.ospi_ext_cipher_data);
  _ospi_ext_cipher_data_length = SIZEOF(.ospi_ext_cipher_data);
  
   .ospi_ext_cipher_data :
  {
    . = ALIGN(4);
    _ospi_ext_cipher_data_start = .;        /* create a global symbol at qspi start */
    *(.ospi_ext_cipher_data)         /* .ospi_nor sections */
    *(.ospi_ext_cipher_data*)        /* .ospi_nor* sections */
    . = ALIGN(4);
    _ospi_ext_cipher_data_ram = .;         /* define a global symbols at end of qspi */
    
  } >OSPI1_REGION2 AT> FLASH
  
  _ospi_ext_plain_data_base = LOADADDR(.ospi_ext_plain_data);
  _ospi_ext_plain_dataa_length = SIZEOF(.ospi_ext_plain_data);
  
   .ospi_ext_plain_data :
  {
    . = ALIGN(4);
    _ospi_ext_plain_data_start = .;        /* create a global symbol at qspi start */
    *(.ospi_ext_plain_data)         /* .ospi_nor sections */
    *(.ospi_ext_plain_data*)        /* .ospi_nor* sections */
    . = ALIGN(4);
    _ospi_ext_plain_data_ram = .;         /* define a global symbols at end of qspi */
    
  } >OSPI1_REGION4 AT> FLASH
  
  /* Uninitialized data section into "RAM" Ram type memory */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough "RAM" Ram  type memory left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM

  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
  .ospi_ext_mem1 : { *(.ospi_ext_mem1) }>OSPI1_REGION1
  .ospi_ext_mem2 : { *(.ospi_ext_mem2) }>OSPI1_REGION3
  .ospi_ext_cipher_data : { *(.ospi_ext_cipher_data) }>OSPI1_REGION2
  .ospi_ext_plain_data : { *(.ospi_ext_plain_data) }>OSPI1_REGION4
}