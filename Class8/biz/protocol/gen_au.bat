@echo off
rd /S /Q protocols_au
call ..\\protocol_generator.exe "./tool/gnet.au.xml" "au" ""