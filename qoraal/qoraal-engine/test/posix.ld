

SECTIONS
{
  .engine BLOCK(4K)   :
  {
		. = ALIGN((32));

    	__engine_action_base__ = .;
         KEEP(*(SORT(.engine.engine_action.*)))
        . = ALIGN(4);
         __engine_action_end__ = .;
         __engine_event_base__ = .;
         KEEP(*(SORT(.engine.engine_event.*)))
        . = ALIGN(4);
         __engine_event_end__ = .;
          __engine_const_base__ = .;
         KEEP(*(SORT(.engine.engine_const.*)))
        . = ALIGN(4);
         __engine_const_end__ = .;
         __engine_cmd_base__ = .;
         KEEP(*(SORT(.engine.engine_cmd.*)))
        . = ALIGN(4);
         __engine_cmd_end__ = .;
  
  }
} 
INSERT AFTER .text;

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