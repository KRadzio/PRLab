#!/bin/bash
gcc -c -Wall -fPIC -D_GNU_SOURCE factorial.c
gcc -c -Wall -fPIC -D_GNU_SOURCE fibonacci.c
gcc -c -Wall -fPIC -D_GNU_SOURCE findMax.c
gcc -c -Wall -fPIC -D_GNU_SOURCE isPrime.c
gcc -c -Wall -fPIC -D_GNU_SOURCE nwd.c
gcc -c -Wall -fPIC -D_GNU_SOURCE nww.c

gcc factorial.o fibonacci.o findMax.o isPrime.o nwd.o nww.o -shared -o lib_lab.so

export LD_LIBRARY_PATH="."

gcc -c main.c -o progD.o
gcc progD.o -L. -l_lab -o progD
ldd progD
./progD # uruchamiany w skrypcie aby nie musieć za każdym razem robić exporta

rm factorial.o
rm fibonacci.o
rm findMax.o
rm isPrime.o
rm nwd.o
rm nww.o
rm progD.o
