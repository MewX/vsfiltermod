@if not exist "%programfiles%\TortoiseSVN\bin\SubWCRev.exe" goto :x64

@"%programfiles%\TortoiseSVN\bin\SubWCRev.exe" .\ include\SubWCRev.conf include\VersionMod.h
@goto :eof

:x64
@if not exist "%ProgramW6432%\TortoiseSVN\bin\SubWCRev.exe" goto :NoSubWCRev

@"%ProgramW6432%\TortoiseSVN\bin\SubWCRev.exe" .\ include\SubWCRev.conf include\VersionMod.h

:eof
