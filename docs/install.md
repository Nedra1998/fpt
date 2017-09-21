# Installation #

Installing the FPT library requires the proper version of SDL2 to be installed
on the system prior to attempting the installation of FPT.


<!-- vim-markdown-toc GFM -->
* [Dependencies](#dependencies)
  * [SDL](#sdl)
    * [Linux](#linux)
      * [Ubuntu/Debian](#ubuntudebian)
      * [Fedora/CentOS](#fedoracentos)
    * [Mac Os X](#mac-os-x)
    * [Source](#source)
* [FPT](#fpt)
  * [Source](#source-1)

<!-- vim-markdown-toc -->

## Dependencies ##

### SDL ###

This version of the FPT library depends on SDL2, and SDL2_ttf. These SDL
libraries can be installed as shown below for the different systems.

#### Linux ####

##### Ubuntu/Debian #####

```fish
sudo apt-get install libsdl2 libsdl2-dev
```

##### Fedora/CentOS #####

```fish
yum install SDL2-devel
```


#### Mac Os X ####

First download the SDL OS X development libraries from the [SDL
website](https://www.libsdl.org/download-2.0.php#source).

Next open the `dmg` and copy the `SDL2.framework` to `/Library/Frameworks`.

The framework may need to resigned. To sign the framework, open up a terminal
to:

```fish
/Library/Frameworks/SDL2.framework/
```

And sign the framework using the command:

```fish
codesign -f -s - SDL2
```

#### Source ####

The SDL2 source can be downloaded from the [SDL
website](https://www.libsdl.org/download-2.0.php#source).

This can then be compiled and installed.

```fish
./configure
make all
make install
```

## FPT ##

It is recomended to install FPT from the source.

### Source ###

Download the source from the [Github
page](https://github.com/ardenrasmussen/fpt). Be sure to download from the SDL
branch.

Then extract the zip, and install the files.

It can also be download it from the command line with git clone.

```fish
git clone https://github.com/ardenrasmussen/fpt
git checkout sdl
```

To compile and install the FPT library simply run

```fish
make install
```

This will build and install all the files needed for the FPT library.
