# poomer-ftxui-modal_select
c++ mockup demonstrating ftxui modal interface

This program is a simple example of how to use the ftxui library to create a modal dialog.
- select an address from a list of addresses in a modal dialog.
- select a file from a list of files in a modal dialog.
- enter an address manually.
- creates a JSON file to store the addresses with nlohmann/json.hpp
- FXTUI links statically

FTXUI is a library that allows you to create text-based user interfaces.
It is a lightweight library that is easy to use and understand.
It is a modern C++ library that provides a functional approach to building terminal applications.

# Build

```
workdir/
├── FXTUI/
├── poomer-fxtui-modal_select/
```

## MacOS

```
mkdir workdir
cd workdir
git clone https://github.com/ArthurSonzogni/FTXUI
cd FXTUI
mkdir build
cd build
/Applications/CMake.app/Contents/bin/cmake -DFTXUI_BUILD_EXAMPLES=ON ..
make
cd ../..
git clone https://github.com/oomer/poomer-fxtui-modal_select.git
cd poomer-fxtui-modal_select
g++ poomer-ftxui-modal_select.cpp -o poomer-ftxui-modal_select -std=c++17 -I../FTXUI/include/ -L../FTXUI/build -lftxui-component -lftxui-dom -lftxui-screen -I.
```

## Linux

```
mkdir workdir
cd workdir
git clone https://github.com/ArthurSonzogni/FTXUI
cd FXTUI
mkdir build
cd build
cmake ..
[OPTIONAL] cmake -DFTXUI_BUILD_EXAMPLES=ON ..
make
cd ../..
git clone https://github.com/oomer/poomer-fxtui-modal_select.git
cd poomer-fxtui-modal_select
g++ poomer-ftxui-modal_select.cpp -o poomer-ftxui-modal_select -std=c++17 -I../FTXUI/include/ -L../FTXUI/build -lftxui-component -lftxui-dom -lftxui-screen -I.
```

## Windows

launch x64 Native tools Command Prompt for VS2022


```
mkdir workdir
cd workdir
git clone https://github.com/ArthurSonzogni/FTXUI
cd FXTUI
mkdir build
cd build
cmake ..
[OPTIONAL] cmake -DFTXUI_BUILD_EXAMPLES=ON ..
cmake --build . --config Release
[OTHER] msbuild ftxui.sln /p:Configuration=release /p:Platform=x64 /p:PlatformToolset=v143
cd ../..
git clone https://github.com/oomer/poomer-fxtui-modal_select.git
cd poomer-fxtui-modal_select
cl /MD /std:c++17 poomer-ftxui-modal_select.cpp /Fe:poomer-ftxui-modal_select.exe /I..\FTXUI\include\ /I. /link ..\FTXUI\build\Release\ftxui-component.lib ..\FTXUI\build\Release\ftxui-screen.lib ..\FTXUI\build\Release\ftxui-dom.lib
```

