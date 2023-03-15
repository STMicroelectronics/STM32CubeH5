# coding: utf-8

import imp
import sys
import os
import fileinput
import subprocess
from datetime import datetime
from datetime import timedelta

sys.path.append( os.path.dirname(__file__) )
sys.path.append( os.path.join( os.path.dirname(__file__), "src" ) )
sys.path.append('Src/common')
sys.path.append('Src/config')
##### Import Custom Package
import console
import parse_files
import command_line
import compare
from common.console import print_colored, bcolors
from colorama import Fore, Back, Style

OUTPUT_FOLDER_NAME = "GENERATED_FILES"
#------------------------------------------------------------------------------------------------------#


#------------------------------------------------------------------------------------------------------#
    #main
#------------------------------------------------------------------------------------------------------#

def main(argv):
    console.init_console()
    ret = 0
    cfg = None
    ip_path = ''
    path_found = False


    '''
    ======================================================================================================================
    check command line parameters : option + argument = 2
    ======================================================================================================================
    '''
    argv_nb = len(argv)
    if (argv_nb > 2):
        print("the number of used it is ", argv_nb)
        console.print_error_message("Bad command line. Script aborted." )
        command_line.script_usage()
        sys.exit( ret )

    '''
    ======================================================================================================================
    Get script version
    ======================================================================================================================
    '''
    ret, msg, scriptVersion = command_line.Get_ScriptVersion()
    if( ret < 0 ):
        console.print_error_message("[Error] " + msg )
        console.print_error_message("Bad command line. Script aborted." )
        sys.exit( ret )

    print(scriptVersion)
    '''
    ======================================================================================================================
    Analyze command line arguments
    ======================================================================================================================
    '''
    print_colored(bcolors.GREEN, "\tAnalyze command line arguments","")
    ret, msg, cfg = command_line.read_command_line(argv, scriptVersion)
    if( ret < 0 ):
        console.print_error_message("[Error] " + msg )
        console.print_error_message("Bad command line. Script aborted." )
        sys.exit(ret)

    '''
    ======================================================================================================================
    Start the parse mechanism to get all source and header files from the workspace path
    ======================================================================================================================
    '''
    ret, msg, H_files, S_files = parse_files.Get_Files(cfg.workspace_path)
    if (ret < 0) :
        console.print_error_message("[Error] " + msg )
        console.print_error_message("Bad command line. Script aborted." )
        sys.exit( ret )


    '''
    ======================================================================================================================
    Get Hash handle name from source files
    ======================================================================================================================
    '''
    ret, msg, hash_handle = parse_files.Get_HashHandle(S_files)
    if (ret < 0) :
        console.print_error_message("[Error] " + msg )
        console.print_error_message("Bad command line. Script aborted." )
        sys.exit( ret )

    '''
    ======================================================================================================================
    HASH config update from V1 to V2
    ======================================================================================================================
    '''
    ret, msg = parse_files.updateHashConfig(hash_handle, S_files, H_files, compare.Defines_CMP_Array)
    if (ret < 0) :
        console.print_error_message("[Error] " + msg )
        console.print_error_message("Bad command line. Script aborted." )
        sys.exit( ret )


    '''
    ======================================================================================================================
    HASH app update from V1 to V2
    ======================================================================================================================
    '''

    ret, msg = parse_files.updateApplication(S_files, H_files, hash_handle, compare.Functions_CMP_Array)
    if (ret < 0) :
        console.print_error_message("[Error] " + msg )
        console.print_error_message("Bad command line. Script aborted." )
        sys.exit( ret )


    '''
    ======================================================================================================================
    __main__
    ======================================================================================================================
    '''
if __name__ == "__main__":
   main(sys.argv[1:])
