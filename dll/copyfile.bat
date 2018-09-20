@echo *******************************************
@echo Copy DLL Files
@echo Solution Dirctory:%1
@echo Project Name:%2
@echo Configuration:%3

xcopy /D /Y %1dll\*.dll %1%2\bin\%3
xcopy /D /Y %1dll\%3\*.dll %1%2\bin\%3\

@echo *******************************************