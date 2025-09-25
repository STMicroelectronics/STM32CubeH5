
SECTIONS
{

  .qshell BLOCK(4K)   :
  {
    . = ALIGN(32);
    __qshell_cmds_base__ = .;
    KEEP(*(SORT(.qshell.cmds.*)))
    __qshell_cmds_end__ = .;


  }

} 
INSERT AFTER .text;

SECTIONS
{

  .memlog BLOCK(4K)   :
  {
    . = ALIGN(32);
    __memlog_base__ = .;
    . += 10K;
    __memlog_end__ = .;


  }

} 
INSERT AFTER .bss;

