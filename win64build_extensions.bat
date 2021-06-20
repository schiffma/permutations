:: Extension path - specify where this extension will be built.
SET EXT_PATH=C:\Users\schiffma\Documents\GitHub\permutations

:: Set path variables
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

CD %EXT_DIR%

:: Build extensions
cl /MD permutations.c -link -dll -out:permutations_64.dll

pause
