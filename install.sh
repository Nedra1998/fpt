#!/bin/bash

git clone https://github.com/ardenrasmussen/fpt .fpt
cd .fpt
cmake .
sudo make install
cd ../
rm .fpt -rf
