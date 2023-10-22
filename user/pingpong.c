#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]){
	int fds[2];
	char buf[2];
	char *msg = "o";
	pipe(fds);

	int pid = fork();
	if(pid == 0){
		if(read(fds[0], buf, 1) != 1){
			fprintf(2, "Can't read from parent\n");
			exit(1);
		}
		close(fds[0]);
		printf("%d: received ping\n", getpid());

		if(write(fds[1], msg, 1) != 1){
			fprintf(2, "Can't write to parent\n");
			exit(1);
		}
		close(fds[1]);
		exit(0);

	}else{
		if(write(fds[1], msg, 1) != 1){
			fprintf(2, "Can't write to child\n");
			exit(1);
		}
		close(fds[1]);
		wait(0);

		if(read(fds[0], buf, 1) != 1){
			fprintf(2, "Can't read from child\n");
			exit(1);
		}
		close(fds[0]);
		printf("%d: received pong\n", getpid());
		exit(0);
	}

}
