# poomer-ftxui-modal_select
Example prototype demonstrating ftxui modal interface

This program is a simple example of how to use the ftxui library to create a modal dialog.
- It allows the user to select an address from a list of addresses in a modal dialog.
- It allows the user to select a file from a list of files in a modal dialog.
- It allows the user to enter an address manually.
- It creates a JSON file to store the addresses.

FTXUI is a library that allows you to create text-based user interfaces.
It is a lightweight library that is easy to use and understand.
It is a modern C++ library that provides a functional approach to building terminal applications.

ftxui is 

Build

Build https://github.com/ArthurSonzogni/FTXUI relative to this repo

```
g++ poomer-fxtui-modal_select.cpp -o poomer-fxtui-modal_select -std=c++17 -I../FTXUI/include/ -L../FTXUI/build -lftxui-component -lftxui-dom -lftxui-screen -I.
```