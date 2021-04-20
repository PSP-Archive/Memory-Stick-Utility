@echo off
TITLE Memory Stick Utility

FOR /R "data\IPLs" %%i IN (*) DO bin2c "%%~pnxi" "%%~pni.h" "%%~ni"
FOR /R "data\PRXs" %%i IN (*) DO bin2c "%%~pnxi" "%%~pni.h" "%%~ni"

make
pause

DEL /F /Q /S data\*.h
DEL /F /Q /S *.elf
DEL /F /Q /S *.o
DEL /F /Q /S *.sfo

CD ..
RMDIR /S /Q "PSP\GAME\Memory Stick Utility"
MKDIR "PSP\GAME\Memory Stick Utility"
MOVE "src\EBOOT.PBP" "PSP\GAME\Memory Stick Utility"