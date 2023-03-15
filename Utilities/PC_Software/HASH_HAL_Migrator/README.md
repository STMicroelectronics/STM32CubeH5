## <b>HASH_HAL_Migrator Script Description</b>

The HASH_HAL_Migrator is a tool that allow to migrate a HASH project using HAL HASH legacy APIs to use the new APIs.

#### <b>Installation</b>

This tool requires the following tools to be installed:

- Python v3.9 or greater
- colorama V0.3.7 or greater

#### <b>Note</b>

The HASH_HAL_Migrator parses the HASH structures, APIs and macros used in a given project that the user specify as input.
Once identified, the tool will replace the legacy definitions with their corresponding ones in the new HAL HASH drivers.
The original files are renamed into *_org.c/.h (ie: main.c renamed into main_org.c). The modification are applied into the original file.
In case a legacy feature is no more supported (ie: MD5), the tool display a warning message and keep the legacy API so the user get a compilation error and fix it.

The tool will display the following information once executed:

 - All identified source and header files to be modified
 - The updated structures, APIs and macros in green color
 - The functions that are no more supported and need to be removed from the application in red color
 - The updated files in white bold color

### <b>How to use it ?</b>

To use this tool, please do the following :

  1. The first step is to open the bash at the location of the script,

  2. Precise the path of the project you want to migrate
	 Example: py HASH_HAL_Migrator.py -p "__PROJECT_PATH_"

  3. Execute the script, (example : py HASH_HAL_Migrator.py -p "/c/STM32Cube_FW_H7_V1.2.0/Projects/STM32H747I-EVAL/Examples/HASH/HASH_SHA224SHA256_DMA/")
  
  4. Check generated files and compile the project

