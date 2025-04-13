#!/bin/bash

ARRAY=("wget https://hil-speed.hetzner.com/100MB.bin" "ping -c 10 stos.eti.pg.edu.pl" "sleep 20 && echo koniec spania" "touch file.txt" "ip a")

function main(){
for item in "${ARRAY[@]}"; do
    eval "$item" &
done

#czekanie na zakończenie programu
wait
echo "Done"

# usuwanie
rm file.txt
rm 100MB.bin
}
main