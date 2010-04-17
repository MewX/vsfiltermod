VSFilterMod (with x64 and SSE2 support)

Additional Features: png textures, some gradients, distorsion,
nonlinear moves, moveable vector clip, vertical spacing and some other stuff.


### Requirements for 32-bit and 64-bit builds ###

* Visual Studio 2010
http://www.microsoft.com/visualstudio/en-us/

* DirectX SDK (latest version)
http://msdn.microsoft.com/en-us/directx/aa937788.aspx

* Microsoft Windows SDK for Windows 7
http://www.microsoft.com/downloads/details.aspx?familyid=C17BA869-9671-4330-A63E-1FD44E0E2505&displaylang=en

* YASM
http://www.tortall.net/projects/yasm/wiki/Download


### Configuring Visual Studio ###

You need to add the locations of the DirectX SDK directories to the settings in Visual Studio.
This is usually already done automatically by the DirectX SDK installer.

1) In the Visual Studio menu, go to: Tools -> Options -> Projects and Solutions -> VC++ Directories
2) Add the location of the Include directory of the latest DirectX SDK.
3) Repeat the step 2 for the x64 platform.
4) Add the location of the Lib\x86 directory of the latest DirectX SDK to the Win32 platform.
5) Add the location of the Lib\x64 directory of the latest DirectX SDK to the x64 platform.
 
 
### Integration of Windows 7 SDK (Optional) ###

To build VSFilterMod with Windows 7 SDK follow these steps:

1) Download and install Windows 7 SDK from Microsoft website (http://www.microsoft.com/downloads/details.aspx?FamilyID=71deb800-c591-4f97-a900-bea146e4fae1&displaylang=en)
2) Open regional settings, and set it to English (see this link for explanations : http://blogs.msdn.com/windowssdk/archive/2009/08/21/windows-sdk-configuration-tool-may-report-an-error-when-os-display-format-is-not-english.aspx)
3) Start "Windows SDK Configuration Tool" from start menu, and set "Installed Windows SDK" to "v7.0"
4) Revert your regional settings to initial value