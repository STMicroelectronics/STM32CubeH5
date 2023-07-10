#
# ****************************************************************************
# @file    main.py
# @author  MCD Application Team
# @brief   Main application file.
# ****************************************************************************
# @attention
#
# Copyright (c) 2023 STMicroelectronics.
# All rights reserved.
#
# This software is licensed under terms that can be found in the LICENSE file
# in the root directory of this software component.
# If no LICENSE file comes with this software, it is provided AS-IS.
#
# ****************************************************************************

import sys
import click
import os
import math
from struct import pack
from AppliCfg.utils import *
from AppliCfg import xml_parser

MIN_PYTHON_VERSION = (3, 10)
if sys.version_info < MIN_PYTHON_VERSION:
    LOG.error("Python %s.%s or newer is required by AppliCfg."
             % MIN_PYTHON_VERSION)

class BasedIntAllParamType(click.ParamType):
    name = 'integer'

    def convert(self, value, param, ctx):
        if isinstance(value, int):
            return value
        try:
            return convert_to_integer(value)
        except ValueError:
            self.fail('(%s) is not a valid integer. Retype the data and/or'
                      'prefixed with 0b/0B, 0o/0O, or 0x/0X as necessary.'
                      % value, param, ctx)


@click.argument('infile')
@click.option('-xml', '--xml', metavar='filename', default="",
              help='Location of the file that contains the RoT configuration')
@click.option('-nxml', '--xml_name', default=[], multiple=True,
              help='Tag <Name> content in the xml file. This will allow to '
              'filter the xml file and choose the value of the parameter to use'
              'It is possible to use more of one argument. Recommended to use with'
              'the --expression option')
@click.option('-l', '--layout', metavar='filename', default="",
              help='Location of the file that contains preprocessed macros')
@click.option('-m', '--macro', default="",
              help='macro symbol to search in the preprocessed file')
@click.option('-n', '--name', required=True,
              help='symbol to search in the linker file. If the symbol is found'
              'The symbol value will be modified')
@click.option('-e', '--expression', default="",
              help='Expression in order to modify the value found. The --constant'
              'argument is optional. The value found is declared as "value", the'
              'others values are declared as cons1, cons2....cons3'
              'Example 1: ((value + cons1)/cons2) - cons3'
              'If using multiple values they need to be defined as val1, val2 ...'
              'Example 2: ((val1 + cons1)/(cons2 + val2)) - cons3')
@click.option('-cons', '--constants', multiple=True,
              default=[], help='Constant values declared in the expression')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='Get the value from a C or xml variable in order to update a linker variable'
               'When using a c file the commands --layout and --macro are required'
               'When using a xml file the commands --xml and --xml_name are required'
               'More information in the README file')
def linker(infile, xml, xml_name, layout, macro, name, expression, constants, vb):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    LOG.info("############## Executing linker command ##############", vb)
    parse_command_input(sys.argv, vb)
    values=[]
    if os.path.isfile(xml): # Recover value(s) from xml file
        if not xml_name:
            LOG.error("--xml_name argument was not defined")
        # Initialize xml object
        xml_obj = xml_parser.XML_APPLI(xml, vb)
        xml_obj.get_items("Name")
        for xname in xml_name:
            value = xml_obj.get_value(xname, "Value")
            if not value:
                LOG.error("Not value found for '%s'. Output file can not be modified"
                    % xname)
            values.append(value)
    elif os.path.isfile(layout): # Recover value from macro_preprocessed file
        if not macro:
            LOG.error("--macro argument was not defined")
        # Compute and return value from macro file
        value = get_macro_value(macro, layout, vb)
        if value is None:
            LOG.error("Macro variable (%s) not found" % macro)
        value = hex(value)
        values.append(value)
    else:
        LOG.error("xml or layout file were not defined")
    # Different input values are only accepted when the expression argument is used
    # otherwise the last value of xml_values list will be used
    if len(values) > 1 and not expression:
        LOG.info("Using the value of the last content recovered %s" %value, vb)
    # If expression is declared, the expression will be computed
    if expression :
        if "cons" in expression and not constants :
            LOG.error("Expressions needs constants values")
        value = compute_expression(expression, values, constants, vb)

    # For linker project the final value is required en hex format. The value
    # will be formatted
    value = get_hex(value)
    LOG.info("Value to be used %s" %value, vb)
    # Verify linker file type
    if infile.endswith('.icf'):
        begin_line = "(.*)symbol " + name + "\s*=\s*(.*)"
        search_value = '\s*=\s*'
    elif infile.endswith('.sct'):
        LOG.error("sct file is not supported in this version")
    elif infile.endswith('.ld'):
        begin_line = "(.*)" + name + "\s*=\s*(.*)"
        search_value = '\s*=\s*'
    else:
        LOG.error("Linker value not recognized")
    infile_dbg = infile.split("\\")[-1] if "\\" in infile else infile
    LOG.info("File to update '%s'" % infile_dbg, vb)
    str_debug = "Linker variable \"%s\" " % name +\
                "will be updated with value \"%s\"" %value
    LOG.info(str_debug, vb)
    # Modify linker value
    if not modify_file_value(infile, begin_line, search_value, value, vb):
        LOG.error("Linker option (%s) not found in the file" % name)


@click.argument('infile')
@click.option('-l', '--layout', metavar='filename', default="",
              help='Location of the file that contains preprocessed macros')
@click.option('-m', '--macro', default="",
              help='macro symbol to search in the preprocessed file')
@click.option('-e', '--enable', default="",
              help='Define xml enable status 0 for disable or 1 for enable')
@click.option('-n', '--name', default="",
              help='Tag <Name> content in the xml file. This will allow to '
              'filter the xml file. The tag <Enable> will be modified')
@click.option('-c', '--command', default="",
              help='Tag <Command> content in the xml file. This will allow to '
              'filter the xml file. The tag <Enable> will be modified')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='Enable or disable xml options. The enable status could be'
                    'modified with the --layout and --macro arguments'
                    'Or by defining the --enable argument'
                    'More information in the README file')
def xmlen(infile, layout, macro, enable, name, command, vb):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    LOG.info("############## Executing xmlen command ##############", vb)
    parse_command_input(sys.argv, vb)
    if os.path.isfile(layout):
        if not macro:
            LOG.error("--macro argument was not defined")
        # if macro value layout is defined, the tag Enable will have the value
        # found in the file
        param_en_status = get_macro_value(macro, layout, vb)
        if param_en_status is None:
            LOG.error("Macro variable (%s) not found" % macro)
    else:
        if enable:
            param_en_status = convert_to_integer(enable)
        else:
            LOG.error("Enable option or layout were not defined")

    # If value is equals to zero the parameter will be disable otherwise it will
    # be enabled
    param_en_status = "0" if param_en_status == 0 else "1"
    # Initialize xml object
    if not name and not command:
        LOG.error("Parameter tag <Name> or <Command> was not defined")
    # Define element to filter the parameters
    is_command = True if command else False
    element_name = "Command" if is_command else "Name"
    element_content = command if is_command else name
    xml_obj = xml_parser.XML_APPLI(infile, vb)
    xml_obj.get_items("Enable")
    xml_obj.get_filter_list(element_name)

    # Enable or disable the options
    if not xml_obj.modify_value(element_content, element_name, "Enable", param_en_status):
        tag_structure = "<%s>" % element_name + \
            element_content + "</%s>" % element_name
        LOG.error("The xml value was not modified. Verify the parameter with the tag '%s'"
                 % tag_structure)
    # Modified and save the xml modified
    xml_obj.save_file()


@click.argument('infile')
@click.option('-l', '--layout', metavar='filename', default="",
              help='Location of the file that contains preprocessed macros')
@click.option('-m', '--macro', default="",
              help='macro symbol to search in the preprocessed file')
@click.option('-n', '--name', default="",
              help='Tag <Name> content in the xml file')
@click.option('-link', '--link', default=None,
              help='Tag <Link> content of the xml parameter')
@click.option('-v', '--value', default=None,
              help='Tag <Value> content of the xml parameter')
@click.option('-t', '--type', default=None,
              help='Tag <Type> content of the xml parameter')
@click.option('-c', '--command', default=None,
              help='Tag <Command> content in the xml file (with - or --)')
@click.option('-e', '--enable', default=None,
              help='Tag <Enable> content of the xml parameter  (0 disable or 1 enable)')
@click.option('-d', '--default', default=None,
              help='Tag <Default> content of the xml parameter')
@click.option('-h', '--hidden', default=None,
              help='Tag <Hidden> content of the xml parameter (0 no hidden, 1 hidden)')
@click.option('-ttip', '--tooltip', default=None,
              help='Tag <Tooltip> content of the xml parameter')
@click.option('-o', '--option', default="add",
              help='If not layout file is defined it is possible to add or remove'
                    'it is possible to add or remove a parameter manually : add or rm')
@click.option('-pp', '--parameter_position', type=BasedIntAllParamType(), default=2,
              help='Position of the new parameter. By default is 2')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='add or remove a xml parameter. The action (add/remove) can'
                    'be defined using  --layout and --macro arguments'
                    'or the --option argument'
                    'More information in the README file')
def xmlparam(infile, layout, macro, name, type, link, value, command, enable,
             default, hidden, tooltip, option, parameter_position, vb):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    LOG.info("############## Executing xmlparam command ##############", vb)
    parse_command_input(sys.argv, vb)
    if os.path.isfile(layout):
        if not macro:
            LOG.error("--macro argument was not defined")
        # if layout is defined, the tag to be add/rm will
        # be get it from this file
        macro_value = get_macro_value(macro, layout, vb)
        if macro_value is None:
            LOG.error("Macro variable (%s) not found" % macro)
        param_option = "add" if macro_value > 0 else "rm"
    else:
        if option:
            param_option = option
        else:
            LOG.error("Option or layout file were not defined")
    if param_option == "rm":
        xml_obj = xml_parser.XML_APPLI(infile, vb)
        if not name and not command:
            LOG.error("To remove a parameter it is necessary to define the name \
                    argument or the command argument")
        tag = "Command" if command else "Name"
        tag_content = command if command else name
        if not xml_obj.remove_param_item(tag, tag_content):
            tag_structure = "<%s>" % tag + tag_content + "</%s>" % tag
            LOG.error("The parameter '%s' was not removed or not found" %
                      tag_structure)
        # Modified and save the xml modified
        xml_obj.save_file()
    elif param_option == "add":
        parameter_content = {}
        if not name is None:
            parameter_content["Name"] = name
        if not value is None:
            parameter_content["Value"] = value
        if not link is None:
            parameter_content["Link"] = link
        if not type is None:
            parameter_content["Type"] = type
        if not command is None:
            parameter_content["Command"] = command
        if not enable is None:
            parameter_content["Enable"] = enable
        if not hidden is None:
            parameter_content["Hidden"] = hidden
        if tooltip:
            parameter_content["Tooltip"] = tooltip
        if not default is None:
            parameter_content["Default"] = default

        if "Name" in parameter_content or "Command" in parameter_content:
            # Item to verify if Parameter already exists
            tag = "Command" if "Command" in parameter_content else "Name"
            xml_obj = xml_parser.XML_APPLI(infile, vb)
            if not xml_obj.add_param_item(tag, parameter_content, parameter_position):
                LOG.error("Parameter was not added")
            # Modified and save the xml modified
            xml_obj.save_file()
    else:
        LOG.error("Wrong option (%s) selected. Only add or rm are available" % option)


@click.argument('infile')
@click.option('-l', '--layout', metavar='filename', default="",
              help='Location of the file that contains preprocessed macros')
@click.option('-m', '--macro',
              help='macro symbol to search in the preprocessed file')
@click.option('-b', '--binary_file', metavar='filename', default="",
              help='Binary input file (AreaAPayload)')
@click.option('-b_el_idx', '--binary_element_index',
              type=BasedIntAllParamType(), default=None,
              help='Index of layout element')
@click.option('-b_el_sz', '--binary_element_size',
              type=BasedIntAllParamType(), default=48,
              help='Binary element size of layout element')
@click.option('-b_d_sz', '--binary_data_size',
              type=BasedIntAllParamType(), default=8,
              help='Binary data size in bits')
@click.option('-b_d_e', '--binary_data_endianness', default="big",
              help='Data binary endianess format (little or big)')
@click.option('-nxml_el_idx', '--xml_name_layout_index', default="",
              help='Tag <Name> content in the xml file. This will allow to '
              'define the Flash Layout configuration index')
@click.option('-xml', '--xml', metavar='filename', default="",
              help='Location of the file that contains the RoT configuration')
@click.option('-nxml', '--xml_name', default=[], multiple=True,
              help='Tag <Name> content in the xml file. If the parameter is '
                   'found the tag value content will be used')
@click.option('-txml', '--xml_tag', default="",
              help='Tag content in the xml file to modify. By default the content'
              'of tag <Value></Value> will be modified'
              'if --name or --command are not defined, --txml will be used to search'
              'the tag to be modified. Attention : The tag must be the only one existing'
              'in the xml file otherwise the first tag found will be modified')
@click.option('-sm', '--second_macro', default="",
              help='Use the second_macro value if value found with macro variable'
                  'is different to equals_to argument')
@click.option('-v', '--value', default=[], multiple=True,
              help='New value content of the parameter selected'
                   'If second_macro is defined, this arguments will help to define'
                   'witch value will be used. If the value found for macro symbol'
                   'is equals to the value argument, this value will be used.'
                   'Otherwise the value found for second_macro will be used')
@click.option('-n', '--name', default="",
              help='Tag <Name> content in the xml file. If the tag is found, '
                   'the tag <Value> content will be modified')
@click.option('-c', '--command', default="",
              help='Tag <Command> content in the xml file. If the tag is found'
                   'The tag <Value> content will be modified')
@click.option('-cond', '--condition', default="",
              help='Expression in order to verify if expression condition must be'
              'executed.'
              'Example : (val2 > 0 and val2  % 2== 0 )')
@click.option('-e', '--expression', default="",
              help='Expression in order to modify the value found. The --constant'
              'argument is optional. The value found is declared as "value", the'
              'others values are declared as cons1, cons2....cons3'
              'Example 1: ((value + cons1)/cons2) - cons3'
              'If using multiple values they need to be defined as val1, val2 ...'
              'Example 2: ((val1 + cons1)/(cons2 + val2)) - cons3')
@click.option('-cons', '--constants', multiple=True,
              default=[], help='Constant values declared in the expression')
@click.option('--decimal', default=False, is_flag=True,
              help='Define it if the value to be replaced is in decimal format'
                   'by default the value to replace is in hex format')
@click.option('--string', default=False, is_flag=True,
              help='Define it if the value to be replaced is in string format'
                   'by default the value to replace is in hex format')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='Modify the Tag <Value> content of an xml parameter'
               'When using a c file the commands --layout and --macro are required'
               'When using a xml file the commands --xml and --xml_name are required'
               'Or the final value could be defined using the --value argument'
               'More information in the README file')
def xmlval(infile, xml, xml_name, layout, macro, second_macro, value, binary_file,
           binary_element_index, binary_element_size, binary_data_size,
           binary_data_endianness, xml_name_layout_index, xml_tag, name, command,
           condition, expression, constants, decimal, string, vb):
    if decimal and string:
        LOG.error("Choose only one format option")
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    LOG.info("############## Executing xmlval command ##############", vb)
    parse_command_input(sys.argv, vb)
    # Define value or values to use
    xml_values = []
    do_compute_expression = True
    if os.path.isfile(layout): # Recover value from macro_preprocessed file
        if not macro:
            LOG.error("--macro argument was not defined")
        # Compute and return value from macro file
        xml_value = get_macro_value(macro, layout, vb)
        if xml_value is None:
            LOG.error("Macro variable (%s) not found" % macro)
        # if macro arguments are defined, it is necessary to verify witch value use
        if second_macro and value:
            str_debug = "Choosing between the value of %s" % macro + \
                        " or %s" %second_macro
            LOG.info(str_debug, vb)
            if xml_value == convert_to_integer(value[-1]):
                str_debug = "The value of %s will be used" %second_macro
                xml_value = get_macro_value(second_macro, layout, vb)
                if xml_value is None:
                    LOG.error("Macro variable (%s) not found" % macro)
        xml_value = hex(xml_value)
        xml_values.append(xml_value)
    elif os.path.isfile(xml):  # Recover value from xml file
        if not xml_name:
            LOG.error("--xml_name argument was not defined")
        # Initialize xml object
        xml_obj = xml_parser.XML_APPLI(xml, vb)
        xml_obj.get_items("Name")
        for xname in xml_name:
            xml_value = xml_obj.get_value(xname, "Value")
            if not xml_value:
                LOG.error("Not value found for '%s'. Output file can not be modified"
                    % xname)
            xml_values.append(xml_value)
    elif os.path.isfile(binary_file):
        if binary_element_index is None:
            LOG.error("Element index to recover was not defined")
        if not xml_name_layout_index:
            LOG.error("Negative index are not available")
        if binary_element_index < 0 :
            LOG.error("Wrong index. Available 1 to %s" % str(binary_element_size-1))
        xml_obj = xml_parser.XML_APPLI(infile, True)
        xml_obj.get_items("Name")
        layout_config_index = xml_obj.get_value(xml_name_layout_index, "Value")
        if not layout_config_index:
            LOG.error("Not value found for '%s'" %xml_name_layout_index)

        # Transform found data to integer
        layout_config_index = convert_to_integer(layout_config_index)

        # Get the started index of the element
        binary_element_offset = int(binary_element_size * layout_config_index)

        # Read binary file and get element configuration
        with open(binary_file, 'rb') as f:
            data = f.read()

        if binary_element_offset + binary_element_size > len(data):
            LOG.error("Binary element configuration is bigger than binary file size")
        binary_conf_element = data[binary_element_offset : binary_element_offset + binary_element_size]

        if binary_element_index >= len(binary_conf_element):
            LOG.error("The index value is bigger than the element size")
        bytes_to_recover  = bits_to_byte(binary_data_size)
        start_idx = binary_element_index
        end_idx = binary_element_index + bytes_to_recover
        if end_idx > len(binary_conf_element) :
              LOG.error("Data overflow (data to recover outside of element)")
        binary_data = binary_conf_element[start_idx : end_idx]
        xml_value = data_format(binary_data, binary_data_endianness)

        # Get extra xml input values
        xml_values = [xml_value] # The binary value will be always the first xml value
        for xname in xml_name:
            extra_xml_value = xml_obj.get_value(xname, "Value")
            if not extra_xml_value:
                LOG.error("Not value found for '%s'. Output file can not be modified"
                    % xname)
            xml_values.append(extra_xml_value)
    else:
        if value:
            xml_value = value[-1]
            xml_values = value
        else:
            LOG.error("Value option or xml/layout file were not defined")
    # Different input values are only accepted when the expression argument is used
    # otherwise the last value of xml_values list will be used
    if len(xml_values) > 1 and not expression:
        LOG.info("Using the value of the last content recovered %s" %xml_value, vb)
    # Post value treatment
    if string : # Not transform output content
        xml_value =  xml_value if type(xml_value) is str else str(xml_value)
    else :
        if condition :
            do_compute_expression = compute_condition(condition, xml_values, vb)
        # If expression is declared, the expression will be computed
        if expression and do_compute_expression:
            if "cons" in expression and not constants :
                LOG.error("Expressions needs constants values")
            xml_value = compute_expression(expression, xml_values, constants, vb)
        # For xml file the final value is required en hex format. The value
        # will be formatted
        xml_value = str(convert_to_integer(xml_value)) if decimal else get_hex(xml_value)

    LOG.info("Value to be used %s" %xml_value, vb)
    # Define element to filter the parameters
    tag_to_modify = "Value" if not xml_tag else xml_tag
    is_unique_tag = False
    if not command and not name :
        if not xml_tag:
            LOG.error("Tag to modify its content was not defined")
        element_name = xml_tag
        element_content = ""
        is_unique_tag = True
    else :
        element_name = "Command" if command else "Name"
        element_content = command if command else name
    # Initialize xml object
    xml_obj = xml_parser.XML_APPLI(infile, vb)
    xml_obj.get_items(element_name)
    # Enable or disable the xml options
    if is_unique_tag :
        ret = xml_obj.modify_value(tag_content=element_name,
                                   new_sub_tag_content=xml_value)
    else :
        ret = xml_obj.modify_value(element_content, element_name, tag_to_modify, xml_value)
    if not ret:
        tag_structure = "<%s>" % element_name + \
            element_content + "</%s>" % element_name
        LOG.error("The xml value was not modified. Verify the parameter with the tag '%s'"
                % tag_structure)
    # Modified and save the xml modified
    xml_obj.save_file()


@click.argument('infile')
@click.option('-l', '--layout', metavar='filename', default="",
              help='Location of the file that contains preprocessed macros')
@click.option('-m', '--macro',
              help='macro symbol to search in the preprocessed file')
@click.option('-v', '--value', default=None, type=BasedIntAllParamType(),
              help='If --second_name is defined, this arguments will help to define'
                   'witch tag <Name> will be used. If the value found for macro symbol'
                   'is equals to the --value argument, the --second_name will be used.'
                   'Otherwise the --name will be used')
@click.option('-n', '--name', default="", required=True,
              help='New Tag <Name> content in the xml file')
@click.option('-sn', '--second_name', default="",
              help='Define a second option of tag <Name> content. If the value'
              'found in the macro preprocessed file is different to --value'
              'argument the second_name will be used')
@click.option('-c', '--command', default="", required=True,
              help='Tag <Command> content in the xml file. If the tag is found'
                   'The name content will be modified')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='Modify the Tag <Name> content of an xml parameter'
                    'The modification could be executed with the --layout and'
                    '--macro arguments. Or by defining the --name argument'
                    'More information in the README file')
def xmlname(infile, layout, macro, value, name, second_name, command, vb):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    LOG.info("############## Executing xmlname command ##############", vb)
    parse_command_input(sys.argv, vb)
    if os.path.isfile(layout): # Recover value from macro_preprocessed file
        if not macro:
            LOG.error("--macro argument was not defined")
        if value is None:
            LOG.error("--value argument was not defined")
        # Compute and return value from macro file
        xml_value = get_macro_value(macro, layout, vb)
        if xml_value is None:
            LOG.error("Macro variable (%s) not found" % macro)
        # if two macro arguments are defined, it is necessary to verify witch value use
        new_name = name
        if second_name :
            new_name = second_name if xml_value == value else new_name
        else :
            if xml_value != value :
                str_debug = "Macro value not equals to "+ \
                            "expected value . " + \
                            "Name modification will not be executed"
                LOG.info(str_debug, vb)
                sys.exit(0)
        if not new_name:
            LOG.error("Tag <Name>%s</Name> will not be modified" % name)
    else:
        if name:
            new_name = name
        else:
            LOG.error("name option or layout file were not defined")
    # Initialize xml object
    xml_obj = xml_parser.XML_APPLI(infile, vb)
    xml_obj.get_items("Command")
    # Enable or disable the xml options
    tag = "Command"
    ret = xml_obj.modify_value(command, tag, "Name", new_name)
    if not ret:
        tag_structure = "<%s>" % tag + command + "</%s>" % tag
        LOG.error("The xml value was not modified. Verify the parameter with the tag '%s'"
                 % tag_structure)
    # Modified and save the xml modified
    xml_obj.save_file()


@click.argument('infile')
@click.option('-xml', '--xml', metavar='filename', default="", required=True,
              help='Location of the file that contains the ST-iRoT configuration')
@click.option('-ob', '--option_byte', required=True,
              help='Define the Rot protection : SECWM, SRAM2_RST, SRAM2_ECC, WRPS')
@click.option('-sp', '--secure_pattern', required=True,
              help='Principal Tag <Name> content of the Rot protection')
@click.option('-co', '--code_offset', default="",
              help='Tag <Name> content for the primary code offset.information. '
              'To set SECWM it is necessary to get the value of the parameter '
              'with tag selected')
@click.option('-cs', '--code_size', default="",
              help='Tag <Name> content for the primary code size information. '
              'To set SECWM it is necessary to get the value of the parameter '
              'with tag selected')
@click.option('-fs', '--full_sec', default="",
              help='Tag <Name> content for the full secure information.'
              'To set SECWM it is necessary to get the value of the parameter '
              'with tag selected')
@click.option('-d', '--division', type=BasedIntAllParamType(), default=0x2000,
              help='Define the microcontroller sector size')
@click.option('-b', '--bank_size', type=BasedIntAllParamType(), default=0x100000,
              help='Define the microcontroller bank size')
@click.option('-b', '--begin', default="stm32programmercli",
              help='begin of line to replace ')
@click.option('-img', '--image_number', default="",
              help='number of managed images')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='Modify the protections of the RoT script'
                    'When the SECWM protection is selected the arguments'
                    '--code_size, --code_offset and --full_secure are required'
                    'More information in the README file')
def obscript(infile, xml, begin, option_byte, secure_pattern, code_size, code_offset,
             full_sec, division, bank_size, image_number, vb):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("The script file (%s) was not found" % infile)
    if not os.path.isfile(xml):
        LOG.error("The xml file (%s) was not found" % xml)
    LOG.info("############## Executing obscript command ##############", vb)
    parse_command_input(sys.argv, vb)
    # Options available for execution
    xml_in = ["SECWM", "SRAM2_RST", "SRAM2_ECC", "WRPS"]
    if not option_byte in xml_in:
        LOG.error("Invalid option byte (%s)" % option_byte)

    # Initialize xml object
    replace_val = {}
    xml_obj = xml_parser.XML_APPLI(xml, vb)
    xml_obj.get_items("Name")
    # Get the value of the Rot protection
    value = xml_obj.get_value(secure_pattern, "Value")
    if value is None:
        LOG.error("Not value found for '%s'" % secure_pattern)
    if "WRPS" in option_byte:
        if not code_offset:
            LOG.error("Tag <Name> content to get the code offset was not defined")
        if not code_size:
            LOG.error("Tag <Name> content to get the code size was not defined")

        sec_val_size = value
        code_offset = xml_obj.get_value(code_offset, "Value")

        ## If the firmware is full secure, it this necessary to get the size
        ## of "Firmware area size" instead the "Size of the secure area"
        if code_offset and sec_val_size :
            replace_val["WRPS"] = stirot_compute_wrps(code_offset, code_size,
                                            division, bankSize, vb)
        else :
            LOG.error("Code primary offset or secure value size are empty")
        
    elif "SECWM" in option_byte:
        if not code_offset:
            LOG.error("Tag <Name> content to get the primary code offset was not defined")
        if not full_sec:
            LOG.error("Tag <Name> content to get the full secure was not defined")
        if not code_size:
            LOG.error("Tag <Name> content to get the primary code size was not defined")

        sec_val_size = value
        is_full_sec = xml_obj.get_value(full_sec, "Value")
        code_primary_offset = xml_obj.get_value(code_offset, "Value")
        # If the firmware is full secure, it this necessary to get the size
        # of "Firmware area size" instead the "Size of the secure area"
        if is_full_sec :
            if not convert_to_integer(is_full_sec) == 0 :
                sec_val_size = xml_obj.get_value(code_size, "Value")
        if code_primary_offset and sec_val_size :
            replace_val = stirot_compute_wm(code_primary_offset, sec_val_size,
                                            division, bank_size, vb)
        else :
            LOG.error("Code primary offset or secure value size are empty")
    elif "SRAM2_RST" in option_byte:
        sram2_rst = "1" if "0x00000008" == value else "0"
        replace_val["SRAM2_RST"] = sram2_rst
    elif "SRAM2_ECC" in option_byte:
        sram2_ecc = "1" if "0x00000040" == value else "0"
        replace_val["SRAM2_ECC"] = sram2_ecc

    ret = True
    protections_error = []
    # Routine that allows to replace all the values contained in the dictionary
    for ob_pattern in replace_val:
        # There are several places where the variables to be replaced are defined,
        # however it was found that the place where they should be replaced
        # the string "stm32programmercli" is always there, if there is a change
        # in the script, this pattern must be modified.
        begin_pattern = begin
        # Sequence that allows us to find the value that will be replaced
        search_value = ob_pattern + "\s*=\s*"
        new_value = replace_val[ob_pattern]
        infile_dbg = infile.split("\\")[-1] if "\\" in infile else infile
        LOG.info("File to update '%s'" % infile_dbg, vb)
        str_debug = "Protection variable \"%s\" " % ob_pattern +\
                    "will be updated with value \"%s\"" %value
        LOG.info(str_debug, vb)
        status = modify_file_value(infile, begin_pattern, search_value, new_value, vb)
        if not status:
            protections_error.append(ob_pattern)
        ret &= status
    if not ret:
        LOG.error("Protections (%s) not found in the file" %
                 ",".join(protections_error))


@click.argument('infile')
@click.option('-xml', '--xml', metavar='filename', default="", required=True,
              help='Location of the file that contains the ST-iRoT configuration')
@click.option('-bxml', '--begin_xml', required=True,
              help='Macro symbol for the start protection value. The symbol'
              'will be searched in the preprocessed file')
@click.option('-m', '--memory', required=True,
              help='Macro symbol for the end protection value. The symbol'
              'will be searched in the preprocessed file')
@click.option('-sxml', '--size_xml', default="", required=True,
               help='Macro symbol for the sector number value. The symbol'
              'will be searched in the preprocessed file')
@click.option('-s', '--slot', default="", required=True,
              help='Line start pattern to replace ')
@click.option('-d', '--division', type=BasedIntAllParamType(),
              help='Sector group size')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='command to set specific Option Byte'
                    'More information in the README file')
def sectorerase(infile, xml, slot, begin_xml, size_xml, memory, division, vb):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    if not slot.startswith("primary") and not slot.startswith("secondary"):
        LOG.error("Slot '%s' not supported" % slot)
    if not memory.startswith("ext_nvm"):
        LOG.error("Memory type '%s' not supported" % memory)
    parse_command_input(sys.argv, vb)
    erase = {"start": begin_xml,
             "size": size_xml}
    LOG.info("Obtaining the input values to calculate the sectors to erase '%s' area" %slot, vb)
    LOG.info("Searching for '%s' area" %erase["start"], vb)

    if not os.path.isfile(xml): # Recover values from xml file
        LOG.error("xml or layout file were not defined")

    xml_obj = xml_parser.XML_APPLI(xml, vb)
    xml_obj.get_items("Name")
    start_value = xml_obj.get_value(begin_xml, "Value")
    start_value = xml_obj.get_value(erase["start"], "Value")
    size_value = xml_obj.get_value(erase["size"], "Value")

    if start_value is None:
        LOG.error("Variable (%s) not founded or not computed" % erase["start"])
    if size_value is None:
        LOG.error("Variable (%s) not founded or not computed" % erase["size"])

    # When using .bat files the variable needs to be set
    erase_line = "set "+ memory + "_" + slot if infile.endswith('.bat') else memory + "_" + slot
    erase_slots_area  = compute_sector_area( start_value, size_value, vb, division)
    erase_line_start = erase_line + "_start="
    erase_line_stop = erase_line + "_stop="

    str_debug = "erase slot \"%s\" " % erase_line_start +\
                    "will be updated with value \"%s\"" % erase_slots_area[0]
    if not modify_file_line(infile, erase_line_start, str(erase_slots_area[0]), vb):
                LOG.error("Initial pattern (%s) not founded" % begin_line)
    str_debug = "erase slot \"%s\" " % erase_line_stop +\
                    "will be updated with value \"%s\"" % erase_slots_area[1]
    if not modify_file_line(infile, erase_line_stop, str(erase_slots_area[1]), vb):
                LOG.error("Initial pattern (%s) not founded" % begin_line)

@click.argument('infile')
@click.option('-l', '--layout', metavar='filename', default="", required=True,
              help='Location of the file that contains preprocessed macros')
@click.option('-m', '--macro', required=True, default="",
              help='macro symbol to search in the preprocessed file')
@click.option('-xml', '--xml', metavar='filename', default="",
              help='Location of the file that contains the RoT configuration')
@click.option('-nxml', '--xml_name', default=[], multiple=True,
              help='Tag <Name> content in the xml file. If the parameter is'
                   'found the tag value content will be used')
@click.option('-n', '--name', required=True,
              help='#define variable to be modified')
@click.option('--parenthesis', default=False, is_flag=True,
              help='Define it if the value to be replaced is between parenthesis'
                   'by default the value to replace is between white spaces')
@click.option('--decimal', default=False, is_flag=True,
              help='Define it if the value to be replaced is in decimal format'
                   'by default the value to replace is in hex format')
@click.option('--string', default=False, is_flag=True,
              help='Define it if the value to be replaced is in string format'
                   'by default the value to replace is in hex format')
@click.option('-e', '--expression', default="",
              help='Expression in order to modify the value found. The --constant'
              'argument is optional. The value found is declared as "value", the'
              'others values are declared as cons1, cons2....cons3'
              'Example 1: ((value + cons1)/cons2) - cons3'
              'If using multiple values they need to be defined as val1, val2 ...'
              'Example 2: ((val1 + cons1)/(cons2 + val2)) - cons3')
@click.option('-cons', '--constants', multiple=True,
              default=[], help='Constant values declared in the expression')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')

@click.command(help='Modify the value of one #define variable in a ".h" file'
                    'More information in the README file')
def definevalue(infile, layout, macro, xml, xml_name, name, parenthesis,
                expression, constants, string, decimal, vb):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    if decimal and string:
        LOG.error("Choose only one format option")
    LOG.info("############## Executing definevalue command ##############", vb)
    parse_command_input(sys.argv, vb)
    values=[]
    if os.path.isfile(layout): #Recover values from macro_preprocessed file
        if not macro:
            LOG.error("--macro argument was not defined")
        # Compute and return value from macro file
        value = get_macro_value(macro, layout, vb)
        if value is None:
            LOG.error("Macro variable (%s) not found" % macro)
        value = hex(value)
        values.append(value)
    elif os.path.isfile(xml): # Recover values from xml file
        if xml_name == []:
            LOG.error("--xml_name argument was not defined")
        # Initialize xml object
        xml_obj = xml_parser.XML_APPLI(xml, vb)
        xml_obj.get_items("Name")
        for xname in xml_name:
            value = xml_obj.get_value(xname, "Value")
            if not value:
                LOG.error("Not value found for '%s'. Output file can not be modified"
                    % xml_name)
            values.append(value)
    else:
        LOG.error("xml or layout file were not defined")
    # Different input values are only accepted when the expression argument is used
    # otherwise the last value of xml_values list will be used
    if len(values) > 1 and not expression:
        LOG.info("Using the value of the last content recovered %s" %value, vb)

    # Transform the values to integer
    if string :
        value =  value if type(value) is str else str(value)
    else :
        # If expression is declared, the expression will be computed
        if expression :
            if "cons" in expression and not constants :
                LOG.error("Expressions needs constants values")
            value = compute_expression(expression, values, constants, vb)
        # For xml file the final value is required en hex format. The value
        # will be formatted
        value = str(convert_to_integer(value)) if decimal else get_hex(value)

    LOG.info("Value to be used %s" %value, vb)
    # Replace the variable where this is defined
    begin_pattern = "#define " + name
    # Select the way to obtain the value to be replaced
    # white spaces #define tests       5   /*test value*/
    # or
    # parenthesis  #define tests      (5)  /*test value*/
    delim = "\(" if parenthesis else ""
    search_value = name + "\s*" + delim
    infile_dbg = infile.split("\\")[-1] if "\\" in infile else infile
    LOG.info("File to update '%s'" % infile_dbg, vb)
    str_debug = "Define variable \"%s\" " % name +\
                "will be updated with value \"%s\"" %value
    LOG.info(str_debug, vb)
    ret = modify_file_value(infile, begin_pattern, search_value, value, vb)
    if not ret:
        LOG.error("Define variable (%s) not found in the file" % name)


@click.argument('infile')
@click.option('-xml', '--xml', metavar='filename', default="",
              help='Location of the file that contains the RoT configuration')
@click.option('-l', '--layout', metavar='filename', default="",
              help='Location of the file that contains preprocessed macros')
@click.option('-m', '--macro', default="",
              help='macro symbol to search in the preprocessed file')
@click.option('-v', '--value', type=BasedIntAllParamType(), default="",
              help='Expected value int the input file( c or xml)'
                   'If value is equals : Uncomment the define variable'
                   'otherwise : Comment the define variable')
@click.option('-a', '--action', default="",
              help='User can define if the variable will be comment or uncomment')
@click.option('-n', '--name', required=True,
              help='Define variable to be modified in the H file')
@click.option('-nxml', '--xml_name', default="",
              help='Tag <Name> content in the xml file. If the parameter is'
                   'found the tag value content will be used')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='Comment/Uncomment a #define variable in a project file'
               'When using a c file the commands --layout and --macro are required'
               'When using a xml file the commands --xml and --xml_name are required'
               'More information in the README file')
def setdefine(infile, xml, xml_name, layout, macro, action, value, name, vb):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    LOG.info("############## Executing setdefine command ##############", vb)
    parse_command_input(sys.argv, vb)
    if os.path.isfile(xml): # Recover value from xml file
        if not xml_name:
            LOG.error("--xml_name argument was not defined")
        # Initialize xml object
        xml_obj = xml_parser.XML_APPLI(xml, vb)
        xml_obj.get_items("Name")
        xml_value = xml_obj.get_value(xml_name, "Value")
        if not xml_value:
            LOG.error("Not value found for '%s'. Include file can not be modified"
                     % xml_name)
        try:
            file_value = convert_to_integer(xml_value)
        except ValueError:
            LOG.error("Wrong data notation '%s'" % xml_value)
        action = "uncomment" if file_value == value else "comment"
    elif os.path.isfile(layout): # Recover value from macro_preprocessed file
        if not macro:
            LOG.error("--macro argument was not defined")
        # Compute and return value from macro file
        macro_value = get_macro_value(macro, layout, vb)
        if macro_value is None:
            LOG.error("Macro variable (%s) not found" % macro)
        file_value = macro_value
        action = "uncomment" if file_value == value else "comment"
    else:
        if not action :
            LOG.error("xml or layout file were not defined")
        action = action.lower()
        if not action in ["uncomment", "comment"] :
            LOG.error("Unknown action %s" %action)

    infile_dbg = infile.split("\\")[-1] if "\\" in infile else infile
    LOG.info("File to update '%s'" % infile_dbg, vb)
    str_debug = "The define '%s' variable " %name + "will be %sed" %action
    LOG.info(str_debug, vb)
    # Modify define status
    if not toggle_comment_line(infile, name, vb, action):
        LOG.error("Define '%s' action was not performed" % action)


@click.argument('infile')
@click.option('-l', '--layout', metavar='filename', default="", required=True,
              help='Location of the file that contains preprocessed macros')
@click.option('-m', '--macro', required=True, default="",
              help='macro symbol to search in the preprocessed file')
@click.option('-n', '--name', required=True,
              help='Tag <Name> content in the xml file.  If the tag is found'
                   'The tag <KeyType> content will be modified')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='Modify key type and key file value'
                    'More information in the README file')
def keyconfig(infile, layout, macro, name, vb):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    if not os.path.isfile(layout): # Recover value from macro_preprocessed file
        LOG.error("File (%s) not found" % layout)
    LOG.info("############## Executing keyconfig command ##############", vb)
    parse_command_input(sys.argv, vb)
    # Compute and return value from macro file
    value = get_macro_value(macro, layout, vb)
    if value is None:
        LOG.error("Macro variable (%s) not found" % macro)
    cryptoscheme = str(value)

    cs = {"0": "rsa_2048", "1": "rsa_3072", "2": "ecdsa-p256"}

    if not cryptoscheme in cs:
        LOG.error("Wrong crypto scheme value (%s)" % cryptoscheme)

    # Initialize xml object
    xml_obj = xml_parser.XML_APPLI(infile, vb)
    xml_obj.get_items("Name")
    ret = xml_obj.modify_value( name, "Name", "KeyType", cs[cryptoscheme])
    if not ret:
        LOG.error("The key modification was not successfully performed")

    # Modified and save the xml modified
    xml_obj.save_file()


@click.argument('infile')
@click.option('-l', '--layout', metavar='filename',
              help='Location of the file that contains preprocessed macros')
@click.option('-m', '--macro',
              help='macro symbol to search in the preprocessed file')
@click.option('-xml', '--xml', metavar='filename', default="",
              help='Location of the file that contains the RoT configuration')
@click.option('-n', '-nxml','--name', "--xml_name", default=[], multiple=True,
              help='Tag <Name> content in the xml file. If the parameter is'
                  'found the tag value content will be used'
                  'It is possible to use more of one argument. Recommended to '
                  'use with the --expression option')
@click.option('-b', '--begin', required=True,
              help='Line start pattern to replace ')
@click.option('-sr', '--save_result',  metavar='filename', default="",
              help='Save result in an external file')
@click.option('-d', '--division', type=BasedIntAllParamType(), default=None,
              help='divide the value')
@click.option('--decimal', default=False, is_flag=True,
              help='Define it if the value to be replaced es in decimal format'
                   'by default the value to replace is in hex format')
@click.option('-e', '--expression', default="",
              help='Expression in order to modify the value found. The --constant'
              'argument is optional. The value found is declared as "value", the'
              'others values are declared as cons1, cons2....cons3'
              'Example 1: ((value + cons1)/cons2) - cons3'
              'If using multiple values they need to be defined as val1, val2 ...'
              'Example 2: ((val1 + cons1)/(cons2 + val2)) - cons3')
@click.option('-cons', '--constants', multiple=True,
              default=[], help='Constant values declared in the expression')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='Modify bash /batch variable value with layout file content'
                    'More information in the README file')
def flash(infile, xml, layout, macro, name, begin, save_result, division, decimal,
          expression, constants, vb):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    LOG.info("############## Executing flash command ##############", vb)
    parse_command_input(sys.argv, vb)
    values=[]
    if os.path.isfile(xml): # Recover value from xml file
        # Initialize xml object
        xml_obj = xml_parser.XML_APPLI(xml, vb)
        xml_obj.get_items("Name")
        for xml_name in name:
            value = xml_obj.get_value(xml_name, "Value")
            if not value:
                LOG.error("Not value found for '%s'. Output file can not be modified"
                    % xml_name)
            values.append(value)
    elif os.path.isfile(layout): # Recover value from macro_preprocessed file
        # Compute and return value from macro file
        value = get_macro_value(macro, layout, vb)
        if value is None:
            LOG.error("Macro variable (%s) not found" % macro)
        value = hex(value)
        values.append(value)
    else:
        LOG.error("xml or layout file were not defined")

    if expression and division:
        '''
            It is recommended to use the --expression option. The division option
            allow to keep compatibility with old scripts
        '''
        str_debug = "Only one operation at the time is valid." + \
                    "Please choose between --expression or --division options"
        LOG.error(str_debug)
    # Different input values are only accepted when the expression argument is used
    # otherwise the last value of xml_values list will be used
    if len(values) > 1 and not expression:
        LOG.info("Using the value of the last content recovered %s" %value, vb)
    # If expression is declared, the expression will be computed
    if expression :
        if "cons" in expression and not constants :
            LOG.error("Expressions needs constants values")
        value = compute_expression(expression, values, constants, vb)
    # Compute the value to use
    elif division is not None : # Division by 0 is not taken in consideration
        if int(division) == 0:
              LOG.error("Division by 0 is not possible")
        # Division operation
        value = int(convert_to_integer(value)/ int(division))
        str_debug = "The value computed will by divided by '%s'." % division
        str_debug += "The new value is '%s'" % get_hex(value)
        LOG.info(str_debug, vb)
    # Choose the value format
    value = str(convert_to_integer(value)) if decimal else get_hex(value)
    LOG.info("Value to be used %s" %value, vb)
    # Define pattern to search
    # When using .bat files the variable needs to be set
    begin_line = "set " + begin if infile.endswith('.bat') else begin
    new_value = "=" + value
    infile_dbg = infile.split("\\")[-1] if "\\" in infile else infile
    LOG.info("File to update '%s'" % infile_dbg, vb)
    str_debug = "Script variable \"%s\" " % begin +\
                "will be updated with value \"%s\"" %value
    LOG.info(str_debug, vb)
    if not modify_file_line(infile, begin_line, new_value, vb):
        LOG.error("Initial pattern (%s) not found" % begin_line)
    if save_result :
        LOG.info("The value %s will be saved in the %s file" % (value, save_result), vb)
        f = open(save_result, "w")
        f.write(value)
        f.close()


@click.argument('infile')
@click.option('-l', '--layout', metavar='filename', required=True,
              help='Location of the file that contains preprocessed macros')
@click.option('-ms', '--macro_start', required=True,
              help='Macro symbol for the start protection value. The symbol'
              'will be searched in the preprocessed file')
@click.option('-me', '--macro_end', required=True,
              help='Macro symbol for the end protection value. The symbol'
              'will be searched in the preprocessed file')
@click.option('-msec', '--macro_sectors', required=True,
               help='Macro symbol for the sector number value. The symbol'
              'will be searched in the preprocessed file')
@click.option('-b', '--begin', required=True,
              help='Line start pattern to replace ')
@click.option('-d', '--division', type=BasedIntAllParamType(), default=0x2000,
              help='Sector group size')
@click.option('-pa', '--protected_area', type=BasedIntAllParamType(), default=0x10000,
              help='Define the protected area size')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='command to set specific Option Byte'
                    'More information in the README file')
def setob(infile, layout, begin, macro_start, macro_end, macro_sectors,
          division, vb, protected_area):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    if not os.path.isfile(layout): # Recover value from macro_preprocessed file
        LOG.error("File (%s) not found" % layout)
    if not begin.startswith("wrp") and not begin.startswith("hdp") and not begin.startswith("OEMiLoader_address"):
        LOG.error("Option Byte '%s' not supported" % begin)
    LOG.info("############## Executing setob command ##############", vb)
    parse_command_input(sys.argv, vb)
    ob = {"start": macro_start,
          "end": macro_end,
          "secnbr": macro_sectors}

    # Compute and return values from macro file
    LOG.info("Obtaining the input values to calculate the protection of '%s'" %begin, vb)
    start_value = get_macro_value(ob["start"], layout, vb)
    end_value = get_macro_value(ob["end"], layout, vb)
    secnbr_value = get_macro_value(ob["secnbr"], layout, vb)

    if start_value is None:
        LOG.error("Variable (%s) not found or not computed" % ob["start"])
    if end_value is None:
        LOG.error("Variable (%s) not found or not computed" % ob["end"])
    if secnbr_value is None:
        LOG.error("Variable (%s) not found or not computed" % ob["secnbr"])

    # When using .bat files the variable needs to be set
    begin_line = "set " + begin if infile.endswith('.bat') else begin

    # Calculation of Value for `wrpgrp`
    if begin == "wrpgrp1" or begin == "wrpgrp2":
        wrpgrp1, wrpgrp2 = compute_wrp_protections(start_value, end_value, division,
                                                   secnbr_value, vb)
        wrp_value = wrpgrp1 if begin == "wrpgrp2" else wrpgrp2
        str_debug = "Protection variable \"%s\" " % begin +\
                    "will be updated with value \"%s\"" % get_hex(wrp_value)
        infile_dbg = infile.split("\\")[-1] if "\\" in infile else infile
        LOG.info("File to update '%s'" % infile_dbg, vb)
        LOG.info(str_debug, vb)
        # Output the stdout modified hardening
        value = "=" + get_hex(wrp_value)
        if not modify_file_line(infile, begin_line, value, vb):
            LOG.error("Initial pattern (%s) not founded" % begin_line)
    elif begin == "wrps":
        wrps_group_nb = int(protected_area / division)
        protected_sector_size = int((secnbr_value - division) / division)
        wrps_str = int(start_value / division)
        wrps_end = int(wrps_str + protected_sector_size)
        LOG.info("Flash sectors to set under WRP from %d to %d" % (wrps_str, wrps_end), vb)
        wrps_enable = 0
        wrps_disable = 1
        for i in range(wrps_group_nb):
            # When using .bat files the variable needs to be set
            begin_line = "set " + begin + str(i) + "=" if infile.endswith('.bat') else begin + str(i)
            if (i >= wrps_str) and (i <= wrps_end):
                wrps_state = wrps_enable
            else:
                wrps_state = wrps_disable
            if not modify_file_line(infile, begin_line, str(wrps_state), vb):
                LOG.error("Initial pattern (%s) not founded" % begin_line)

    # Calculation of value for hdp start/end
    elif (begin == "hdp1_start" or begin == "hdp1_end" or begin == "hdp2_start" or begin == "hdp2_end") or (begin == "hdp_start" or begin == "hdp_end"):
        hdp1, hdp2 = compute_hdp_protection( start_value, end_value, secnbr_value, vb, division)
        if begin in ["hdp1_start", "hdp_start"]:
            hdp_value = hdp1[0]
        elif begin in ["hdp1_end", "hdp_end"]:
            hdp_value = hdp1[1]
        elif begin == "hdp2_start":
            hdp_value = hdp2[0]
        elif begin == "hdp2_end":
            hdp_value = hdp2[1]
        else:
            hdp_value = 0
        str_debug = "Protection variable \"%s\" " % begin +\
                    "will be updated with value \"%s\"" % get_hex(hdp_value)
        infile_dbg = infile.split("\\")[-1] if "\\" in infile else infile
        LOG.info("File to update '%s'" % infile_dbg, vb)
        LOG.info(str_debug, vb)
        value = "=" + get_hex(hdp_value)
        if not modify_file_line(infile, begin_line, value, vb):
            LOG.error("Initial pattern (%s) not found" % begin_line)
    elif begin == "OEMiLoader_address":
        iLoader_add = convert_to_integer(0x08000000) + convert_to_integer(start_value)
        str_debug = "Protection variable \"%s\" " % begin +\
                    "will be updated with value \"%s\"" % get_hex(iLoader_add)
        infile_dbg = infile.split("\\")[-1] if "\\" in infile else infile
        LOG.info("File to update '%s'" % infile_dbg, vb)
        LOG.info(str_debug, vb)
        value = "=" + get_hex(iLoader_add)
        if not modify_file_line(infile, begin_line, value, vb):
            LOG.error("Initial pattern (%s) not founded" % begin_line)
    else:
        LOG.error("Error: OB (%s) not supported" % begin)



@click.option('-fb', '--first_bin', metavar='filename', required=False,
              default="", help='First binary path (Normally secure image)')
@click.option('-o', '--optional_size', '--expected_f_binary_size',
              type=BasedIntAllParamType(), required=False,
              default=0, help='Expected first binary size '
                              '(padding will be added if necessary)')
@click.option('-sb', '--second_bin', metavar='filename', required=False,
              default="", help='Second binary path (Normally nonsecure image)')
@click.option('-i', '--input_size', '--expected_s_binary_size',
              type=BasedIntAllParamType(), required=False,
              default=0, help='Expected second binary size '
                              '(padding will be added if necessary)')
@click.option('-ob', '--output_bin', required=False,
              default="", help='Define the name of the assembled binary')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='Create one image'
                    'More information in the README file')
def oneimage(first_bin, second_bin, input_size, optional_size, output_bin, vb):
    # Get the input paths
    LOG.info("############## Executing oneimage command ##############", vb)
    parse_command_input(sys.argv, vb)
    LOG.info("First binary  : '%s'" % first_bin, vb)
    LOG.info("Second binary : '%s'" % second_bin, vb)
    if first_bin is not None:
        with open(first_bin, 'rb') as f:
            optional_binary = f.read()
    else:
        LOG.info("First binary not defined", vb)
        optional_binary = b''

    if optional_size != 0:
        LOG.info("Extra padding will be added at the end of the first binary", vb)
        optional_binary += (optional_size-len(optional_binary))*pack("B", 0xff)

    with open(second_bin, 'rb') as f:
        binary = f.read()
        if input_size != 0:
            LOG.info("Extra padding will be added at the end of the second binary", vb)
            binary += (input_size-len(binary))*pack("B", 0xff)

    with open(output_bin, 'wb') as f:
        f.write(optional_binary)
        f.write(binary)
    LOG.info("Image assembly success", True)
    LOG.info("Final image size '%s'" % int(os.path.getsize(output_bin)), True)


@click.argument('infile')
@click.option('-l', '--layout', metavar='filename', default="", required=True,
              help='Location of the file that contains preprocessed macros')
@click.option('-me', '--macro_encrypted', default="", required=True,
               help='Macro symbol for the encryption mode. The symbol'
              'will be searched in the preprocessed file')
@click.option('-mi', '--macro_image', default="", required=True,
               help='Macro symbol for the image number value. The symbol'
              'will be searched in the preprocessed file')
@click.option('-xml', '--xml', metavar='filename', default="", required=True,
              help='Location of the XML file of the Rot Application')
@click.option('-i', '--input_bin', default="", required=True,
              help='Project binary to use as input for the TPC tool')
@click.option('-in', '--input_name',required=True,
              help='Tag <Name> content to select the input binary of the xml file')
@click.option('-on', '--output_name', required=True,
              help='Tag <Name> content to select the output binary of the xml file')
@click.option('-en', '--enc_name', required=True,
              help='Tag <Name> content to select the authentication mode of the xml file')
@click.option('-b', '--begin', required=True,
              help='begin of line to replace in the script file')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='Modify the project input/output files in order to use the correct images'
                    'More information in the README file')
def iofile(infile, layout, macro_encrypted, macro_image, xml, input_bin,
           input_name, output_name, begin, enc_name, vb):
    # Compute and return value from macro file
    # 0 non image, 1 (for data that means enable, for app that mean one image cfg)
    # 2 impossible for data images, only available for app
    LOG.info("############## Executing iofile command ##############", vb)
    parse_command_input(sys.argv, vb)
    LOG.info("Obtaining the input values to get the input/output binary name", vb)
    image_cfg = get_macro_value(macro_image, layout, vb)
    enc_mode = get_macro_value(macro_encrypted, layout, vb)
    is_oneimage = False
    is_nsimage = False

    if image_cfg is None:
        LOG.error("Macro variable (%s) not found" % macro_image)
    if enc_mode is None:
        LOG.error("Macro variable (%s) not found" %macro_encrypted)

    img_enc_mode = "_enc_sign.hex" if enc_mode else "_sign.hex"
    enc_status = "1" if enc_mode else "0"

    input_bin_path=input_bin.replace("\\","/")
    if "Code" in xml:
        img_type = "app"
        if "_S_" in xml:
            if image_cfg != 2:
                #Configuration with one code image
                LOG.info("Nothing to modify. Configuration in one image", vb)
                sys.exit(0)
        elif "_NS_" in xml:
            if image_cfg == 1 and "Binary" in input_bin_path:
                is_oneimage = True
            elif image_cfg == 2 and "NonSecure" in input_bin_path:
                is_nsimage = True
            else:
                if image_cfg == 1:
                    LOG.info("Nothing to modify. Configuration in one image", vb)
                    sys.exit(0)
                elif image_cfg == 2:
                    LOG.info("Nothing to modify. Configuration in two image", vb)
                    sys.exit(0)
                else :
                    str_debug = "Wrong image configuration '%s'" %image_cfg + \
                                "Wrong format for the input path '%s'" %input_bin_path
                    LOG.error(str_debug)
        else:
            LOG.error("Wrong xml image")
    else:
        img_type = "data"
        if "_NS_" in xml:
            if image_cfg == 0:
                # Configuration with non secure data image disable
                LOG.info("Nothing to modify. Secure data image is disable",vb)
                sys.exit(0)
        elif "_S_" in xml:
            if image_cfg == 0:
                # Configuration with secure data image disable
                LOG.info("Nothing to modify. Non secure data image is disable",vb)
                sys.exit(0)
        else:
            LOG.error("Wrong xml image")

   # Initialize xml object
    LOG.info("Modifying input/output binaries from the xml file", vb)
    xml_obj = xml_parser.XML_APPLI(xml, vb)
    xml_obj.get_items("Name")
    xml_name = xml_obj.get_value(output_name, "Value")
    file_name = xml_name.split("/")[-1] if "/" in xml_name else xml_name
    file_prefix = file_name.split(img_type)[0]
    if is_oneimage :
        replace_ns_pattern = "ns" if file_prefix.startswith("ns") else "_ns"
        if "ns" in file_prefix :
            LOG.info("One image configuration replacing the prefix '_ns' ", vb)
            file_prefix = file_prefix.replace(replace_ns_pattern,"")
    if is_nsimage:
        if not "ns" in file_prefix:
            LOG.info("Two image configuration adding the prefix 'ns' ", vb)
            file_prefix  += "ns_"

    new_file_name =  file_prefix + img_type + img_enc_mode

    # Save xml modification
    save_modifications = False # If one value is True then the file must be saved
    if file_name != new_file_name:
        LOG.info("Replacing '%s' " % file_name + "by '%s'" % new_file_name, vb)
        new_xml_name = xml_name.replace(file_name, new_file_name)
        # Modify output file
        LOG.info("Modify Binary output name by '%s'" % new_xml_name, vb)
        xml_obj.modify_value(output_name, "Name", "Value", new_xml_name)
        save_modifications |= xml_obj.do_save_file
    else :
        LOG.info("The binary output content has the correct file", vb)

    LOG.info("Modify Binary input path by '%s'" % input_bin_path, vb)
    xml_obj.modify_value(input_name, "Name", "Value", input_bin_path)
    save_modifications |= xml_obj.do_save_file
    LOG.info("%s encryption mode " % ("Enable" if enc_mode else "Disable"), vb)
    xml_obj.modify_value(enc_name, "Name", "Enable", enc_status)
    save_modifications |= xml_obj.do_save_file
    if save_modifications:
        xml_obj.do_save_file = True
    xml_obj.save_file()

    LOG.info("Modifying input binaries from the script file", vb)
    # When using .bat files the variable needs to be set
    begin_line = ("set " + begin if infile.endswith('.bat') else begin) + "="

    new_value = new_file_name
    if not modify_file_line(infile, begin_line, new_value, vb):
        LOG.error("Initial pattern (%s) not found" % begin_line)

@click.argument('infile')
@click.option('-xml', '--xml', metavar='filename', default="",
              help='Location of the file that contains the RoT configuration')
@click.option('-nxml', '--xml_name', default="",
              help='Tag <Name> content in the xml file. If the parameter is'
                   'found the tag value content will be used')
@click.option('-v', '--value', default="",
              help='New value to replace in file')
@click.option('-var', '--variable', required=True,
              help='Name of the variable whose value will be modified')
@click.option('-sft', '--shift', type=BasedIntAllParamType(), default=None,
              help='Shift to define value position')
@click.option('-del', '--delimiter', default=',',
              help='Name of the variable whose value will be modified')
@click.option('--upper', default=False, is_flag=True,
              help='Define value output in capital letters')
@click.option('--str', default=False, is_flag=True,
              help='Define if value to modify is a string value')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.command(help='Modify the variable value of an external file')
def modifyfilevalue(infile, xml, xml_name, value, variable, shift, delimiter,
                    upper, str, vb):
    # Verify if the input files exist
    if not os.path.isfile(infile):
        LOG.error("File (%s) not found" % infile)
    LOG.info("########## Executing modifyfilevalue command #########", vb)
    parse_command_input(sys.argv, vb)
    # Define value or values to use
    if os.path.isfile(xml):  # Recover value from xml file
        if not xml_name:
            LOG.error("--xml_name argument was not defined")
        # Initialize xml object
        xml_obj = xml_parser.XML_APPLI(xml, vb)
        xml_obj.get_items("Name")
        xml_value = xml_obj.get_value(xml_name, "Value")
        if not xml_value:
            LOG.error("Not value found for '%s'. Output file can not be modified"
                % xml_name)
    else:
        if value is None:
            LOG.error("Value option or xml file were not defined")
        else:
            xml_value = value

    if str :
        # Define pattern to search
        # When using .bat files the variable needs to be set
        begin_line = variable + delimiter
        new_value = value
        infile_dbg = infile.split("\\")[-1] if "\\" in infile else infile
        LOG.info("File to update '%s'" % infile_dbg, vb)
        str_debug = "Script variable \"%s\" " % variable +\
                    "will be updated with value \"%s\"" %value
        LOG.info(str_debug, vb)
        if not modify_file_line(infile, begin_line, new_value, vb, delimiter=delimiter):
           LOG.error("File variable (%s) not modified in the file" % variable)
        sys.exit(0)

    # Define pattern to search
    infile_dbg = infile.split("\\")[-1] if "\\" in infile else infile
    begin_line = variable + delimiter
    search_value = variable + '\s*' + delimiter + '\s*'
    if shift is not None:
        LOG.info("Searching old variable %s value in file '%s'" % (variable, infile_dbg), vb)
        search_value = variable + "\s*" + delimiter
        old_value = get_file_value(infile, begin_line, search_value)
        new_hex_value = modify_hex_value(old_value, xml_value, shift)
        old_value_sz = len(old_value) - 2 if "0x" in old_value else len(old_value)
        new_hex_value = get_hex(new_hex_value, size=old_value_sz, upper_format=upper)
    else :
        new_hex_value = xml_value

    LOG.info("File to update '%s'" % infile_dbg, vb)
    str_debug = "File variable \"%s\" " % variable +\
                "will be updated with value \"%s\"" %new_hex_value
    LOG.info(str_debug, vb)
    if not modify_file_value(infile, begin_line, search_value, new_hex_value, vb):
        LOG.error("File variable (%s) not found in the file" % variable)

@click.argument('infile')
@click.option('-o', '--offset', type=BasedIntAllParamType(), default=0,
              help='Index start where the HASH will be added')
@click.option('-h', '--header', default="",
              help='Add Header before Hash (in hex format and without 0x)'
                    'Exemple : AABBCCDDEEFF')
@click.option('-i', '--input_data', metavar='filename', default="", required=True,
              help='Data which a Hash 256 will be calculate')
@click.option('-d', '--da_bin_file', metavar='filename', default="", required=True,
              help='Binary with da address + password payload size + password')
@click.option('--vb', default=False, is_flag=True,
              help='Define if debug traces will be displayed')
@click.option('--create', default=False, is_flag=True,
              help='Create binary file with hash value')
@click.command(help='Calculate the HASH256 of the content of a file. '
                    'The calculated hash will be stored in another binary file at '
                    'the location defined by the user.'
                    'More information in the README file')
def hashcontent(infile, input_data, da_bin_file, header, offset, create, vb):
    LOG.info("############## Executing hashcontent command ##############", vb)
    parse_command_input(sys.argv, vb)
    # Function input verification
    if offset and create :
        LOG.error("Offset and Create option can not be used at the same time")
    if not offset and not create :
        LOG.error("At least Offset and Create option must be defined")
    if not os.path.isfile(input_data):
        LOG.error("File dont exits " + input_data)

    # Opening data input
    input_data_dbg = input_data.split("\\")[-1] if "\\" in input_data else input_data
    LOG.info("Getting data from %s" % input_data_dbg, vb)
    with open(input_data, 'rb') as f:
        data=f.read()
        f.close()

    #Format header information
    if header:
      header_in_bytes = b""
      # Format DA address in little endian
      da_addr_size = math.ceil(len(header)/2)
      da_addr_size = da_addr_size - 1 if "0x" in header.lower() else  da_addr_size
      header_da_bytes = data_to_bytes(header, "little", da_addr_size)
      # Format user password size in little endian
      payload_size = hex(len(data))
      payload_bytes = data_to_bytes(payload_size, "little", 4)
      # Create DA file
      header_in_bytes = header_da_bytes + payload_bytes + data
      da_bin_file_dbg = da_bin_file.split("\\")[-1] if "\\" in da_bin_file else da_bin_file
      LOG.info("Creating the binary '%s'" %da_bin_file_dbg, vb)
      with open(da_bin_file, 'wb') as f:
          f.write(header_in_bytes)


    #Compute HASH256
    hash256_computed = compute_sha256(data, vb)
    infile_dbg = infile.split("\\")[-1] if "\\" in infile else infile
    # Create binary
    LOG.info("Creating the binary '%s'" %infile_dbg, vb)
    with open(infile, 'wb') as f:
        f.write(hash256_computed)

@click.group()
def AppliCfg():
    pass

AppliCfg.add_command(xmlen)
AppliCfg.add_command(xmlval)
AppliCfg.add_command(xmlparam)
AppliCfg.add_command(xmlname)
AppliCfg.add_command(linker)
AppliCfg.add_command(obscript)
AppliCfg.add_command(definevalue)
AppliCfg.add_command(setdefine)
AppliCfg.add_command(keyconfig)
AppliCfg.add_command(flash)
AppliCfg.add_command(setob)
AppliCfg.add_command(oneimage)
AppliCfg.add_command(iofile)
AppliCfg.add_command(hashcontent)
AppliCfg.add_command(modifyfilevalue)
AppliCfg.add_command(sectorerase)
