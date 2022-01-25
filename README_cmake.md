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

### Static Linking Boost Libraries

If you wish to static link boost into the binaries a compile flag option has been added - just add `-DDIKU_STATIC=1` to the cmake command.

Without the flag for example

    [user@linux DikuMUD3]$ ldd vme/bin/vme
      linux-vdso.so.1 (0x00007ffc07963000)
      libcrypt.so.2 => /lib64/libcrypt.so.2 (0x00007f93ce27d000)
      libboost_filesystem.so.1.76.0 => /lib64/libboost_filesystem.so.1.76.0 (0x00007f93ce25e000)
      libboost_regex.so.1.76.0 => /lib64/libboost_regex.so.1.76.0 (0x00007f93ce217000)
      libstdc++.so.6 => /lib64/libstdc++.so.6 (0x00007f93cdff8000)
      libm.so.6 => /lib64/libm.so.6 (0x00007f93cdf1c000)
      libgcc_s.so.1 => /lib64/libgcc_s.so.1 (0x00007f93cdf01000)
      libc.so.6 => /lib64/libc.so.6 (0x00007f93cdcf5000)
      libicudata.so.69 => /lib64/libicudata.so.69 (0x00007f93cc19a000)
      libicui18n.so.69 => /lib64/libicui18n.so.69 (0x00007f93cbe6f000)
      libicuuc.so.69 => /lib64/libicuuc.so.69 (0x00007f93cbc7b000)
      /lib64/ld-linux-x86-64.so.2 (0x00007f93ce2da000)


With the flag for example

    [user@linux DikuMUD3]$ ldd vme/bin/vme
      linux-vdso.so.1 (0x00007ffdeb289000)
      libcrypt.so.2 => /lib64/libcrypt.so.2 (0x00007f801052a000)
      libstdc++.so.6 => /lib64/libstdc++.so.6 (0x00007f801030b000)
      libm.so.6 => /lib64/libm.so.6 (0x00007f801022f000)
      libgcc_s.so.1 => /lib64/libgcc_s.so.1 (0x00007f8010214000)
      libc.so.6 => /lib64/libc.so.6 (0x00007f801000a000)
      /lib64/ld-linux-x86-64.so.2 (0x00007f8010587000)
