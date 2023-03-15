"""
    The module [ command_line.py ] gathers all class/functions to read the arguments from the command line

    Config file is formatted as an INI file, like this :
        [SECTION_1]
        argument_1 = value
        argument_2 = value
        ...
        [SECTION_n]
        argument_1 = value
        argument_2 = value

"""

import sys
import re # RegEx
import os # basename
import getopt # getopt
sys.path.append('Src/common')
sys.path.append('Src/config')
import console
from common.console import print_colored, bcolors
from colorama import Fore, Back, Style



class ConfigHASH( object ):
    """
        Class ConfigHASH to store input parameters to fill Hash script configuration

    """
    def __init__(self ):
        self.script_verion = ''
        self.script_path = ''
        self.workspace_path = ''
        self.output_folder = 'Output'
        self.isVerbose = False

    def show( self ):
        """
            Method to show all attributes with theirs values
        """
        print( "\tHASH_HAL_Migrator configuration" )
        print( "\tScript version        : {}".format( self.script_verion ) )
        print( "\tScript path           : {}".format( self.script_path ) )
        print( "\tWorkspace path        : {}".format( self.workspace_path ) )
        print( "\tOutput folder path    : {}".format( self.output_folder ) )




















def script_usage():
    script_path = os.path.dirname( os.path.abspath( sys.argv[ 0 ] ) )
    file_usage = open( os.path.join( script_path, "Resources", "script_usage.txt") , "rt")
    if( file_usage ):
        content = file_usage.read()
        for line in content.splitlines():
            print(Fore.LIGHTYELLOW_EX)
            print( line )

        file_usage.close()
        print(Style.RESET_ALL)

def Get_ScriptVersion():
    ret = 0
    parse = False
    msg =""
    scriptVersion = ""
    script_path = os.path.dirname( os.path.abspath(sys.argv[ 0 ]))
    file_usage = open(os.path.join( script_path, "Resources", "Script_Config.txt") , "rt")
    if(file_usage):
        content = file_usage.read()
        for line in content.splitlines():
            if("HASH_HAL_Migrator version" in line) :
                start_index = line.find("\"")
                if (start_index == -1) :
                    parse = False
                    msg = "(start index) HASH_HAL_Migrator version not defined !!"
                else :
                    start_index += 1
                    tmp = line[start_index :]
                    end_index = tmp.find("\"") + start_index
                    if (end_index == -1) :
                        parse = False
                        msg = "(end index) HASH_HAL_Migrator version not defined !!"
                    else :
                        scriptVersion = line[start_index : end_index]
                        parse = True
    file_usage.close()
    if (parse == False) :
        ret = -1
    return ret, msg, scriptVersion


"""
/**
  * @brief  Function reads the command line arguments of the script
  * @param  argv : list contains the used arguments for the script.
  * @retval returns 0 : pass

"""
def read_command_line (argv, scriptVersion):
    ret = 0

    cfg_hash = ConfigHASH()

    if( len(argv) == 0):
        print_colored(bcolors.RED, "\tError   : no entered arguments !!", "")
        ret = -1

    try:
        opts, args = getopt.getopt(sys.argv[1:],"p:",
                                  ["wokspacePath="])
    except getopt.GetoptError as err:
        console.print_error_message(err)
        script_usage()
        sys.exit(-1)

    # Processing command line
    for opt, arg in opts:
        if opt in ['-p', '--wokspacePath']:
            cfg_hash.workspace_path = arg
        else :
            print_colored(bcolors.RED, "\tError   : illegal option !!", opt)
            ret = -1

    # # Analyzing Mandatory arguments
    if(not cfg_hash.workspace_path):
        print_colored(bcolors.RED, "\tError   : Workspace path not specified !!", "")
        ret = -1
    return ret, "", cfg_hash
