
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include "qoraal/qoraal.h"
#include "qoraal-flash/qoraal.h"
#include "qoraal/example/platform.h"
#include "qoraal/svc/svc_logger.h"
#include "qoraal-engine/starter.h"
#include "qoraal-engine/parts/parts_events.h"
#include "qoraal-flash/registry.h"


#define ENGINE_VERSION_STR      "Navaro Qoraal Engine Demo v '" __DATE__ "'"

#define OPTION_ID_HELP              4
#define OPTION_ID_VERBOSE           6
#define OPTION_ID_LIST              7
#define OPTION_ID_CONFIG_FILE       8
#define OPTION_COMMENT_MAX          256

void            logger_cb (void* channel, LOGGER_TYPE_T type, uint8_t facility, const char* msg) ;
static int32_t  out(void* ctx, uint32_t out, const char* str) ;
static void     list(void* ctx, starter_list_t type, const char * name, const char* description) ;

static const QORAAL_CFG_T       _qoraal_cfg = { .malloc = platform_malloc, .free = platform_free, .debug_print = platform_print, .debug_assert = platform_assert, .current_time = platform_current_time, .rand = platform_rand, .wdt_kick = platform_wdt_kick};
static const QORAAL_FLASH_CFG_T _qoraal_flash_cfg = { .flash_read = platform_flash_read, .flash_write = platform_flash_write, .flash_erase = platform_flash_erase};
static LOGGER_CHANNEL_T         _qoraal_log_channel = { .fp = logger_cb, .user = (void*)0, .filter = { { .mask = SVC_LOGGER_MASK, .type = SVC_LOGGER_SEVERITY_LOG | SVC_LOGGER_FLAGS_PROGRESS }, {0,0} } };

REGISTRY_INST_DECL              (_registry_cfg,  0, (64*1024), 24, 128, 101)


struct option opt_parm[] = {
    { "help",no_argument,0,OPTION_ID_HELP },
    { "verbose",no_argument,0,OPTION_ID_VERBOSE },
    { "list",no_argument,0,OPTION_ID_LIST },
    { "config",required_argument,0,OPTION_ID_CONFIG_FILE },
    { 0,0,0,0 },
};

char *              opt_file = 0 ;
bool                opt_verbose = false ;
bool                opt_list = false ;
char *              opt_config_file = 0;


void
usage(char* comm)
{
    printf (
        "usage:\n"
        "  %s to compile and start an Engine Machine definition file.\n\n"
        "  %s <file> [OPTIONS]\n"
        "    <file>                Engine Machine definition file.\n"
        "    --help                Shows this message.\n"
        "    --verbose             Verbose output.\n"
        "    --list                Lista all Actions, Events and Constants.\n"
        "    --config              Configuration file or \"registry\" (default file.cfg).\n"
        "\n"
        "example: ./build/engine ./test/toaster.e\n",
        ENGINE_VERSION_STR,
        comm);
    exit (0);
}



int
main(int argc, char* argv[])
{
    char c;
    int opt_index = 0;
    int32_t res ;
    printf (ENGINE_VERSION_STR) ;
    printf ("\r\n\r\n") ;




    /*
     * Parse the command line parameters.
     */
    while ((c = getopt_long (argc, (char *const *) argv, "-h", opt_parm, &opt_index)) != -1) {
        switch (c) {
        case 1:
            opt_file = optarg;
            break;

        case 'h':
        case OPTION_ID_HELP:
            usage (argv[0]);
            return 0;

        case OPTION_ID_VERBOSE:
            opt_verbose = true ;
            break;

        case OPTION_ID_LIST:
            opt_list = true ;
            break;

        case OPTION_ID_CONFIG_FILE:
            opt_config_file = optarg ;
            break ;

         }

    }

    if (opt_list) {
        /*
         * Dump all parts loaded
         */
        starter_parts_list (0, list) ;
        if (!opt_file) return 0 ;

    }

    if (!opt_file) {
        /*
         * No Machine Definition File. Exit.
         */
        usage (argv[0]);
        return 0;

    }

    /*
    * Read the Machine Definition File specified on the command line.
    */
    FILE * fp;
    fp = fopen(opt_file, "rb");
    if (fp == NULL) {
        printf("terminal failure: unable to open file \"%s\" for read.\r\n", opt_file);
        return 0;

    }
    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char * buffer = malloc (sz) ;
    if (!buffer) {
        printf("terminal failure: out of memory.\r\n");
        return 0;

    }
    long num = fread( buffer, 1, sz, fp );
    if (!num) {
        printf("terminal failure: unable to read file \"%s\".\r\n", opt_file);
        return 0;

    }
    fclose(fp);

     /*
      * Compile the Machine Definition File and start the Engine.
      */
    printf("starting \"%s\"...\r\n\r\n", opt_file);

    /*
     * Get some of the qoraal modules up and running.
     */
    platform_init (1024*256) ;
    qoraal_init_default (&_qoraal_cfg, 0) ;
    qoraal_flash_init_default (&_qoraal_flash_cfg, &_registry_cfg, 0) ;

    os_sys_start ();

    platform_start () ;
    qoraal_start_default () ;
    qoraal_flash_start_default () ;
    svc_logger_channel_add (&_qoraal_log_channel) ;

    /*
     * Lets get the engine started...
     */
    starter_init (0) ;
    res = starter_start (buffer, sz, 0, out, opt_verbose) ;
    free (buffer) ;

    if (res) {
        printf("starting \"%s\" failed with %d\r\n\r\n",
                opt_file, (int) res);
        goto cleanup ;

    }

    /*
    * Engine is running now. Read the console input and generate events
    * for the characters read. The characters are fired into the Engine as
    * console events.
    */
    do {
        c = getchar() ;
        ENGINE_EVENT_CONSOLE_CHAR(c) ;
    } while (c != 'q') ;

cleanup:
    starter_stop () ;
    svc_logger_wait_all (500) ;
    registry_stop () ;
    platform_stop () ;

     return 0;
}


static void
list(void* ctx, starter_list_t type, const char * name, const char* description)
{
    static starter_list_t t = typeNone ;
    if (t != type) {
        t = type ;
        const char * type_names[] = {"", "Actions", "Events", "Constatnts" } ;
        printf("%s:\r\n", type_names[t]) ;

    }

    printf ("    %-24s %s\r\n", name, description) ;
}

static int32_t
out(void* ctx, uint32_t out, const char* str)
{
    printf ("%s", str) ;
    size_t len = strlen(str) ;
    if (str[len-1] != '\n') printf ("\r\n") ;

    return 0 ;
}

void
logger_cb (void* channel, LOGGER_TYPE_T type, uint8_t facility, const char* msg)
{
    printf("--- %s\n", msg) ;
}