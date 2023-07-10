import xml.etree.ElementTree as ET
from AppliCfg.utils import *

class XML_APPLI():
    def __init__(self, xml_file, vb):
        # Import xml data
        self.xml_file = xml_file
        self.is_debug = vb
        xml_file_dbg = xml_file.split("\\")[-1] if "\\" in xml_file else xml_file
        LOG.info("Opening the '%s' file" %xml_file_dbg, vb)
        self.tree = ET.parse(xml_file)
        self.xmlContent = self.tree.getroot()
        self.xml_filtered_items = []
        self.xml_options = self.tree.getroot()
        self.do_save_file=True

    def get_items(self, tag):
        """ 
        Get all parameters containing the item name
        Parameters:
            tag  - Tag name (filter the xml file)
        Returns :
            xml_options - xml filtered
        """
        # Get options name
        self.xml_options = self.xmlContent.findall(
            './/' + tag + '/..')
        return self.xml_options

    def get_filter_list(self, tag):
        """ 
        Get the list of parameters containing the item name.
        The input to use is the pre-filter obtained with the get_items function.
        Parameters:
            tag  - Tag name (filter the xml file)
        Returns :
            xml_filtered_items - xml filtered
        """
        self.xml_filtered_items = []
        LOG.info("Filtering the file with the tag <%s>" % tag, self.is_debug)
        for option in self.xml_options:
            current_tag_content = option.find(tag).text
            if current_tag_content is not None:
                self.xml_filtered_items.append(current_tag_content)
        return self.xml_filtered_items

    def get_value(self, tag_content, sub_tag, tag_filter="Name"):
        """ 
        Filter the list by the name of the item, and obtain the value 
        of the desired subitem.
        The input to use is the pre-filter obtained with the get_items function.
        Parameters:
            tag_content  - Content of the item 'Name'.
            subsub_tag - Subitem from which content will be extracted
            tag_filter - Filter method of xml parameters
        Returns :
            xml_value - Value of the subitem found
        """
        # String structures (only for debug)
        tag_str = "<%s>" % tag_filter + tag_content + "</%s>" % tag_filter
        LOG.info("Searching the parameter with %s" % tag_str, self.is_debug)
        # Filter the xml file with the tag_filter and search the tag_content
        for option in self.xml_options:
            current_tag_content = option.find(tag_filter).text
            if current_tag_content:
                # Verify if expected item content exists in the current item found
                if tag_content in current_tag_content:
                    sub_tag_content = option.find(sub_tag).text
                    if sub_tag_content is not None: # subtag exists but with an empty value
                        subtag_str = "<%s>" % sub_tag + sub_tag_content +"</%s>" % sub_tag
                        debug_str = "Parameter found. Extracting information" + \
                                    " from '%s'" % subtag_str
                        LOG.info(debug_str, self.is_debug)
                        LOG.info("The content recovered is '%s'" % sub_tag_content, 
                                 self.is_debug)
                        return sub_tag_content
        LOG.info("No matches for %s" % tag_str, self.is_debug)
        LOG.info("The value was not recovered", self.is_debug)
        return ""

    def modify_value(self, tag_content="", tag="", sub_tag="",
                     new_sub_tag_content=""):
        """ 
        Modify the value of a subitem.
        Parameters:
            tag_content  - Content to be searched
            tag - Name of the item to compare its content
            sub_tag - Element from which content will be modified
            new_sub_tag_content - new element content
        Returns :
            value_is_modified -  Boolean status (True content modified)
        """
        # If not items were defined, the function return True
        if tag and sub_tag :
           tag_str = "<%s>" %tag + tag_content + "</%s>" % tag
           LOG.info("Searching the parameter with %s element" % tag_str, self.is_debug)
        else :
          LOG.info("Searching the tag <%s>" % tag_content, self.is_debug)
        
        for option in self.xml_options:
            # Modify an element in a subitem
            if tag and sub_tag :
                # String structures (only for debug)
                if tag == sub_tag:
                    current_element = option.find(tag)
                    if current_element is not None:
                        tag_str = "Parameter found. Verifying information of '"
                        tag_str += "<%s>" %tag + current_element.text + "</%s>'" %tag
                        LOG.info(tag_str, self.is_debug)
                        if current_element.text == new_sub_tag_content :
                            str_debug = "The tag <%s> " % tag  +\
                                        "has already the correct content (%s)" % new_sub_tag_content
                            LOG.info("The modification is not necessary", 
                                     self.is_debug)
                            LOG.info(str_debug, self.is_debug)
                            self.do_save_file = False
                        else :
                            self.do_save_file = True
                            str_debug = "The tag <%s> content" %tag +\
                                         "was updated to '%s'" % new_sub_tag_content
                            LOG.info(str_debug, self.is_debug)
                            current_element.text = new_sub_tag_content
                        return True # Value was modified
                else :
                    current_tag_content = option.find(tag).text
                    if current_tag_content:
                        if current_tag_content == tag_content or \
                                current_tag_content.replace('-', '', 2) == tag_content:
                            current_element = option.find(sub_tag)
                            if current_element is not None:
                                tag_str = "Parameter found. Verifying information of '"
                                current_element_content = ""
                                if current_element.text is not None :
                                     current_element_content = current_element.text
                                tag_str += "<%s>" %sub_tag + \
                                           current_element_content + "</%s>'" %sub_tag
                                LOG.info(tag_str, self.is_debug)
                                if current_element.text == new_sub_tag_content :
                                    str_debug = "The tag <%s> " % sub_tag  +\
                                                "has already the correct content (%s)" % new_sub_tag_content
                                    LOG.info("The modification is not necessary", self.is_debug)
                                    LOG.info(str_debug, self.is_debug)
                                    self.do_save_file = False
                                else :
                                    self.do_save_file = True
                                    str_debug = "The tag <%s> content " %sub_tag +\
                                                "was updated to '%s'" % new_sub_tag_content
                                    LOG.info(str_debug, self.is_debug)
                                    current_element.text = new_sub_tag_content
                                return True # Value was modified
            else:
                # String structures (only for debug)
                tag_str = "<%s>" %tag_content + "</%s>" % tag_content
                # Modify an element in a principal item
                current_element = option.find(tag_content)
                if current_element is not None:
                    if current_element.text == new_sub_tag_content :
                        str_debug = "The tag <%s> " % tag_content  +\
                                    "has already the correct content (%s)" % new_sub_tag_content
                        LOG.info("The modification is not necessary", self.is_debug)
                        LOG.info(str_debug, self.is_debug)
                        self.do_save_file = False
                    else :
                        self.do_save_file = True
                        str_debug = "The tag <%s> content " %tag_content +\
                                    "was updated to '%s'" % new_sub_tag_content
                        LOG.info(str_debug, self.is_debug)
                        current_element.text = new_sub_tag_content
                    return True # Value was modified

        # Modify trace as an error
        LOG.info("The parameter or the tag content were not found", self.is_debug)
        return False

    def remove_param_item(self, tag, tag_content):
        """ 
        Remove an entire parameter. To choose the parameter to delete it is
        necessary to define the command
        Parameters:
           tag  - Tag to filter the xml parameters
           tag_content - Tag content to define the parameter to delete
        Returns :
            param_deleted = Status of the parameter to be deleted
        """
        # Filter all items with Param
        self.do_save_file = True
        for child in self.xmlContent.findall('.//' + "Param" + '/..'):
            for param in child.findall('Param'):
                # Filter all the subitems with Command
                for item in param.findall(tag):
                    # Verify if the command to be deleted exists
                    if tag_content in ET.tostring(item).decode("utf8"):
                        try :
                            # Deleted the param item
                            child.remove(param)
                            str_debug = "Parameter with tag <%s>" %tag + \
                                        tag_content + "</%s> was removed" %tag
                            LOG.info(str_debug, self.is_debug)
                            return True # The parameter was deleted
                        except Exception as e :
                            LOG.info(e, self.is_debug)
                            return False
        LOG.info("Parameter not found. Perhaps it has already been removed", self.is_debug)
        self.do_save_file = False
        return True

    def add_param_item(self, tag, param_dictionary, param_idx):
        """ 
        Add an entire parameter to the xml file
        Parameters:
            tag - Tag to filter the xml file and verify if param_dictionary[tag]
                  already exists in the xml file
            param_dictionary  - Dictionary with all the values to be saved
        Returns :
            param_added = Status of the parameter to be deleted
        """
        # Verify if Name item exists in the dictionary (mandatory value)
        self.do_save_file = True
        param_name = param_dictionary[tag]
        # Verify if param already exists (it is not possible to have two
        # time the same item)
        self.get_items(tag)
        if not self.get_value(param_name, tag, tag_filter=tag):
            try :
                # Access to the xmlContent item
                root_node = self.xmlContent.find("McubootFormat")
                # Add new subitem
                new_param = ET.Element('Param')
                # Add elements to the subitem
                for element in param_dictionary:
                    child = ET.SubElement(new_param, element)
                    child.text = param_dictionary[element]
                # Format the subitem
                ET.indent(new_param, space='	', level=1)
                new_param.tail = '\n	'
                # Insert to the principal structure
                LOG.info("Adding new parameter at the second position", self.is_debug)
                root_node.insert(param_idx, new_param)
                return True
            except Exception as e :
                LOG.info(e, self.is_debug)
                return False
        else : 
            LOG.info("Parameter already exists", self.is_debug)
            self.do_save_file = False
            return True
        return False

    def save_file(self):
        if self.do_save_file:
            LOG.info("Saving xml modifications", self.is_debug)
            # Modified and save the xml modified
            xml_data = ET.tostring(self.xmlContent, encoding="UTF-8",
                                   method='xml', xml_declaration=True,
                                   short_empty_elements=False)
            xml_data = xml_data.decode("utf8").replace('\'', '\"')
            with open(self.xml_file, 'w') as xml_output_file:
                xml_output_file.write(xml_data)
