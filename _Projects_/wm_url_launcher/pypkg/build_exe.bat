@c:\Python27\scripts\pyinstaller.exe --onefile pkg_custom.py
@del pkg_custom.spec
@del /s/q build\*.*
@rd /s/q build
