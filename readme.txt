VSFilterMod (with x64 and SSE2 support)

Additional Features: png textures, gradients, distorsion,
nonlinear moves, moveable vector clip, vertical spacing and some other stuff.


### Requirements for 32-bit and 64-bit builds ###

* Visual Studio 2010
http://www.microsoft.com/visualstudio/en-us/

* DirectX SDK (latest version)
http://msdn.microsoft.com/en-us/directx/aa937788.aspx


### Configuring Visual Studio ###

You need to add the locations of the DirectX SDK directories to the settings in Visual Studio.
This is usually already done automatically by the DirectX SDK installer.

1) In the Visual Studio menu, go to: Tools -> Options -> Projects and Solutions -> VC++ Directories
2) Add the location of the Include directory of the latest DirectX SDK.
3) Repeat the step 2 for the x64 platform.
4) Add the location of the Lib\x86 directory of the latest DirectX SDK to the Win32 platform.
5) Add the location of the Lib\x64 directory of the latest DirectX SDK to the x64 platform.
 
 
### Windows 7 SDK ###

With VS2010 Windows SDK isn't required. Just make sure that 7.0A version is used by default.