@echo off
rem First argument is path to build directory
rem We need to copy required dll's there
copy %AMD_AGS_PATH%\ags_lib\lib\amd_ags_x64.dll %1\amd_ags_x64.dll