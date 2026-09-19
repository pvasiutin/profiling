@echo off
SetLocal EnableDelayedExpansion

set APP_NAME=profiler2
set PROJECT_DIR=%~dp0
set BUILD_DIR=%PROJECT_DIR%\build
set SRC_DIR=%PROJECT_DIR%\source
set SOURCES=%SRC_DIR%\main.cpp %SRC_DIR%\PlatformMetrics.cpp %SRC_DIR%\ProfilingUtilities.cpp
set CL_FLAGS=/nologo /std:c++20 /EHsc /Zi /W4 /Od /Fe%APP_NAME%
set ADDITIONAL_INCLUDE_DIRS=/I%PROJECT_DIR%\source
set LINKER_FLAGS=/INCREMENTAL:NO 

::
:: Compile target
::
:compile
    call :header "Compiling"

    if not exist %BUILD_DIR% mkdir %BUILD_DIR%

    pushd %BUILD_DIR%
        set _ADDITIONAL_INCLUDE_DIRS_=
        for %%L in (%ADDITIONAL_INCLUDE_DIRS%) do (set _ADDITIONAL_INCLUDE_DIRS_=!_ADDITIONAL_INCLUDE_DIRS_! /I%%L)

        %PROJECT_DIR%\ctime.exe -begin %APP_NAME%.ctm
            cl.exe %CL_FLAGS% %SOURCES% %_ADDITIONAL_INCLUDE_DIRS_% /link %LINKER_FLAGS%
            set CL_EXIT_CODE=%ERRORLEVEL%
        %PROJECT_DIR%\ctime.exe -end %APP_NAME%.ctm
        del %APP_NAME%.ctm
    popd

    call :footer "Compiling"
    exit /b %CL_EXIT_CODE%
goto :eof

:header
    echo ========== %~1...       ==========
goto :eof

:footer
    echo ========== %~1... Done^^! ==========
    echo.
goto :eof
