@SubWCRev .\ include\SubWCRev.conf include\VersionMod.h
@if %ERRORLEVEL% NEQ 0 goto :NoSubWCRev

:NoSubWCRev
@echo #pragma once > include\VersionMod.h
@echo. >> include\VersionMod.h
@echo #define DO_MAKE_STR(x) #x >> include\VersionMod.h
@echo #define MAKE_STR(x) DO_MAKE_STR(x) >> include\VersionMod.h
@echo. >> include\VersionMod.h
@echo #define VERSION_MAJOR 1 >> include\VersionMod.h
@echo #define VERSION_MINOR 1 >> include\VersionMod.h
@echo #define VERSION_REV   0 >> include\VersionMod.h
@echo #define VERSION_PATCH 0 >> include\VersionMod.h
