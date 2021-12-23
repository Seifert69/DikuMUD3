# Building with cmake from command line

Pre-requisites:
   * cmake
   * optional ninja build system (Fedora: `sudo dnf install ninja-build`)

The examples below will use the ninja build system but if you want to use gnu make change `-G Ninja` to `-G "Unix Makefiles"` in the below commands. Then rather than executing `ninja` run `make` instead.

### Ninja
Ninja is a lot like make - so you can build specific targets eg `ninja pp`. The equivalent of `make all` is just `ninja` as everything is the default.

## Building Binaries
***Note this replaces Step 1 portion of README.md***

All these are run from top level of project (eg DikiMud3) 

#### List of built-in build types
* Debug
* Release
* RelWithDebInfo
* MinSizeRel

### Build Debug version
From top level of project

    mkdir cmake-build-debug
    cd cmake-build-debug
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
    ninja

### Build Release version
From top level of project

    mkdir cmake-build-release
    cd cmake-build-release
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
    ninja
