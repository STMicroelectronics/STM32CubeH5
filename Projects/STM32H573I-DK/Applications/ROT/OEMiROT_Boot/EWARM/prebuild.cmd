iccarm.exe --cpu=Cortex-M33 -DBL2  -I%1\..\Inc %1\stm32h573xx_flash.icf  --silent --preprocess=ns %1\flash.icf.i > %1\prebuild.log 2>&1
iccarm.exe --cpu=Cortex-M33 -DBL2  -I%1\..\Inc %1\..\src\image_macros_to_preprocess_bl2.c  --silent --preprocess=ns %1\image_macros_preprocessed_bl2.c >> %1\prebuild.log 2>&1
