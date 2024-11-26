# Summary

- [Getting Started](#Getting-Started)
- [Install](#Install)
  - [WSL2](#WSL2)
  - [ESP-IDF](#ESP-IDF)
  - [CppUTest](#CppUTest)

# Getting Started

This repository was made using windows and WSL2 ubuntu. You can build, flash and monitor using windows, but the tests must be made with a linux OS. I used WSL2 with ubuntu to build and to run tests on each example.


# Install

As I said, these repository needs some programs and libraries to run. There is a tutorial for each type of them here.

## WSL2 

WSL2 means Windows Subsystem Linux that allows a linux work inside the Windows and also allows to windows programs have access to linux directories at the same time that the linux subsystem can also access Windows folders.

It works only in Windows 10 and Windows 11.

### Pre-install
For the install, it's necessary make your windows ready before.

 - First open the Windows Features. You can open it seaching on windows menu.
 - Check the itens:
   - Virtual Machine Platform
   - Windows Subsystem for linux
 - Restart your windows

 ### Install Linux Kernel
 - On Windows 11 you can install WSL2 by microsoft store. But there is a link to download it in windows 10 or 11: https://wslstorestorage.blob.core.windows.net/wslblob/wsl_update_x64.msi
 - Execute this installer, select _next_ until finish
 - Restart your windows

 ###  Define WSL2 as default
 - Open your CMD prompt or Windows Powershell as ADMIN
 - Run the command:
 `wsl --set-default-version 2`

 ### Chose your Linux Distribution
 - Open your microsoft store and download your linux distribution like:
   - Ubuntu
   - Debian
- Install one of them

### Hello Linux

- Execute your linux distribution
- Choose your linux user and your password. 
- Run these two commands and wait them finish:
  - `sudo apt-get update`
  - `sudo apt-get upgrade`

### VS Code Extension

- You can use the extension WSL on VS code to access your linux files and directories.

### Tips on WSL

- You can open Windows Explorer, chose a folder and then shift + right-click and select open with linux shell here
- You can navigate on directories using linux terminal and open it on VS Code using the command `code .` or open it on windows explorer using the command `explorer.exe .` (change the '.' by the path to the directory that you want to open)

## ESP-IDF

This tutorial will tech to install the esp-idf v4.4.4 on linux. You can adapt it for other version if you want. If you want to install it on windows, I have a tutorial in portuguese that might help: https://drive.google.com/drive/folders/1rDEi_ZVZpv8i6to1JckakVsTsIdx5W53?usp=sharing

On linux you can follow these steps:

- Open your linux terminal
- Choose your directory that you will install esp-idf (I use the home with the command `cd ~`)
- Clone ESP-IDF on esp directory
`git clone -b v4.4.4 --recursive https://github.com/espressif/esp-idf.git esp
`
- Enter on esp directory: `cd ~/esp`
- Execute the install script: `./install.sh`
- Config ESP-IDF enviroment: `. ~/esp/export.sh`

`cd ~/esp`

`git clone --branch v4.4.4 --single-branch https://github.com/espressif/esp-idf.git`



## CppUTest

Using linux or WSL2, you can install cppUtest easily.

- Open your linux terminal
- Run the command: `sudo apt-get install cpputest`

# Project

Each numbered project directory has some files and folders. They usually follow a pattern. 

Select a project and enter it. It should follow this structure:

|file| description |
|-|-|
|./CMakeLists.txt| Defines your project and references ESP-IDF|
|./sdkconfig| Config file to personalize drivers and hardware config of ESP-IDF framework in your project|
|./ main| Directory with your main files |
| ./main/main.c| Your main file, your firmware application will start in app_main function there|
| ./main/CMakeLists.txt| Register each c file on main directory. It also allows some stacks linked by there c files registered|
| ./test| your test directory with your test cases|
| ./test/CMakeLists.txt| Defines your test and references ESP-IDF, CppUTests, source directory, components and test files|
| ./components| A directory to organize and reutilize your code|

# Config

Before you start building or testing these examples, you should fix the reference of ESP-IDF on your computer.

I installed ESP-IDF in my ~home/[user]/ directory, so you should change my username in [user] to your linux username if you followed the same steps.

If not, just adapt the IDF_PATH to your esp-idf directory that you have installed.

Do it in ./CMakeLists.txt and ./test/CMakeLists.txt on this line:
`set(IDF_PATH "/home/[USERNAME]/esp/esp-idf")`

# Build

Inside of one example project directory, you can open the linux terminal and run the command `idf.py build`.

It should build the project if there is no syntax errors or linking errors. 

The first build take a while, if you don't do a lot of changes on the project, delete the build files in build directory, or edit the sdkconfig it will be faster next time.

If you builded your project or windows and then try it on linux, you should delete the build directory and then do the build process again. The same happens from linux to windows.
