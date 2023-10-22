#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#define MAXLEN 100

int
main(int argc, char *argv[])
{
    if(argc < 2){
        fprintf(2, "Usage: xargs [command]\n");
        exit(1);
    }
    // Get the command
    char *cmd = argv[1];
    char buf;
    char nargv[MAXARG][MAXLEN];
    char *pnargv[MAXARG];
    // Loop line
    while(1){
        memset(nargv, 0, MAXARG*MAXLEN);
        // Copy the origin params to the new ram
        for(int i = 1; i < argc; i++) strcpy(nargv[i-1], argv[i]);
        int cargc = argc - 1;
        int offset = 0;
        int is_read = 0;
        // Get all params of one line
        while((is_read = read(0, &buf, 1)) > 0){
            if(buf == ' '){
                cargc++;
                offset = 0;
                continue;
            }
            if(buf == '\n'){
                break;
            }
            if(offset == MAXLEN){
                fprintf(2, "xargs: parameter too long\n");
                exit(1);
            }
            if(cargc == MAXARG){
                fprintf(2, "xargs: too many arguments\n");
                exit(1);
            }
            nargv[cargc][offset++] = buf;
        }
        if(is_read <= 0) break;
        // Loop every params
        for(int i = 0; i <= cargc; i++){
            pnargv[i] = nargv[i];
        }
        if(fork() == 0){
            exec(cmd, pnargv);
            exit(1);
        }
        wait(0);
    }
    exit(0);
}