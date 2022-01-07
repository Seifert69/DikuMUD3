# Building with cmake from command line

Special thanks to @bluedreamer for modernizing our Makefiles :+1

Pre-requisites:
   * cmake
   * optional ninja build system (Fedora: `sudo dnf install ninja-build`)

### cmake
    Ubuntu: sudo apt-get install cmake
    Centos 7: sudo yum install epel-release; sudo yum install cmake3
    Fedora/Centos 8/9: sudo dnf install cmake

### ninja
    Ubuntu: sudo apt-get insta@ll ninja-build
    Centos 7: not available in official repos - requires manual install
    Fedora/Centos 8/9: dnf install ninja-build

### flex-devel (Centos >=8)
    dnf config-manager --set-enabled powertools
    dnf install flex-devel

### Ninja

The examples below will use the ninja build system but if you want to use gnu make change `-G Ninja` to `-G "Unix Makefiles"` (or most linunx system default to `Unix Makefiles` so you could probably leave the option off) in the below commands. Then rather than executing `ninja` run `make` instead.

Ninja is a lot like make - so you can build specific targets eg `ninja pp`. The equivalent of `make all` is just `ninja` as everything is the default.

#### Tips

* Ninja uses all available cores when compiling and you may not have the ram to do that. You can limit cores just like you can with make `ninja -j 4`

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
