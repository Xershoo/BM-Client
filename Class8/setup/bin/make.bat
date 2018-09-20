@echo Compress Files
rar a -r -ep1 BMClass.rar BMClass

@echo Make Setup Packet
makensis.exe setup.nsi

del BMClass.rar
@echo end.