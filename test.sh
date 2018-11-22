str="Source Files"; line_length=${#str}; printf "%b%s\n" "\033[1;97m" "$str"; while [ $line_length -gt 0 ]; do printf "="; let line_length=line_length-1; done; printf "%b\n" "\033[m"
if [ -d "source" ]; then cd "source" && make; fi
printf "%b%s%b\n" "\033[0;34m" ""Source" Library" "\033[m"
mkdir -p /home/arden/Programming/c/fpt/build/source/.
printf "%b%s%b\n" "\033[0;32m" "Building /home/arden/Programming/c/fpt/build/source/./graphics.o" "\033[m"
cc -I/home/arden/Programming/c/fpt/include  -MMD -w -c  graphics.c -o /home/arden/Programming/c/fpt/build/source/./graphics.o
mkdir -p /home/arden/Programming/c/fpt/build/source/.
printf "%b%s%b\n" "\033[0;32m" "Building /home/arden/Programming/c/fpt/build/source/./input.o" "\033[m"
cc -I/home/arden/Programming/c/fpt/include  -MMD -w -c  input.c -o /home/arden/Programming/c/fpt/build/source/./input.o
printf "%b%s%b\n" "\033[0;35m" "Linking libFPT.a" "\033[m"
if [[ ! -z "/home/arden/Programming/c/fpt/build/source/./graphics.o /home/arden/Programming/c/fpt/build/source/./input.o" ]]; then ar rcs /home/arden/Programming/c/fpt/build/libFPT.a /home/arden/Programming/c/fpt/build/source/./graphics.o /home/arden/Programming/c/fpt/build/source/./input.o; fi
printf "%b%s%b\n" "\033[0;34m" ""Source" Executable" "\033[m"
mkdir -p /home/arden/Programming/c/fpt/build/source/.
printf "%b%s%b\n" "\033[0;32m" "Building /home/arden/Programming/c/fpt/build/source/./main.o" "\033[m"
cc -I/home/arden/Programming/c/fpt/include  -MMD -w -c  main.c -o /home/arden/Programming/c/fpt/build/source/./main.o
printf "%b%s%b\n" "\033[0;35m" "Linking FPT" "\033[m"
if [ -e "/home/arden/Programming/c/fpt/build/libFPT.a" ]; then cc /home/arden/Programming/c/fpt/build/source/./main.o /home/arden/Programming/c/fpt/build/libFPT.a -lSDL2 -lSDL2_ttf -lSDL2_image -lm  -o /home/arden/Programming/c/fpt/FPT; fi
if [ ! -e "/home/arden/Programming/c/fpt/build/libFPT.a" ]; then cc /home/arden/Programming/c/fpt/build/source/./main.o -lSDL2 -lSDL2_ttf -lSDL2_image -lm  -o /home/arden/Programming/c/fpt/FPT; fi

