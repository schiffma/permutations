:: Destination path - specify where to move package after build
SET DEST_PATH=C:\Users\schiffma\Documents\GitHub\permutations

SET SQLITE_DIR=C:\Users\schiffma\Documents\GitHub\permutations

:: WIN64 SQLITE BUILD PROCEDURE

:: Set path variables
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

CD %SQLITE_DIR%

:: Build extensions

cl /MD permutations.c -link -dll -out:permutations_64.dll

pause



