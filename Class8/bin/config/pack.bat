set path=%path%;"c:\program files\winrar\"

echo rar config
del config.dat

attrib -R /S /D
winrar a config.zip config.ini -pBMClass@2018

ren config.zip config.dat

mkdir .\..\..\setup\bin\BMClass\config\
xcopy /D /Y config.dat .\..\..\setup\bin\BMClass\config\ 

pause