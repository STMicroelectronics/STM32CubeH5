#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_shell.h"
#include "qoraal-engine/config.h"
#include "qoraal-engine/engine.h"
#include "qoraal-engine/starter.h"
#include "qoraal-engine/parts/parts.h"


SVC_SHELL_CMD_DECL("engine_list", qshell_cmd_engine_list, "");
SVC_SHELL_CMD_DECL(  "engine", qshell_cmd_engine, "[statemachine] [statemachine] [..]" );
SVC_SHELL_CMD_DECL(  "engine_event", qshell_cmd_engine_event, " <event>" );
SVC_SHELL_CMD_DECL(  "engine_trans", qshell_cmd_engine_trans, "[statemachine] [statemachine] [..]" );
SVC_SHELL_CMD_DECL(  "engine_dbg", qshell_cmd_engine_dbg, "" );

static void
starter_list(void* ctx, starter_list_t type, const char * name, const char* description)
{
    SVC_SHELL_IF_T * pif = (SVC_SHELL_IF_T *) ctx ;
    static starter_list_t t = typeNone ;
    if (t != type) {
        t = type ;
        const char * type_names[] = {"", "Actions", "Events", "Constatnts" } ;
        svc_shell_print (pif, SVC_SHELL_OUT_STD, 
                    "%s:\r\n", type_names[t]) ;

    }

    svc_shell_print (pif, SVC_SHELL_OUT_STD, "    %-24s %s\r\n", 
                    name, description) ;
}

/**
 * @brief       qshell_cmd_run
 * @details     Outputs the version of the Qoraal shell.
 *
 * @param[in]   pif     Shell interface pointer.
 * @param[in]   argv    Command-line arguments.
 * @param[in]   argc    Number of command-line arguments.
 *
 * @return      status  The result of the command execution.
 */
int32_t
qshell_cmd_engine_list (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    starter_parts_list (pif, starter_list) ;
    return SVC_SHELL_CMD_E_OK ;
}


int32_t _engine (SVC_SHELL_IF_T * pif, char** argv, int argc, uint32_t filter)
{
	uint32_t idx = 0 ;
	int found = 0 ;

    if (argc < 2) {
    	engine_loginstance ((uint32_t)-1, 0) ;
    	engine_logfilter (ENGINE_LOG_FILTER_DEFAULT, (uint16_t)-1) ;

		for (idx=0; idx<engine_statemachine_count(); idx++) {

			svc_shell_print(pif, SVC_SHELL_OUT_STD,
					"%s\r\n" ,
					engine_statemachine_name(idx)) ;

		}

		svc_shell_print(pif, SVC_SHELL_OUT_STD,
				"statemachine logging disabled\r\n") ;

		return EOK ;

    } else {

   if (engine_logfilter(0,0) == ENGINE_LOG_FILTER_DEFAULT) {
			engine_logfilter (0, (uint16_t)-1) ;
			engine_loginstance (0, (uint32_t)-1) ;


	}

   for (idx=0; idx<engine_statemachine_count(); idx++) {

		int n  ;

		for (n = 1; n < argc; n++) {

			if (strcmp(engine_statemachine_name(idx), argv[n]) == 0) {
				engine_loginstance (1 << idx, 0) ;
				engine_logfilter (filter, 0) ;
				svc_shell_print(pif, SVC_SHELL_OUT_STD,
						"statemachine '%s' logging enabled\r\n" , engine_statemachine_name(idx)) ;

				found++ ;

			}

		}


		}
	}


    svc_shell_print(pif, SVC_SHELL_OUT_STD,
    		"engine set %d logging instance 0x%x, filter 0x%x\r\n"  ,
			found, engine_loginstance(0,0), engine_logfilter(0,0)) ;

	return EOK ;

}


int32_t qshell_cmd_engine (SVC_SHELL_IF_T * pif, char** argv, int argc)
{

	return _engine (pif, argv, argc, ENGINE_LOG_FILTER_ALL) ;

}

int32_t qshell_cmd_engine_dbg (SVC_SHELL_IF_T * pif, char** argv, int argc)
{

	return engine_logfilter (ENGINE_LOG_FILTER_ALL|ENGINE_LOG_TYPE_DEBUG, 0) ;

}

int32_t qshell_cmd_engine_trans (SVC_SHELL_IF_T * pif, char** argv, int argc)
{

	return _engine (pif, argv, argc, ENGINE_LOG_TYPE_TRANSITIONS) ;

}

int32_t qshell_cmd_engine_event (SVC_SHELL_IF_T * pif, char** argv, int argc)
{

	if (argc < 2) {
		return SVC_SHELL_CMD_E_PARMS ;
	}

	int32_t id = parts_find_event_id (argv[1]) ;
	if (id<0) {
		svc_shell_print(pif, SVC_SHELL_OUT_STD,
				"event '%s' not a valid event name\r\n" , argv[1]) ;
		return SVC_SHELL_CMD_E_FAIL ;

	}

	engine_queue_event (0, (uint16_t)id, 0) ;

	return EOK ;

}


