import ctypes

class bcolors:
    NORMAL = '\033[37m'
    RED    = '\033[31m'
    GREEN  = '\033[32m'
    BLUE   = '\033[34m'
    YELLOW = '\033[33m'
    CYAN   = '\033[36m'
    RESET  = '\033[0m'
    BOLD   = '\033[1m'
    UNDERLINE = '\033[4m'

def init_console():
    kernel32 = ctypes.windll.kernel32
    kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)

def print_condition(condition, txt):
    """
        Print Text depending on condition
    """
    if(condition):
        print(txt)

def interface_select_file( msg, lstOfElts ):
    """
        Show each element of the list and ask to select one of them.

        Args:
            msg       : (str) Message to show before selection
            lstOfElts : (List) List of Elements to select
    """
    print( "\n" + msg ) # Add an empty line before showing the selection
    for id, foldername in enumerate( lstOfElts, start = 1 ):
            print("\t{} - {}".format(id, foldername))

    while True:
        res = input("\t[1-{}] or [E]xit ?: ".format(len(lstOfElts)))
        if( res.isnumeric() ):
            index = int(res)
            if(( index >= 1 ) and (index <= len(lstOfElts))):
                return 0, lstOfElts[index - 1]
            else:
                print("\t[Error] Value must be in range [1-{}]. Try again...".format(len(lstOfElts)))
        elif( res.lower() == 'e'):
            return -1, ""
        else:
            print("\t[Error] Value must be in range [1-{}]. Try again...".format(len(lstOfElts)))

    return -2, ""

def colored(r, g, b, text):
    return "\033[38;2;{};{};{}m{} \033[38;2;255;255;255m".format(r, g, b, text)

def print_error_message(text) :
    colored_text = colored(255, 0, 0, text)
    print (colored_text)

def print_colored(color, txt, arg):
    """
        Print colored text
    """
    res = type(arg) == str
    if (res == True):
        print("{}{}{}".format(color,txt,bcolors.RESET),str(arg))
    else :
        print("{}{}{}".format(color,txt,bcolors.RESET),arg)