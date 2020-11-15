@echo off
set PS3SDK=/c/PSDK3v2
set WIN_PS3SDK=C:/PSDK3v2
set PATH=%WIN_PS3SDK%/mingw/msys/1.0/bin;%WIN_PS3SDK%/mingw/bin;%WIN_PS3SDK%/ps3dev/bin;%WIN_PS3SDK%/ps3dev/ppu/bin;%WIN_PS3SDK%/ps3dev/spu/bin;%WIN_PS3SDK%/mingw/Python27;%PATH%;
set PSL1GHT=%PS3SDK%/psl1ght
set PS3DEV=%PS3SDK%/ps3dev

:: ------------------------------------------------------------------
:: Simple script to build a proper PKG using Python (by CaptainCPS-X)
:: ------------------------------------------------------------------
@echo off
cd %~dp0

:: Change this depending where you installed Python...
set PYTHON=c:\Python27

:: Don't change these...
set PATH=%PYTHON%;%PATH%
set PKG=.\pypkg\pkg_custom.py

:: Change these for your application / manual...
set CONTENTID=UP0001-PKGLAUNCH_00-0000000000000000
set PKG_DIR=./pkg/
set PKG_NAME=./%CONTENTID%.pkg

:: This will run the Python PKG script...
python.exe %PKG% --contentid %CONTENTID% %PKG_DIR% %PKG_NAME%

pause