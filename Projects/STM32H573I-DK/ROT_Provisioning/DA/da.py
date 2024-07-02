import sys
import argparse
import logging
import configparser
import os

# Define the default arguments
DEFAULT_ARGS = {
    "da_config_file": os.path.normpath(os.path.join(os.path.dirname(__file__), "./Config/da.ini")),
}

# Set once SM path is known
logger = None


def process_arg_path(path):
    if (not os.path.isabs(path)):
        return os.path.abspath(os.path.join(os.getcwd(), path))
    return path


def save_config_path(da):
    config = configparser.ConfigParser()
    config["path"] = {'da': da}
    with open(os.path.join(os.path.dirname(__file__), "config_path.ini"), 'w') as configfile:
        config.write(configfile)


def get_da_config_file(arg_obj):
    arg = None
    config = configparser.ConfigParser()
    if arg_obj:
        arg = arg_obj
    else:
        try:
            config.read(os.path.join(os.path.dirname(os.path.abspath(__file__)), "config_path.ini"))
            abs_path = config["path"]["da"]
            arg = os.path.relpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), abs_path))
        except Exception as e:
            arg = DEFAULT_ARGS["da_config_file"]

    return arg


class ProductAlreadyStateOpen(Exception):
    """Exception raised when the product is already in OPEN state."""

    def __init__(self, message="The product state is already OPEN"):
        self.message = message
        super().__init__(self.message)


class ProductStateLocked(Exception):
    """Exception raised when the product is in LOCKED state."""

    def __init__(self, message="The product state is LOCKED"):
        self.message = message
        super().__init__(self.message)


class ProductStateUnknown(Exception):
    """Exception raised when the product is in UNKNOWN state."""

    def __init__(self, message="The product is in UNKNOWN state"):
        self.message = message
        super().__init__(self.message)


def regression(cube_prog, da_config, silent=False):
    lifecycle_state = cube_prog.get_lifecycle_state()
    logger.debug(f"Product state {lifecycle_state}")
    if (lifecycle_state == "PSA_LIFECYCLE_UNKNOWN"):
        raise ProductStateUnknown
    if (lifecycle_state == "ST_LIFECYCLE_LOCKED"):
        raise ProductStateLocked
    if (lifecycle_state == "ST_LIFECYCLE_OPEN"):
        raise ProductAlreadyStateOpen
    cube_prog.close_debug()
    cube_prog.disable_trustzone()
    cube_prog.reset()
    cube_prog.provision_ps_provisioning(process_arg_path(da_config.password_obk))
    cube_prog.regression(process_arg_path(da_config.key), process_arg_path(
        da_config.certificate), process_arg_path(da_config.password), silent)


def _print_da(message, from_prov=False, error=False):
    global logger
    if error:
        logger_print = logger.error
    else:
        logger_print = logger.info
    if not from_prov:
        logger_print("=====")
        logger_print(message)
        logger_print("=====")
    else:
        logger_print(f"\t\t{message}")


def print_regression_success(from_prov=False):
    _print_da("Successful regression", from_prov)


def print_regression_not_needed(from_prov=False):
    _print_da("Regression not needed, product already in state OPEN", from_prov)


def print_regression_locked_state(from_prov):
    _print_da("Product state is LOCKED. Regression is not possible", from_prov, error=True)


def print_da_unknown_state(from_prov=False):
    _print_da("Product state is UNKNOWN. Please unplug/plug your board and retry", from_prov, error=True)


def print_dbg_auth_success():
    _print_da("dbg auth successful")


def print_dbg_auth_open_state():
    _print_da("Product state is OPEN. --dbg-auth is not possible", error=True)


def print_dbg_auth_locked_state():
    _print_da("Product state is LOCKED. --dbg-auth is not possible", error=True)


def discovery(cube_prog):
    cube_prog.discovery()


class DbgAuthNotAllowedInOpen(Exception):
    """Exception raised when the product is an OPEN state."""

    def __init__(self, message="The product state is OPEN, --dbg-auth not supported"):
        self.message = message
        super().__init__(self.message)


class DbgAuthNotAllowedInLocked(Exception):
    """Exception raised when the product is in LOCKED state."""

    def __init__(self, message="The product state is LOCKED, --dbg-auth not supported"):
        self.message = message
        super().__init__(self.message)


def debug_authentication(cube_prog, da_config, dbg_auth_value=None):
    lifecycle_state = cube_prog.get_lifecycle_state()
    logger.debug(f"Product state {lifecycle_state}")
    if (lifecycle_state == "PSA_LIFECYCLE_UNKNOWN"):
        raise ProductStateUnknown
    if (lifecycle_state == "ST_LIFECYCLE_LOCKED"):
        raise DbgAuthNotAllowedInLocked
    if (lifecycle_state == "ST_LIFECYCLE_OPEN"):
        raise DbgAuthNotAllowedInOpen
    cube_prog.debug_auth(da_config, dbg_auth_value)

    # Get SM path from project.ini


def get_path_sm(ini_path, config_project):
    return os.path.abspath(os.path.join(os.path.dirname(ini_path), config_project['sm']['path']))


def main(args, silent=False, create_log_file=True, from_prov=False):
    global logger
    # Gettin SM path
    da_config_file = get_da_config_file(args.da_config_file)
    da_config_path = process_arg_path(da_config_file)
    temp_da_config = configparser.ConfigParser()
    temp_da_config.read(da_config_path)
    path_sm = temp_da_config["sm"]["path"]
    path_sm = os.path.abspath(os.path.join(os.path.dirname(da_config_path), path_sm))

    # Importing from PyHelper & DA/Config
    sys.path.append(os.path.abspath(os.path.join(path_sm, "PyHelper")))
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "Config")))
    from cubeprog import CubeProgrammer_DA
    from tools_config import Tools_Config
    from sm_config import SM_Config
    from da_config import DA_Config
    from log import SecureManagerLogger
    from helper import load_config_path

    # Settings the logs
    if (create_log_file):
        da_logger = SecureManagerLogger("da.log")
    logger = logging.getLogger()

    # Loading the right tools.ini
    config_path = load_config_path()
    tools_ini = config_path['path']['tools']

    if (args.clear):
        config_path_ini = os.path.join(os.path.dirname(__file__), "config_path.ini")
        if (os.path.exists(config_path_ini)):
            os.remove(config_path_ini)
    if (args.verbose and create_log_file):
        da_logger.set_print_debug_level(logging.DEBUG)

    # Setting up CubeProgrammer with the right tools
    da_config = DA_Config(da_config_path)
    save_config_path(da_config_path)
    sm_config = SM_Config(config_path['path']['sm'])
    tools_config = Tools_Config(tools_ini, sm_config.get_generated_by_cubemx())
    cube_prog = CubeProgrammer_DA(tools_config)

    # Processing the args
    if args.regression:
        try:
            regression(cube_prog, da_config, silent)
            print_regression_success(from_prov)
        except ProductAlreadyStateOpen:
            print_regression_not_needed(from_prov)
        except ProductStateUnknown:
            print_da_unknown_state(from_prov)
            exit(-1)
        except ProductStateLocked:
            print_regression_locked_state(from_prov)
            exit(-1)

    elif args.discovery:
        discovery(cube_prog)
    elif args.dbg_auth:
        dbg_auth_value = None
        if (args.dbg_auth != True):
            dbg_auth_value = args.dbg_auth
        try:
            debug_authentication(cube_prog, da_config, dbg_auth_value)
        except DbgAuthNotAllowedInOpen:
            print_dbg_auth_open_state()
            exit(-1)
        except ProductStateUnknown:
            print_da_unknown_state()
            exit(-1)
        except DbgAuthNotAllowedInLocked:
            print_dbg_auth_locked_state()
            exit(-1)
        print_dbg_auth_success()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Debug Authentication')
    parser.add_argument('-v', '--verbose', action="store_true", help='Set log to debug')
    parser.add_argument('-d', '--da-config-file', type=str,
                        help=f'Path to da config file (default da.ini: {DEFAULT_ARGS["da_config_file"]})')

    # Add mutually exclusive group for mandatory options
    mandatory_group = parser.add_mutually_exclusive_group(required=True)
    mandatory_group.add_argument('--regression', action='store_true', help='Trigger a regression')
    mandatory_group.add_argument('--dbg-auth', const=True, default=False, nargs='?', help='Trigger a dbg auth')
    mandatory_group.add_argument('--discovery', action='store_true', help='Trigger a DA discovery')

    parser.add_argument('-c', '--clear', action="store_true", help='Clear saved args')
    args = parser.parse_args()

    try:
        main(args)
    except KeyboardInterrupt:
        logger.error("DA process was manually terminated. Exiting now.")
        exit(-1)
