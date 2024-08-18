# ORigin Game Engine

Clone the repository recursively: <br> 
`git clone --recursive https://github.com/evangelionmanuhutu/ORigin-Engine.git`. <br>

## Build on Windows
#### Please Install `pip install requests`
Required [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) and [Python3](https://www.python.org/downloads/).<br>
Run [gen.bat](gen.bat) to generate Visual Studio .sln file or Makefile.

## Build on Linux
Install: <br>
`sudo apt install libwayland-dev libxkbcommon-dev xorg-dev`
Install Mono
[Mono Project](https://www.mono-project.com/download/stable/#download-lin)<br>
`sudo apt install build-essential mono-complete mono-devel mono-dbg libicu-dev`<br>
``<br>
Run `bash gen.sh` to generate Makefile.
