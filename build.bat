@echo off
pushd "%~dp0"

where cl.exe > nul 2> nul
if not %ERRORLEVEL%==0 ( echo.cl.exe not found && exit /b 1 )

if "%~1"=="/d" call :install_dep

if not exist "bin" mkdir bin
cl /nologo /O1 /sdl- /GS- /I phnt src\*.c /Fe:bin\winboottext.exe /link /subsystem:native /nodefaultlib ntdll\ntdll64.lib
del *.obj

popd
exit /b

:install_dep
	where git.exe > nul 2> nul
	if not %ERRORLEVEL%==0 ( echo.git.exe not found && exit /b 1 )

	where nmake.exe > nul 2> nul
	if not %ERRORLEVEL%==0 ( echo.nmake.exe not found && exit /b 1 )

	git clone https://github.com/winsiderss/phnt

	git clone https://github.com/Fyyre/ntdll
	cd ntdll
	nmake msvc
	cd ..
goto :eof
