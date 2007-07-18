@make
@echo off
if exist "%MAXPATH%\plugins" (
        copy ..\..\..\dist\bin\megaexp.dle "%MAXPATH%\plugins"
        start "" "%MAXPATH%\3dsmax.exe"
)
