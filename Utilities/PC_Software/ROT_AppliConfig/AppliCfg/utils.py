import sys
import re
import time
import hashlib
from AppliCfg import macro_parser

def get_hex(value, size=None, upper_format=True):
    """
        Transform value to hex number in upper mode
    Parameters:
        value - Value to transform
    Returns:
        hex_value - Value in hex mode
    """
    if type(value) == str :
        value = value.lower()
        if "0x" in value :
            hex_value = value[2:]
        else :
            hex_value = hex(convert_to_integer(value))[2:]
    else :
        hex_value = hex(value)[2:]

    if size is not None and len(hex_value) <= size :
        hex_value = format(int(hex_value,16), 'x').zfill(size)

    if upper_format :
        hex_value = hex_value.upper()

    return "0x" + hex_value

def compute_condition(condition, values, vb):
    """
        Compute a conditional expression (string to integer)
    Parameters:
        condition - Value to transform
        value - Value found in the input file. The value must be integer
        vb - Debug option (True or False)
    Returns:
        conditional_value : Return True or False
    """
    # Replace constants string patterns to real values
    LOG.info("Condition to be computed '%s' " %condition, vb)


    # Replace value string pattern to real values
    only_one_value = True if type(values) == int else (True if len(values) == 1 else False)
    if only_one_value:
        new_value = str(values) if type(values) == int else str(values[0])
        """ use "value" as search pattern for the single val
        to keep compatibility with all script using 
        a single value , then try replacement with val1 if new syntax is used
        """
        value_pattern = "value" if "value" in condition else "val1"
        condition = condition.replace(value_pattern, new_value)
    else:
        i = 0
        for value in values:
            i +=1
            pattern = "val%s" %i
            condition = condition.replace(pattern, str(value))

    LOG.info("Condition patterns were replaced to '%s' " %condition, vb)
    try :
        return eval(condition)
    except Exception as e:
        LOG.error("Evaluation condition error '%s'" % e)


def compute_expression(expression, values, constants, vb):
    """
        Compute a mathematical expression (string to integer)
    Parameters:
        expression - Value to transform
        value - Value found in the input file. The value must be integer
        constants - Constant list. The constant must be integers
        vb - Debug option (True or False)
    Returns:
        computed_value : Computed value in integer format
    """
    # Replace constants string patterns to real values
    LOG.info("Expression to be computed '%s' " %expression, vb)
    i = 0
    for cons in constants:
        i += 1
        pattern = "cons%s" %i
        expression = expression.replace(pattern, str(cons))

    # Replace value string pattern to real values
    only_one_value = True if type(values) == int else (True if len(values) == 1 else False)
    if only_one_value:
        new_value = str(values) if type(values) == int else str(values[0])
        """ use "value" as search pattern for the single val
        to keep compatibility with all script using 
        a single value , then try replacement with val1 if new syntax is used
        """
        value_pattern = "value" if "value" in expression else "val1"
        expression = expression.replace(value_pattern, new_value)
    else:
        i = 0
        for value in values:
            i +=1
            pattern = "val%s" %i
            expression = expression.replace(pattern, str(value))

    LOG.info("Expression patterns were replaced to '%s' " %expression, vb)
    try :
        return int(eval(expression))
    except Exception as e:
        LOG.error("Evaluation expression error '%s'" % e)

def stirot_compute_wm(code_primary_offset, secure_area_size, 
                      pagesize, bankSize, vb):
    """
    Compute the value of the OptionByte "Secure WaterMark". Two values are
    necessary to compute this protection :
    Parameters:
        code_primary_offset - code primary offset
        secure_area_size - secure area size
        pagesize - Board page size (Defined in the reference manual)
        bankSize - Banks size for HDP protection (Defined in the reference manual)
        vb - Debug option (True or False)
    Returns:
        wm_config - Dictionary with Watermarks values
    """
    wm_config = {}
    sector_size = pagesize
    bank_size = int(bankSize / sector_size)
    secure_sector_size = int((int(secure_area_size, 16) - sector_size)/ sector_size)
    if secure_sector_size < 0:
        sys.exit("Wrong secure area size '%s'" % secure_area_size)
    # Compute WM's start and end protections
    wm_str = int(int(code_primary_offset, 16) / sector_size)
    wm_end = wm_str + secure_sector_size
    # Protection only in bank 1
    if wm_end <= bank_size:
        wm_config["SECWM1_STRT"] = get_hex(wm_str)
        wm_config["SECWM1_END"] = get_hex(wm_end)
        wm_config["SECWM2_STRT"] = "0x7F"
        wm_config["SECWM2_END"] = "0x0"
    else:
        if wm_str <= bank_size:
            # Protection between bank 1 and bank 2
            wm_config["SECWM1_STRT"] = get_hex(wm_str)
            wm_config["SECWM1_END"] = "0x7F"
            wm_config["SECWM2_STRT"] = "0x0"
        else:
            # Disable protection in the bank 1
            wm_config["SECWM1_STRT"] = "0x7F"
            wm_config["SECWM1_END"] = "0x0"
            # Protection in bank 2
            wm_str -= bank_size# + 1
            #wm_end -= 1 
            wm_config["SECWM2_STRT"] = get_hex(wm_str)
        wm_end -= bank_size
        wm_config["SECWM2_END"] = get_hex(wm_end)

    return wm_config


def compute_wrp_protections(wrp_address_start, wrp_address_end, wrp_sector_sz,
                            sector_nb, vb, sector_size="0x2000"):
    """
        Compute Sector group write-protection (WRPSG). Protecting up to 
                (sector_nb/(wrp_sector_size/sector_size)) groups of 
                (wrp_sector_size/sector_size) sectors 
    Parameters:
        wrp_address_start - Begin address of wrp protection
        wrp_address_end - End address of wrp protection
        wrp_sector_sz -Size of the wrp group
        sector_nb - Board sector number in one bank
        vb - Debug option (True or False)
        sector_size - Board sector size
    Returns:
        wrpsg1 - Write sector group protection for Bank 1
        wrpsg2 - Write sector group protection for Bank 2
    """
    # Transform initial values to decimal
    LOG.info("Computing the wrp protection."+ \
             " See the reference manual for more information", vb)
    sector_sz = int(sector_size, 16)
    # Compute the start and end sectors (with the wrp group size)

    wrp_start = int(wrp_address_start / wrp_sector_sz)
    wrp_end = int(wrp_address_end / wrp_sector_sz)
    # Total number of sectors group in bank 1 and 2
    wrp_group_nb = (
        int((sector_nb + 1) / int(wrp_sector_sz / sector_sz)) * 2) - 1

    wrp = ""
    for i in range(wrp_group_nb, -1, -1):
        # 0: the group is write protected; 1: the group is not write protected
        wrp += "0" if wrp_start <= i <= wrp_end else "1"

    # Transform binary to dec
    wrpg1_end = int((wrp_group_nb + 1)/2)
    wrpsg1 = int(wrp[:wrpg1_end], 2)
    wrpsg2 = int(wrp[wrpg1_end:], 2)
    return wrpsg1, wrpsg2


def compute_hdp_protection(hdp_address_start, hdp_address_end, sector_nb, vb, hdp_sector_sz:int=0x2000):
    """
        Compute the HDP protection
    Parameters:
        hdp_address_start - Begin address of hdp protection
        hdp_address_end - End address of hdp protection
        sector_nb - Board sector number in one bank
        vb - Debug option (True or False)
        sector_size - Board sector size (Defined in the reference manual)
    Returns:
        hdp1 - Tuple with start and and values
        hdp2 - Tuple with start and and values
    """
    # Compute the start and end sectors (with the wrp group size)
    LOG.info("Computing the hdp protection." + \
            " See the reference manual for more information", vb)
    #hdp_sector_sz = int(sector_size, 16)
    hdp_start = int(hdp_address_start / hdp_sector_sz)
    hdp_end = int(hdp_address_end / hdp_sector_sz)
    # Compute sectors to protect
    if hdp_start > sector_nb:
        hdp1 = sector_nb, 0
        hdp2 = sector_nb - hdp_start, sector_nb - hdp_end
    else:
        if hdp_end > sector_nb:
            hdp1 = hdp_start, sector_nb
            hdp2 = 0, hdp_end - sector_nb - 1
        else:
            hdp1 = hdp_start, hdp_end
            hdp2 = sector_nb, 0
    return hdp1, hdp2

def compute_sector_area(sector_address_start, area_size, vb, sector_size):
    """
        Compute the HDP protection
    Parameters:
        hdp_address_start - Begin address of hdp protection
        hdp_address_end - End address of hdp protection
        sector_nb - Board sector number in one bank
        vb - Debug option (True or False)
        sector_size - Board sector size (Defined in the reference manual)
    Returns:
        hdp1 - Tuple with start and and values
        hdp2 - Tuple with start and and values
    """
    offset_1=0x70000000
    offset_2=0x90000000
    if (offset_1 & convert_to_integer(sector_address_start) == offset_1):
        sel_offset=offset_1
    elif (offset_2 & convert_to_integer(sector_address_start) == offset_2):
        sel_offset=offset_2
    else:
        LOG.error("Sector address start is unknown '%s'" % sector_address_start)

    if convert_to_integer(area_size) < sector_size:
        decrement = 0
    else:
        decrement = 1

    # Compute the start and end sectors (with the wrp group size)
    LOG.info("Computing the erase slot area.", vb)
    slot_sector_sz = sector_size
    LOG.info("Sector size %d" % sector_size, vb)
    slot_start = (convert_to_integer(sector_address_start) - sel_offset) / slot_sector_sz
    LOG.info("Slot start %d" % slot_start, vb)
    slot_end = (convert_to_integer(sector_address_start)- sel_offset + convert_to_integer(area_size)) / slot_sector_sz - decrement
    LOG.info("Slot end %d with sector_size %d" % (slot_end,slot_sector_sz), vb)
    # Compute sectors to erase
    erase = int(slot_start), int(slot_end)
    return erase


def modify_file_line(infile, begin_line, value, vb, delimiter="="):
    """
        Modify the entire line of a file. Recommended to modify script variables
        Parameters:
            infile - File to update
            begin_line - Pattern to select the line to modify
            value - value to be added to the new line
            vb - Debug option (True or False)
        Returns:
            line_is_modified - Boolean status (True line modified)
    """
    line_is_modified = False
    # Read and modify a specific line
    f = open(infile, "r")
    lines = f.readlines()
    for i, line in enumerate(lines):
        if line.startswith(begin_line):
            line_is_modified = True
            if begin_line + value in line:
                # The line has te correct value the file 
                # modification is not necessary
                value = value.replace(delimiter,"")
                str_debug = "The variable has already the correct value (%s)" % value
                LOG.info("The file modification is not necessary", vb)
                LOG.info(str_debug, vb)
                return True
            else:
                # Line to modify has been found (modify only the first line
                # with the pattern )
                lines[i] = begin_line + value + "\n"
                LOG.info("The variable value was modified to %s" % value.replace(delimiter,""), vb)
                break
    # Save file modification
    LOG.info("Saving file modification", vb)
    f = open(infile, "w")
    f.write("".join(lines))
    f.close()

    return line_is_modified


def toggle_comment_line(infile, name, vb, action="comment"):
    """ 
    Modify the value of an existing variable in a file 
    Parameters:
        infile - File to be updated
        name -   Define variable to comment or uncomment
        vb -     Debug option (True or False)
        action - comment or uncomment the file line
    Returns:
        line_is_modified - Boolean status (True line with correction action)
    """
    line_is_modified = False
    # Modify value
    # Read and modify a specific line
    f = open(infile, "r")
    lines = f.readlines()
    for i, line in enumerate(lines):
        # Search define with value
        pattern_in_line = re.search('#define\s* (\w+)\s* (\w+)', line)

        if pattern_in_line is None:
            pattern_in_line = re.search('#define\s* (\w+)', line)

        if pattern_in_line:
            if name in pattern_in_line.group(1):
                line_is_modified = True
                str_idx, end_indx = pattern_in_line.span()
                if action == "comment":
                    if str_idx == 0:
                        new_line = "/*" + \
                                   line[str_idx:end_indx] + \
                                   "*/" + line[end_indx:]
                        LOG.info("The #define variable is now commented", vb)
                    elif "/*" in line[:str_idx]:
                        LOG.info("The #define variable is already commented", vb)
                        new_line = line
                    else:
                        new_line = line[:str_idx] + "/*" + \
                            line[str_idx:end_indx] + "*/" + line[end_indx:]
                        LOG.info("The #define variable is now commented", vb)
                else:
                    if str_idx == 0:
                        new_line = line
                        LOG.info("The variable is already defined", vb)
                    elif "/*" in line[:str_idx]:
                        new_line = line.replace(
                            "/*", "", 1).replace("*/", "", 1)
                        LOG.info("The variable is now defined", vb)
                    else:
                        new_line = line
                        LOG.info("The variable is already defined", vb)
                if new_line in lines[i]:
                    LOG.info("The modification is not necessary", vb)
                    # The line has te correct define configuration
                    # The file modification is not necessary
                    return True
                else : 
                    lines[i] = new_line
                    line_is_modified = True
                    # Line to modify has been found (modify only the first line
                    # with the pattern )
                    break
    if line_is_modified :
        LOG.info("Saving the define modifications", vb)
        # Save file modification
        f = open(infile, "w")
        f.write("".join(lines))
        f.close()
    return line_is_modified


def get_file_value(infile, pattern_line, search_value, search_option="((0x[0-9a-fA-F]+)|([0-9]+))"):
    """
    Get the value of an existing variable in a file
    Parameters:
        infile - File to be updated
        pattern_line - Pattern to choose the line to be modified
        vb - Debug option (True or False)
        search_option = Search regex option (By default search hex an decimal numbers)
    Returns:
        value_is_modified - Boolean status (True value modified)
    """
    value_is_modified = False
    # Modify value
    # Read and modify a specific line
    f = open(infile, "r")
    lines = f.readlines()
    for i, line in enumerate(lines):
        # Search line to modify
        if re.search(pattern_line, line):
            # Get the value to be replaced
            replace_resp = re.search(pattern_line +search_option, line)
            if replace_resp is not None:
              return replace_resp.group(1)
            else:
                sys.exit("Wrong option to replace while searching the pattern")
    return ""

def modify_file_value(infile, pattern_line, search_value, new_value, vb,
                      search_option="((0x[0-9a-fA-F]+)|([0-9]+))"):
    """ 
    Modify the value of an existing variable in a file 
    Parameters:
        infile - File to be updated
        pattern_line - Pattern to choose the line to be modified
        search_value - Pattern to search in the line previously selected
                      (This allows also to get te value to be replaced)
        new_value - New value
        vb - Debug option (True or False)
        search_option = Search regex option (By default search hex an decimal numbers)
    Returns:
        value_is_modified - Boolean status (True value modified)
    """
    value_is_modified = False
    # Modify value
    # Read and modify a specific line
    f = open(infile, "r")
    lines = f.readlines()
    for i, line in enumerate(lines):
        # Search line to modify
        if re.search(pattern_line, line):
            # Get the value to be replaced
            replace_resp = re.search(search_value + search_option, line)
            if replace_resp is not None:
                # Replace the value if value to replace is a hex or dec number
                replace_value = replace_resp.group(1)
                str_idx, end_indx = replace_resp.span()
                str_to_replace = line[str_idx: end_indx].replace(
                    replace_value, new_value)
                new_line = line[:str_idx] + str_to_replace + line[end_indx:]
                if new_line in lines[i]:
                    # The variable has the correct value
                    # The file modification is not necessary
                    str_debug = "The variable has already the correct value (%s)" % new_value
                    LOG.info("The modification is not necessary", vb)
                    LOG.info(str_debug, vb)
                    return True
                else : 
                    LOG.info("The variable value was modified to %s" % new_value, vb)
                    lines[i] = new_line
                    value_is_modified = True
                    # Line to modify has been found (modify only the first line
                    # with the pattern )
                    break
            elif replace_resp is None:
                # Empty value to replace, then add the new value
                replace_resp = re.search(search_value, line)
                if replace_resp is not None:
                    str_idx, end_indx = replace_resp.span()
                    if "(" in line and ")" in line and not ";" in line:
                        new_line = line[:end_indx] + new_value + line[end_indx:]
                    elif ";" in line:
                        new_line = line[:end_indx] + new_value + line[end_indx:]
                    else:
                        new_line = line[:end_indx - 1] + \
                            new_value + line[end_indx - 1:]
                    if new_line in lines[i]:
                        # The variable has the correct value
                        # The file modification is not necessary
                        str_debug = "The variable has already the correct value (%s)" % new_value
                        LOG.info("The modification is not necessary", vb)
                        LOG.info(str_debug, vb)
                        return True
                    else :
                        lines[i] = new_line
                        LOG.info("The variable value was modified to %s" % new_value, vb)
                        value_is_modified = True
                        # Line to modify has been found (modify only the first
                        # line with the pattern )
                        break
            else:
                sys.exit("Wrong option to replace while searching the pattern")

    if value_is_modified :
        LOG.info("Saving the define modifications", vb)
        # Save file modification
        f = open(infile, "w")
        f.write("".join(lines))
        f.close()
    return value_is_modified


def get_macro_value(macro, layout, vb):
    """ 
    Search one variable in the macro document and return an integer value 
    Parameters:
        macro  - variable to search and get the value
        layout - file
        vb - Debug option (True or False)
    Returns :
        value - value found (None if no value)
    """
    import os.path

    ext = os.path.splitext(layout)[1][1:]
    if ext=="h":
        image_value_re = re.compile(r"^\#define\s*" + macro + "\s\s*(.*)")
    elif ext=="icf":
        image_value_re = re.compile(r"^define symbol \s*" + macro + "\s*=\s*(.*)")
    else:
        image_value_re = re.compile(r"^\s*" + macro + "\s*=\s*(.*)")

    try:
        layout_dbg = layout.split("\\")[-1] if "\\" in layout else layout
        LOG.info("Opening the '%s' file" %layout_dbg, vb)
        value = macro_parser.evaluate_macro(layout, image_value_re, 0, 1)
        if value is not None:
            debug_msg = "The value computed for '%s'" % macro + " is '%s'" % get_hex(value)
            LOG.info(debug_msg, vb)
    finally:
        pass
    return value


def convert_to_integer(value):
    """Transform input data into integer notation"""
    try:
        if type(value) is int:
            return value
        if value[:2].lower() == "0x":
            return int(value[2:], 16)
        elif value[:1] == "0":
            return int(value, 8)
        return int(value, 10)
    except:
        raise

def parse_command_input(command, vb):
    """ Parse input command to reduce the size of the file paths """
    if vb :
        new_cmd = []
        for cmd in command:
            cmd = cmd.split("\\")[-1] if "\\" in cmd else cmd
            new_cmd.append(cmd)
        LOG.info(" ".join(new_cmd), True)

def data_to_bytes(hex_value, order, byte_size):
    """ Transform hex value to byte value
        Parameters:
          hex_value
          order : little or big (endian)
          byte_size : Size of data in bytes (ex.uint32 --> 4)
        """
    int_value = int(hex_value, 16)
    # Create four bytes from the integer
    bytes_value = int_value.to_bytes(byte_size, byteorder=order)
    return bytes_value
  
def compute_sha256(data_bytes, vb, get_hex_val=False):
    """ Compute the sha256. Return the hex or byte value, depending the
       value of get_hex_val """
    sha256_list = []
    sha256_bytes = b""
    #Compute Hash256
    hash256 = hashlib.sha256()
    hash256.update(data_bytes)
    hash256 = hash256.hexdigest();
    LOG.info("SHA256 computed : " + hash256, vb)
    for i in range(0,len(hash256),2):
      sha256_list.append(hash256[i]+ hash256[i+1])
    #Transform hash hex values to bytes
    if get_hex_val:
        return sha256_list
    else :
        for i in range(len(sha256_list)):
            sha256_bytes += data_to_bytes(sha256_list[i], "big", 1)
        return sha256_bytes

def modify_bin_content(infile, offset, val, vb):
    """ Modify or add byte in the  binary file. If offset is empty, 
        one byte is added at the end of the file otherwise the byte is added at 
        offset position"""
    with open(infile, "r+b") as f:
        f.seek(offset) # Set file pointer to offset position
        str_debug = "Data will be added at the " + str(offset) + " index"
        LOG.info(str_debug, vb)
        f.write(val)
    return 1


def print_debug(msg):
    utf8stdout = open(1, 'w', encoding='utf-8', closefd=False) # fd 1 is stdout
    print(str(time.strftime("%c")) + " : " + msg, file=utf8stdout, flush=True)

class LOG():
    def info(msg, is_debug):
        if is_debug:
            print_debug("[INF] " + msg.replace("\n","").replace("\r",""))
    def error(msg):
        sys.exit("[ERR] " + msg.replace("\n","").replace("\r",""))


def data_format(bytes_value, order):
    """ Transform byte value to hex value
        Parameters:
          bytes_value
          order : little or big (endian)
        """
    return hex(int.from_bytes(bytes_value, byteorder=order))

def bits_to_byte(bits_value):
    if bits_value % 8 != 0 :
        LOG.error("Bits size not available")
    return int(bits_value/8)

def modify_hex_value(hex_value, hex_to_replace, shift_value) :
    """ Modify the value of a hex number
        Parameters:
          hex_value : Initial hex value
          hex_to_replace : Value to replace
          shift_value : Shift of the hex to replace
        """
    hex_value = hex_value[2:] if "0x" in hex_value else hex_value
    hex_val = int(hex_value, 16)
    replace_val = int(hex_to_replace, 16) << shift_value
    replace_mask = "".join([ "0" if value == "0" else "F" for value in hex(replace_val)[2:]])
    full_inv_mask = "".join([ "F" for value in hex_value])
    full_inv_mask_val = int(full_inv_mask,16)
    replace_mask_val = int(replace_mask,16)
    final_mask = full_inv_mask_val ^ replace_mask_val
    new_hex_value = (final_mask & hex_val) | replace_val

    return hex(new_hex_value)
