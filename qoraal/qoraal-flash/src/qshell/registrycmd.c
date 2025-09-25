#include <stdio.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_shell.h"
#include "qoraal-flash/registry.h"

SVC_SHELL_CMD_DECL( "reg", qshell_cmd_reg,  "[entry] [value]");
SVC_SHELL_CMD_DECL( "regadd", qshell_cmd_regadd,  "<entry> <value> [str/int/enum_type]");
SVC_SHELL_CMD_DECL( "regdel", qshell_cmd_regdel,  "<entry>");
SVC_SHELL_CMD_DECL( "regstats", qshell_cmd_regstats,  "");
SVC_SHELL_CMD_DECL( "regrepair", qshell_regrepair,  "");
SVC_SHELL_CMD_DECL( "regerase", qshell_regerase,  "");


#define REGISTRY_VALUE_MAX				192

static void
reg_print (SVC_SHELL_IF_T * pif, REGISTRY_KEY_T key, char* value, uint16_t type, int length)
{
    char tmp[40] ;
    snprintf(tmp, 40, "%s:", key) ;
    if (REGISTRY_GET_TYPE(type) == REGISTRY_TYPE_STRING) {
        svc_shell_print_table (pif, SVC_SHELL_OUT_STD,
            tmp, 24, "[str] %s" SVC_SHELL_NEWLINE, value) ;
    }  else if (REGISTRY_GET_TYPE(type) == REGISTRY_TYPE_INT) {
        svc_shell_print_table (pif, SVC_SHELL_OUT_STD,
            tmp, 24, "[int] %d" SVC_SHELL_NEWLINE, *((int32_t*)value)) ;
    } else if (REGISTRY_GET_TYPE(type) == REGISTRY_TYPE_BLOB) {
        uint8_t tmpvalue[128] ;
        registry_blob_value_get (key, tmpvalue, 128) ;
        svc_shell_print_table (pif, SVC_SHELL_OUT_STD,
            tmp, 24, "[blb] %s" SVC_SHELL_NEWLINE, tmpvalue) ;
    }

}

static uint32_t
reg_show(SVC_SHELL_IF_T * pif, const char * search, char * value, uint32_t len)
{
    REGISTRY_KEY_T key ;
    uint16_t type ;
    uint32_t cnt = 0 ;
    int32_t res = registry_first (&key, value, &type, len) ;
    while ((res >= 0) || (res == E_NOTFOUND)) {

        if (!search || strstr(key, search)) {

            reg_print (pif, key, value, type, len) ;

            cnt++ ;
        }
        res = registry_next (&key, value, &type, len) ;

    }

    return cnt ;
}


int32_t qshell_cmd_reg (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    //uint32_t iterator ;
    //char* pstrentry = 0 ;
    //REGISTRY_KEY_T key ;
    uint16_t type ;
    //int32_t intval = 0 ;
    char value[REGISTRY_VALUE_MAX] ;
    int32_t res = 0 ;

    if (argc == 1) {
        res = reg_show (pif, 0, value, REGISTRY_VALUE_MAX) ;
        svc_shell_print (pif, SVC_SHELL_OUT_STD,
                SVC_SHELL_NEWLINE "        %d entries found." SVC_SHELL_NEWLINE, res) ;

    }
    else if (argc == 2) {

        res = registry_value_get (argv[1], value, &type, REGISTRY_VALUE_MAX) ;
        if (res > 0) {
            reg_print (pif, argv[1], value, type, REGISTRY_VALUE_MAX) ;

        } else {
            res = reg_show (pif, argv[1], value, REGISTRY_VALUE_MAX) ;
            svc_shell_print (pif, SVC_SHELL_OUT_STD,
                    SVC_SHELL_NEWLINE "        %d entries found." SVC_SHELL_NEWLINE, res) ;

        }

    }
    else if (argc == 3) {

        res = registry_set_strval (argv[1], argv[2], REGISTRY_TYPE_NONE) ;

        svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "%s (%d)" SVC_SHELL_NEWLINE, res == EOK ? "OK" : "ERR", res) ;

    }


    return res >= 0 ? SVC_SHELL_CMD_E_OK : SVC_SHELL_CMD_E_FAIL ;
}

int32_t
qshell_cmd_regadd(SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    uint16_t type ;
    int32_t res = SVC_SHELL_CMD_E_OK ;
    int32_t val ;

    if (argc < 3) {
        return SVC_SHELL_CMD_E_PARMS ;

    }

    res = registry_value_length (argv[1]) ;
    if (res > 0 ) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "registry setting %s exists" SVC_SHELL_NEWLINE, argv[1]) ;

        return res ;
    }

    if (argc == 3) {

        if (svc_shell_scan_int (argv[2], (uint32_t*)&val) == EOK) {
            type = REGISTRY_TYPE_INT ;
        } else {
            type = REGISTRY_TYPE_STRING ;
        }
        res = EOK ;

    } else /*if(argc > 3)*/ {

        res = EOK ;
        if (strcmp(argv[3], "str") == 0) {
            type = REGISTRY_TYPE_STRING ;
        } else if (strcmp(argv[3], "int") == 0) {
            type = REGISTRY_TYPE_INT ;
        } else {
            type = REGISTRY_TYPE_ENUM ;
        }

    }

    if (res == EOK) {

        res = registry_set_strval (argv[1], argv[2], REGISTRY_TYPE(type, 0)) ;

        if (res >= 0) {
            svc_shell_print (pif, SVC_SHELL_OUT_STD,
                "%s (%d)" SVC_SHELL_NEWLINE, res == EOK ? "OK" : "ERR", res) ;

        } else {
            svc_shell_print (pif, SVC_SHELL_OUT_STD,
                "error %d for %s not valid" SVC_SHELL_NEWLINE, res, argv[2]) ;

        }

    } else {
        svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "type %s for %s not valid" SVC_SHELL_NEWLINE, argv[3], argv[2]) ;

    }

    return res ;
}

int32_t qshell_cmd_regdel(SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    int32_t res ;

    if (argc != 2) {
        return SVC_SHELL_CMD_E_PARMS ;

    }

    res = registry_value_delete (argv[1]) ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD, "%s (%d)\r\n", res == EOK ? "OK" : "ERR", res) ;

    return SVC_SHELL_CMD_E_OK ;
}

int32_t qshell_regerase (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    int32_t status = registry_erase () ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD, "%s\r\n",
            status == EOK ? "OK" : "ERR") ;

    return SVC_SHELL_CMD_E_OK ;
}

int32_t qshell_regrepair (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    int32_t status = registry_repair () ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD, "%s\r\n",
            status == EOK ? "OK" : "ERR") ;

    return SVC_SHELL_CMD_E_OK ;
}

int32_t qshell_cmd_regstats (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    registry_log_status () ;

    return SVC_SHELL_CMD_E_OK ;
}

void
keep_registrycmds(void)
{
    (void)qshell_cmd_reg ;
    (void)qshell_cmd_regadd ;
    (void)qshell_cmd_regdel ;
    (void)qshell_cmd_regstats ;
    (void)qshell_regerase ;
    (void)qshell_regrepair ;
}