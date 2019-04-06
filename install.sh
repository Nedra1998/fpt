#!/bin/bash

git clone https://github.com/ardenrasmussen/fpt .fpt
cd .fpt
cmake -DCMAKE_INSTALL_PREFIX=./install .
sudo make install
