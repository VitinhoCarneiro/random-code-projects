#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int main(void){
	pid_t proc = fork();
	if (proc == 0){
		usleep(10000);
		printf("[Loli process]: Onii-chan, kawaii desu ne!\n");
		return 0;
	}
	else if (proc < 0){
		printf("[Main process]: Onee-san, doko ni imasu ka? :(\n");
		usleep(50000);
		return 1;
	}
	else{
		printf("[Main process]: Onee-san, watashi wa kirei desu ka?\n");
		usleep(50000);
		return 0;
	}
}
