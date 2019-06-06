#!/bin/bash

OS="$OSTYPE"
HAS_GIT=$([ $(command -v git) ] && echo "1" || echo "0")
HAS_CMAKE=$([ $(command -v cmake) ] && echo "1" || echo "0")
# HAS_SDL=$([ ! -f /usr/lib/libSDL.so ] && echo "1" || echo "0")

if [[ "$OS" =~ darwin.* ]]; then
  HAS_BREW=$(command -v brew)
  if [[ -z "$HAS_BREW" ]]; then
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  fi
  HAS_XQUARTZ=$(brew ls --versions xquartz)
  if [[ -z "$HAS_XQUARTZ" ]]; then
    brew cask install xquartz
  fi
  if [[ $HAS_GIT == 0 ]]; then
    brew install git
  fi
  if [[ $HAS_CMAKE == 0 ]]; then
    brew install cmake
  fi
elif [[ "$OS" == "linux-gnu" ]]; then
  DISTRO=$(hostnamectl | grep "Operating System" | sed 's/[^O]*Operating System: *\(.*\)$/\1/g')
  if [[ "$DISTRO" =~ Arch.* ]]; then
    PAC=$([ $(command -v yay) ] && echo "yay" || echo "sudo pacman")
    if [[ $HAS_GIT == 0 ]]; then
      $PAC -S git
    fi
    if [[ $HAS_CMAKE == 0 ]]; then
      $PAC -S cmake
    fi
  elif [[ "$DISTRO" =~ Ubuntu.* ]]; then
    if [[ $HAS_GIT == 0 ]]; then
      sudo apt-get install git
    fi
    if [[ $HAS_CMAKE == 0 ]]; then
      sudo apt-get install cmake
    fi
  fi
fi

# git clone https://github.com/ardenrasmussen/fpt .fpt && cd .fpt && cmake . && make && cd ../ && rm .fpt -rf
# cd .fpt
# cmake .
# sudo make install
# cd ../
# rm .fpt -rf
