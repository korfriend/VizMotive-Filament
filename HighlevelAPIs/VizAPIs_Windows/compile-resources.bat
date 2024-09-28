@echo off
setlocal enabledelayedexpansion

set MATC_PATH=..\..\VisualStudio\tools\matc\Release\matc.exe
if not exist "%MATC_PATH%" (
    echo Error: MATC executable not found at %MATC_PATH%
    exit /b 1
)

set MATERIAL_DIR=..\..\HighlevelAPIs\API_SOURCE\generated\mat
if not exist "%MATERIAL_DIR%" mkdir "%MATERIAL_DIR%"

set MATC_FLAGS=-a opengl -a vulkan -p desktop -g
set MATERIAL_SRCS=..\..\HighlevelAPIs\API_SOURCE\mat\compositor.mat
set RESOURCE_BINS=
set ERROR_COUNT=0

for %%F in (%MATERIAL_SRCS%) do (
    set "localname=%%~nF"
    set "fullname=%%~fF"
    set "output_path=%MATERIAL_DIR%\!localname!.filamat"
    
    echo Compiling material: !fullname!
    echo Output: !output_path!
    "%MATC_PATH%" %MATC_FLAGS% -o "!output_path!" "!fullname!"
    
    if !errorlevel! neq 0 (
        echo Error: Compilation failed for %%F
        set /a ERROR_COUNT+=1
    ) else (
        echo Success: Compiled %%F
        set "RESOURCE_BINS=!RESOURCE_BINS! !output_path!"
    )
)

if %ERROR_COUNT% equ 0 (
    echo All materials compiled successfully.
) else (
    echo Compilation completed with %ERROR_COUNT% error(s^).
    exit /b 1
)

set ARCHIVE_DIR=..\..\HighlevelAPIs\API_SOURCE\generated\res
if not exist "%ARCHIVE_DIR%" mkdir "%ARCHIVE_DIR%"

set ARCHIVE_NAME=mat_internal
set OUTPUTS=%ARCHIVE_DIR%\%ARCHIVE_NAME%.bin %ARCHIVE_DIR%\%ARCHIVE_NAME%.h
set RESGEN_HEADER=%ARCHIVE_DIR%\%ARCHIVE_NAME%.h
set RESGEN_OUTPUTS=%OUTPUTS% %ARCHIVE_DIR%\%ARCHIVE_NAME%.c
set RESGEN_FLAGS=-qcx "%ARCHIVE_DIR%" -p %ARCHIVE_NAME%
set RESGEN_SOURCE=%ARCHIVE_DIR%\%ARCHIVE_NAME%.c

echo Aggregating resources...
set RESGEN_PATH=..\..\VisualStudio\tools\resgen\Release\resgen.exe
if not exist "%RESGEN_PATH%" (
    echo Error: resgen executable not found at %RESGEN_PATH%
    exit /b 1
)

"%RESGEN_PATH%" %RESGEN_FLAGS% %RESOURCE_BINS%
if !errorlevel! neq 0 (
    echo Error: Failed to aggregate resources
    exit /b 1
)

echo Resources aggregated successfully.

for %%F in (%RESGEN_OUTPUTS%) do (
    if not exist "%%F" (
        echo Error: Expected output file not found: %%F
        exit /b 1
    )
)

echo All expected output files generated.
echo Process completed successfully.

endlocal
