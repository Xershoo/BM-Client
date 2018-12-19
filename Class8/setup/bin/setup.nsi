icon ".\Setup.ico"
OutFile "bmclass_setup.exe"
SilentInstall silent

section
sectionend

VIProductVersion "1.0.0.1150"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileDescription" "BMClass Client"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileVersion" "1.0.0.1150"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "ProductName" "BMClass Client Install Program"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "ProductVersion" "1.0.0.1150"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "LegalCopyright" "Copyright @ 2018-2020 bang mang data Co.,Ltd. All rights reserved"

Function .onInit
InitPluginsDir
SetOutPath $PLUGINSDIR
  File "Qt5Gui.dll"
  File "Qt5Core.dll"
  File "Qt5Widgets.dll"
  File "Qt5Widgets.dll"
  File "msvcp120.dll"
  File "msvcr120.dll"
  File "msvcp100.dll"
  File "msvcr100.dll"
  File "qt.conf"
  File /r "plugins\"
  File "UnRAR.exe"
  File "exePath"
  File "BMClass.rar"
  File "BMClassSetup.exe"
FunctionEnd

Function .onInstSuccess
call fun
FunctionEnd

Function fun
   ExecWait $PLUGINSDIR\BMClassSetup.exe
   RMDir $PLUGINSDIR
FunctionEnd


