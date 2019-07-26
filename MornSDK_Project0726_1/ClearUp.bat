@echo off
cls
cd MornSDK\Debug
del /f /q *.*
echo Delete temp files in \Debug, OK! 
cd ..\Release
del /f /q *.*
echo Delete temp files in \Release, OK!
cd ..\..\Bin
del /f /q "*.sup" "*.pdb" "*.exp" "*.ilk"
echo Delete Temp files in \Bin, OK!
pause
@echo on
