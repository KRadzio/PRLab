#!/bin/bash

gcc -c factorial.c -o factorial.o
gcc -c fibonacci.c -o fibonacci.o
gcc -c findMax.c -o findMax.o
gcc -c isPrime.c -o isPrime.o
gcc -c nwd.c -o nwd.o
gcc -c nww.c -o nww.o

ar r lib_lab2.a factorial.o fibonacci.o findMax.o isPrime.o nwd.o nww.o
#ar tv lib_lab2.a

gcc -c main.c -o prog.o

gcc prog.o lib_lab2.a -o prog
./prog

rm factorial.o
rm fibonacci.o
rm findMax.o
rm isPrime.o
rm nwd.o
rm nww.o
rm prog.o
