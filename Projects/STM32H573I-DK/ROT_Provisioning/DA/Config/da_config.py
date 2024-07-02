import configparser
import logging
import os
from singleton import Singleton

logger = logging.getLogger()


class DA_Config(metaclass=Singleton):
    def __init__(self, da_config_path):
        self.config_path = da_config_path
        if (os.path.exists(da_config_path)):
            self.config = configparser.ConfigParser()
            self.config.read(da_config_path)
            self.load_sm()
            self.load_config()
            self.print_config()
        else:
            logger.error(f"DA config file doesn't exist {da_config_path}")
            exit(-1)

    def load_sm(self):
        if (self.config != None):
            self.sm_path = self._real_path_config(self.config_path, self.config["sm"]["path"])
        else:
            logger.error("Config not loaded")
            exit(-1)

    def load_config(self):
        if (self.config != None):
            self.xml = self._real_path_config(self.config_path, self.config["config"]["xml"])
            self.key = self._real_path_config(self.config_path, self.config["config"]["key"])
            if (not os.path.exists(self.key)):
                logger.error("DA key ({}) doesn't exists".format(self.key))
                exit(-1)
            self.certificate = self._real_path_config(self.config_path, self.config["config"]["certificate"])
            if (not os.path.exists(self.certificate)):
                logger.error("DA Certificate ({}) doesn't exists".format(self.certificate))
                exit(-1)
            self.password = self._real_path_config(self.config_path, self.config["config"]["password"])
            self.password_obk = self._real_path_config(self.config_path, self.config["config"]["password_obk"])
        else:
            logger.error("Config not loaded")
            exit(-1)

    def get_sm_path(self):
        return self.sm_path

    def get_xml_path(self):
        return self.xml

    def get_key_path(self):
        return self.key

    def get_certificate_path(self):
        return self.certificate

    def get_password_path(self):
        return self.password

    def get_password_obk_path(self):
        return self.password_obk

    def print_config(self):
        logger.debug("=== Config Debug Authentication tools ===")
        logger.debug("\tDA Config File \t: {}".format(self.config_path))
        logger.debug("\tSM path \t: {}".format(self.sm_path))
        logger.debug("\tDA Xml \t\t: {}".format(self.xml))
        logger.debug("\tDA Keys \t: {}".format(self.key))
        logger.debug("\tDA Certificate \t: {}".format(self.certificate))
        logger.debug("==================================")

    def _real_path_config(self, ini_path, ini_value):
        return os.path.abspath(os.path.join(os.path.abspath(os.path.dirname(ini_path)), ini_value))
