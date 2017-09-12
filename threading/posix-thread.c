#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

struct int_t{
	int i;
};

void* fib(void *n){
	int i;
	int64_t last1 = 1, last2 = 0, buf;
	for(i = 1; i < ((struct int_t*)n)->i; i++){
		buf = last1 + last2;
		last2 = last1;
		last1 = buf;
	}
	printf("fib(%d) = %ld\n", ((struct int_t*)n)->i, buf);
	return NULL;
}

int main(void){
	pthread_t thrs[4];
	int i, values[] = {60, 54, 50, 45};
	struct int_t args[4];
	int rets[4];
	for(i = 0; i < 4; i++){
		args[i].i = values[i];
		rets[i] = pthread_create(&thrs[i], NULL, fib, (void*) &args[i]);
		if(rets[i]){
			fprintf(stderr, "Error while opening thread %d - error #%d", i, rets[i]);
		}
	}
	for(i = 0; i < 4; i++){
		pthread_join(thrs[i], NULL);
	}
	return 0;
}
