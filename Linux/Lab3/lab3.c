#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

#define COMMAND_AMOUNT 5

int main()
{
    char* tab[] = {
        "wget https://hil-speed.hetzner.com/100MB.bin",
        "ping -c 10 stos.eti.pg.edu.pl",
        "sleep 20 && echo koniec spania",
        "touch file.txt",
        "ip a"
    };
    for(int i=0; i<COMMAND_AMOUNT; i++ ){
        int pid = fork();
        if(pid == 0){
            execlp ("sh","sh", "-c" , tab[i], NULL);
        }
    }
    while (wait(NULL) > 0);
    printf("Done \n");
    system("rm file.txt");
    system("rm 100MB.bin");
    return 0;
}