set path=%path%;"c:\program files\winrar\"

echo 0. encode xml
md style
layoutenc

echo 1. pack style
cd style
del style.gt

attrib -R /S /D
winrar a  style.zip -pBMClass@2018

ren style.zip style.gt
cd ..

echo 2. pack image_default
cd image_default
del image.gt

attrib -R /S /D
winrar a  image.zip -pBMClass@2018

ren image.zip image.gt
cd ..

echo 3. copy files
md data
md themes\default

move style\style.gt 		data\style.gt
move image_default\image.gt 	themes\default\image.gt

md ..\..\bin\data
md ..\..\bin\themes
md ..\..\bin\themes\default

md ..\..\setup\bin\BMClass\data
md ..\..\setup\bin\BMClass\themes
md ..\..\setup\bin\BMClass\\themes\default

xcopy data 		..\..\bin\data\ /y/q
xcopy themes\default 	..\..\bin\themes\default\ /y/q

xcopy data 		..\..\setup\bin\BMClass\data\ /y/q
xcopy themes\default 	..\..\setup\bin\BMClass\themes\default\ /y/q
pause;