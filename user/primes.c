#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
prime(int rd)
{
    int num, cnum;
    if(read(rd, &num, 4) != 4){
        fprintf(2, "Cann't read from pipe\n");
        exit(1);
    }
    printf("prime %d\n", num);

    int cfds[2];
    pipe(cfds);
    if(fork() == 0){
        close(cfds[1]);
        prime(cfds[0]);
        close(cfds[0]);
    }else{
        close(cfds[0]);
        while(read(rd, &cnum, 4)){
            if(cnum % num != 0){
                if(write(cfds[1], &cnum, 4) != 4){
                    fprintf(2, "Cann't write to pipe\n");
                    exit(1);
                }
            }
        }
        close(rd);
        close(cfds[1]);
        wait(0);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    if(argc != 1){
        fprintf(2, "Usage: primes\n");
        exit(1);
    }

    int fds[2];
    pipe(fds);

    int pid = fork();
    if(pid != 0){
        close(fds[0]);
        for(int i = 2; i <= 35; i++){
            if(write(fds[1], &i, 4) != 4){
                fprintf(2, "Cann't write to pipe\n");
                exit(1);
            }
        }
        close(fds[1]);
        wait(0);
    }else{
        close(fds[1]);
        prime(fds[0]);
        close(fds[0]);
    }
    exit(0);
}
