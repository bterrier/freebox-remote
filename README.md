# Freebox Remote
## Introduction
This is a draft project aiming to develop a multiplatform remote controller for the Freebox Player (media box provided by the French ISP Free).

## License
This project is published under the CeCILL license.

The CeCILL license is a copyleft license similar to, and compatible with, the GNU GPL.

This project contains and uses code from `libela`, `librudp` and `foils_hid` which are published by *Freebox* SA under a 2-clause BSD license.

This projects uses the Qt framework published under the GNU GPLv3.

## Content
- `libelaqt`: implementation of a `libela` backend for Qt.
- `remote-cli`: a cli Freebox remote. Based on the test remote from `foils_hid`, but using the Qt backend for `libela`.
- `remote`: a graphical Freebox remote made with Qt.

## Building
1. Get submodules
    ```bash
    git submodule update -i
    ```

1. Build 3rd party libraries (libela, librudp and foild_hid). This only need to be done once, or when on the the submodule has been updated. This will create a folder `build` where the artifacts of the 3rd party libraries will be installed.
    ```bash
    ./build.sh
    ```

1. Build using cmake
    ```bash
    mkdir mybuild
    cd mybuild
    cmake ..
    cmake --build .
    ```