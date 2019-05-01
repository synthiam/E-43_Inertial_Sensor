@echo off

echo This maps the G:\ to this folder so there are no spaces in the filename. This is required for ST Link debugging 
echo.
echo You must launch the SLN file from the newly created G:
echo.
if exist g:\ ( subst g: /d )

subst g: .

start g:

pause