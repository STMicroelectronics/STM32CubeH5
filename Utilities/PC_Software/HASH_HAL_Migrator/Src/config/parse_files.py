"""
    The module [ parse_files.py ] gathers all data from IP files
"""

from dataclasses import replace
from operator import index
import os
from posixpath import split
from sre_parse import FLAGS
from turtle import Turtle
from colorama import Fore, Back, Style
from pickle import FALSE, TRUE
import re

from common.console import print_colored, bcolors

HASH_HANDLE = "HASH_HandleTypeDef"

HASH_FUNCTION_PROTOTYPE = "HAL_HASH"
HASH_INIT_FUNCTION_PROTOTYPE = "HAL_HASH_Init"
HASH_DEINIT_FUNCTION_PROTOTYPE = "HAL_HASH_DeInit"
HASH_INIT_DATATYPE = ".Init.DataType"
HASH_TIMEOUT_PARAM = 1
HASH_OUTPUT_BUFFER = "OutputBuffer"
HASH_OUTPUT_BUFFER_DEF = "uint8_t OutputBuffer[];"
TAB_DEF_LOCATION = "uint8_t"

def create_CopyFile(Filename, refFile):
    #read input file
    fin = open(refFile, "rt")
    #read file contents to string
    data = fin.read()
    #close the input file
    fin.close()
    fin = open(Filename , "wt")
    fin.write(data)
    #close the file
    fin.close()
    return 0


def updateFile(file, location, data) :
    #read input file
    fin = open(file, "rt")
    #read file contents to string
    content = fin.read()
    fin.close()
    #new_data = data[old_c_code_start_index:old_c_code_end_index]
    #replace all occurrences of the required string
    content = content.replace(location, data)
    #open the input file in write mode
    fin = open(file, "wt")
    #overrite the input file with the resulting data
    fin.write(content)
    #close the file
    fin.close()
    return 0

"""
/**
  * @brief  Get the necessary files for the hash script
  * @param  ip_path : IP path.
  * @retval returns 0 : pass / ip_header_files / ip_source_files
"""
def Get_Files (ip_path) :
    ip_header_files = []
    ip_source_files = []
    REGEX_PATTERN_IP_FILE = ".[ch]$"
    REGEX_PATTERN_HASH_FILE = "hash*.[ch]$"
    hashSrcfound = False
    hashHeafound = False
    # First, search all IP header files ".h"
    for root, dirs, files in os.walk(ip_path):
        relative_path = os.path.relpath(root, ip_path)
        for file in files:
            if file.endswith('.h'):
                res = re.search( REGEX_PATTERN_IP_FILE, file)
                if res:
                    filename = os.path.join(root, file)
                    print("\t- Search in file [ {} ]... \n".format(os.path.join(relative_path, file)), end = '')
                    index = filename.find("CMSIS")
                    if (index == -1) :
                        ip_header_files.append(filename)
                        print_colored(bcolors.GREEN, "\tfile path : ", filename)
        # Second, search all IP source files ".c"
        for file in files:
            if file.endswith('.c'):
                res = re.search( REGEX_PATTERN_IP_FILE, file )
                if res:
                    filename = os.path.join(root, file)
                    print("\t- Search in file [ {} ]... \n".format(os.path.join(relative_path, file)), end = '')
                    index = filename.find("CMSIS")
                    if (index == -1) :
                        ip_source_files.append(filename)
                        print_colored(bcolors.GREEN, "\tfile path : ", filename)
    # Third, search all Hash source files ".c"
    for root, dirs, files in os.walk(ip_path):
        relative_path = os.path.relpath(root, ip_path)
        for file in files:
            if file.endswith('.c'):
                res = re.search( REGEX_PATTERN_HASH_FILE, file )
                if res:
                    filename = os.path.join(root, file)
                    print_colored(bcolors.BOLD, "\tHash source file path : [ {} ]".format(filename), "")
                    hashSrcfound = True
    #if(hashSrcfound == False) :
            #print_colored(bcolors.RED, "\tHash source file not located in your project workspace!!" , '')
            #return -1, "", ip_header_files, ip_source_files
    # fourth, search all Hash header files ".h"
    for root, dirs, files in os.walk(ip_path):
        relative_path = os.path.relpath(root, ip_path)
        for file in files:
            if file.endswith('.h'):
                res = re.search( REGEX_PATTERN_HASH_FILE, file )
                if res:
                    filename = os.path.join(root, file)
                    print_colored(bcolors.BOLD, "\tHash header file path : [ {} ]".format(filename), "")
                    hashHeafound = True
    #if(hashHeafound == False) :
            #print_colored(bcolors.RED, "\tHash header file not located in your project workspace!!" , '')
            #return -1, "", ip_header_files, ip_source_files
    if(ip_header_files == []) and (ip_source_files == []):
        return -1, "IP files are not found in the given folder !!, verify the IP path", None, None
    return 0, "", ip_header_files, ip_source_files


"""
/**
  * @brief  Get the necessary files for the hash script
  * @param  ip_path : IP path.
  * @retval returns 0 : pass / ip_header_files / ip_source_files
"""
def Get_HashHandle(files) :
    handle_found = False
    handle =""
    for _file in files:
        f = open(_file, 'r')
        n = f.read()
        pars = n.split('\n')
        for line in pars :
            find = line.find(HASH_HANDLE)
            if(find != -1) and (handle_found == False):
                handle_found = True
                tmp = line.split(' ')
                counter = 0
                for element in tmp:
                    counter += 1
                    if(element != '') and (counter != 1):
                        #delete the ; caracter
                        verif = element.find(';')
                        if(verif != -1):
                            index = element.index(';')
                            if(index != -1):
                                handle = element[: index]
                                return 0, "", handle


    return -1, "no handle is defined in your project files !!", ""

"""
/**
  * @brief  Get the necessary files for the hash script
  * @param  ip_path : IP path.
  * @retval returns 0 : pass / ip_header_files / ip_source_files
"""
def updateHashConfig(handle, Src_File, head_File, patch) :
    for _file in Src_File:
        content = ''
        update = FALSE
        counter = 0
        f = open(_file, 'rt')
        n = f.read()
        f.close()
        pars = n.split('\n')
        fileLinesNbre = len(pars)
        for line in pars:
            counter += 1
            #Get DataType value
            nbr = len(patch)
            i = 0
            while(i < nbr) :
                tmp = line.find(patch[i][0])
                if(tmp != -1):
                    var = line.replace(patch[i][0], patch[i][1])
                    update = TRUE
                    #Add a line to configure the used algo for the hash operation
                    line = var
                    print_colored(bcolors.GREEN, "\tHash datatype value updated from [ {} ] to [ {} ] in the file [ {} ]".format(patch[i][0], patch[i][1],_file), "")
                i += 1
            if(fileLinesNbre != counter):
                content += line + '\n'
            else :
                content += line
        if (update == TRUE):
            newFileName = _file.replace('.c', '_orig.c')
            create_CopyFile(newFileName, _file)
            f = open(_file, 'wt')
            f.write(content)
            f.close()
            print_colored(bcolors.BOLD, "\tFile [ {} ] updated correctly ".format(_file), "")

    for _file in head_File:
        content = ''
        update = FALSE
        counter = 0
        f = open(_file, 'rt')
        n = f.read()
        f.close()
        pars = n.split('\n')
        fileLinesNbre = len(pars)
        for line in pars:
            counter += 1
            #Get DataType value
            nbr = len(patch)
            i = 0
            while(i < nbr) :
                tmp = line.find(patch[i][0])
                if(tmp != -1):
                    var = line.replace(patch[i][0], patch[i][1])
                    update = TRUE
                    #Add a line to configure the used algo for the hash operation
                    line = var
                    print_colored(bcolors.GREEN, "\tHash datatype value updated from [ {} ] to [ {} ] in the file [ {} ]".format(patch[i][0], patch[i][1],_file), "")
                i += 1
            if(fileLinesNbre != counter):
                content += line + '\n'
            else :
                content += line
        if (update == TRUE):
            newFileName = _file.replace('.h', '_orig.h')
            create_CopyFile(newFileName, _file)
            f = open(_file, 'wt')
            f.write(content)
            f.close()
            print_colored(bcolors.BOLD, "\tFile [ {} ] updated correctly ".format(_file), "")

    return 0, ""


"""
/**
  * @brief  Get the necessary files for the hash script
  * @param  ip_path : IP path.
  * @retval returns 0 : pass / ip_header_files / ip_source_files
"""
def updateApplication(Src_File, header_File, hash_handler, HashfunctionsArray) :
    functionsNbr = len(HashfunctionsArray)
    for _file in Src_File:
        Functions_found = FALSE
        OUTPUT_DEFINED  = TRUE
        content =''
        linescounter = 0
        update = FALSE
        f = open(_file, 'r')
        n = f.read()
        f.close()
        pars = n.split('\n')
        fileLinesNbre = len(pars)
        for line in pars:
            linescounter +=1
            txt = ''
            hal_function = line.find("HAL_")
            hash_handle_line = line.find(HASH_HANDLE)
            hash_handle_init = line.find(HASH_INIT_FUNCTION_PROTOTYPE)
            outputbuff_location = line.find(TAB_DEF_LOCATION)
            
            if(outputbuff_location != -1) and (OUTPUT_DEFINED == TRUE):
              OUTPUT_DEFINED = FALSE
              newLine = HASH_OUTPUT_BUFFER_DEF + '\n' + line
              line = newLine
            
            if(hal_function != -1):
                counter = 0
                while(counter < functionsNbr):
                    function_def = line.find(HashfunctionsArray[counter][0])    
                    if(function_def != -1) and (HashfunctionsArray[counter][1] != 'NONE') :
                        Functions_found = TRUE
                        alignment = ''
                        space = line.split(' ')
                        for tmp in space :
                            if(tmp != ''):
                                break
                            elif (tmp =='') :
                                alignment += ' '
                        
                        txt = alignment + 'HAL_HASH_GetConfig(&' + hash_handler + ', &ConfHash);\n'
                        txt += alignment + 'ConfHash.Algorithm = ' + HashfunctionsArray[counter][2] + ';\n'
                        txt += alignment + 'HAL_HASH_SetConfig(&' + hash_handler + ', &ConfHash);\n\n'
                        # Check if the function missing a output buffer parameter
                        if(HashfunctionsArray[counter][3] == 'YES') :
                            # <-- start_line  ---- middle_line --- end_line --->

                            #start_line
                            functname_index = line.index(HashfunctionsArray[counter][0]) + len(HashfunctionsArray[counter][0]) + len('(')
                            start_line = line[:functname_index]
                            
                            #middle_line
                            tmp_line = line[functname_index :]
                            para_index = tmp_line.index(')') + functname_index
                            middle_line = line[functname_index : para_index]
                            tmp_endline = line[para_index :]
                            virg_find = tmp_endline.find(',')
                            if(virg_find != -1):
                                tmp_endline = line[para_index + 1 :]
                                endpara_index = tmp_endline.index(')')
                                middle_line = line[functname_index : endpara_index + para_index + 1]

                            #Add the timeout parameter
                            new_middle_line = middle_line + ', ' + HASH_OUTPUT_BUFFER
                            middle_line = new_middle_line
                            
                            #end_line
                            if(virg_find != -1):
                                end_line = line[endpara_index + para_index + 1 : ]
                            else:
                                end_line = line[para_index : ]
                            line  = start_line + middle_line + end_line


                        # Check if the function missing a timeout parameter
                        if(HashfunctionsArray[counter][4] == 'YES') :
                            # <-- start_line  ---- middle_line --- end_line --->
                            #start_line
                            functname_index = line.index(HashfunctionsArray[counter][0]) + len(HashfunctionsArray[counter][0]) + len('(')
                            start_line = line[:functname_index]
                            
                            #middle_line
                            tmp_line = line[functname_index :]
                            para_index = tmp_line.index(')') + functname_index
                            middle_line = line[functname_index : para_index]
                            #Add the timeout parameter
                            new_middle_line = middle_line + ', ' + str(HASH_TIMEOUT_PARAM)
                            middle_line = new_middle_line
                            
                            #end_line
                            end_line = line[para_index : ]
                            line  = start_line + middle_line + end_line

                            
                        txt +=  line.replace(HashfunctionsArray[counter][0], HashfunctionsArray[counter][1])
                                            
                        update = TRUE
                        print_colored(bcolors.GREEN, "\tHash function name updated from [ {} ] to [ {} ] in the file [ {} ]".format(HashfunctionsArray[counter][0],HashfunctionsArray[counter][1],_file), "")
                        break
                    elif (function_def != -1) and (HashfunctionsArray[counter][1] == 'NONE') :
                        print_colored(bcolors.RED, "\tHash function not supported in V2 [ {} ] mentioned in the file [ {} ]".format(HashfunctionsArray[counter][0], _file), "")
                        content += line

                        txt = ''
                        update = TRUE
                        break
                    elif (function_def == -1) and (hash_handle_init != -1):
                        alignment = ''
                        space = line.split(' ')
                        for tmp in space :
                            if(tmp != ''):
                                break
                            elif (tmp =='') :
                                alignment += ' '                    
                        txt = alignment + hash_handler + '.Instance = HASH; \n' + line
                        update = TRUE
                    counter += 1
                if(update != TRUE):
                    txt = line
                content += txt + '\n'
                update = FALSE
            
            elif (hash_handle_line != -1):
                txt = line + '\nHASH_ConfigTypeDef ConfHash;'
                content += txt + '\n'
            else :
                txt = line
                if(fileLinesNbre != linescounter):
                    content += txt + '\n'
                else :
                    content += txt
        if(Functions_found == TRUE):
            Functions_found = FALSE
            #HASH_OUTPUT_BUFFER_DEF

            #CHECK if the file missing the HASH_OUTPUT_BUFFER_DEF
            if(OUTPUT_DEFINED == TRUE) :
                OUTPUT_DEFINED = FALSE
                tmp_content = content
                
            print_colored(bcolors.BOLD, "\tFile [ {} ] updated correctly ".format(_file), "")
            f = open(_file, 'wt')
            f.write(content)
            f.close()

    for _file in header_File:
        content =''
        Functions_found = FALSE
        linescounter = 0
        update = FALSE
        f = open(_file, 'r')
        n = f.read()
        f.close()
        pars = n.split('\n')
        fileLinesNbre = len(pars)
        for line in pars:
            linescounter +=1
            txt = ''
            hal_function = line.find("HAL_")
            hash_handle_line = line.find(HASH_HANDLE)
            if(hal_function != -1):
                counter = 0
                while(counter < functionsNbr):
                    function_def = line.find(HashfunctionsArray[counter][0])
                    if(function_def != -1) and (HashfunctionsArray[counter][1] != 'NONE') :
                        Functions_found = TRUE
                        alignment = ''
                        space = line.split(' ')
                        for tmp in space :
                            if(tmp != ''):
                                break
                            elif (tmp =='') :
                                alignment += ' '

                        txt = alignment + 'HAL_HASH_GetConfig(&' + hash_handler + ', &HashConf);\n'
                        txt += alignment + 'HashConf.Algorithm = ' + HashfunctionsArray[counter][2] + '\n'
                        txt += alignment + 'HAL_HASH_SetConfig(&' + hash_handler + ', &HashConf);\n'
                        txt +=  line.replace(HashfunctionsArray[counter][0], HashfunctionsArray[counter][1])
                        update = TRUE
                        print_colored(bcolors.GREEN, "\tHash function name updated from [ {} ] to [ {} ] in the file [ {} ]".format(HashfunctionsArray[counter][0],HashfunctionsArray[counter][1],_file), "")
                        break
                    elif (function_def != -1) and (HashfunctionsArray[counter][1] == 'NONE') :
                        print_colored(bcolors.RED, "\tHash function not supported in V2 [ {} ] mentioned in the file [ {} ]".format(HashfunctionsArray[counter][0], _file), "")
                        content += line

                        txt = ''
                        update = TRUE
                        break
                    counter += 1
                if(update != TRUE):
                    txt = line
                content += txt + '\n'
                update = FALSE
            
            elif (hash_handle_line != -1):
                txt = line + '\nHASH_ConfigTypeDef HashConf;'
                content += txt + '\n'
            else :
                txt = line
                if(fileLinesNbre != linescounter):
                    content += txt + '\n'
                else :
                    content += txt
        if(Functions_found == TRUE):
            Functions_found = FALSE
            print_colored(bcolors.BOLD, "\tFile [ {} ] updated correctly ".format(_file), "")
            f = open(_file, 'wt')
            f.write(content)
            f.close()

    return 0, ""