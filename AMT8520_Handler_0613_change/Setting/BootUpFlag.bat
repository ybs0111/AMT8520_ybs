@ECHO OFF
CLS

SET FILENAME=C:\AMT8520\Setting\BootUpFlag.txt	
DEL %FILENAME%
 			
ECHO BootUpFlag = 1 >> %FILENAME%

ECHO. >> %FILENAME%
