#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int generate();
int primes(int p, int n);

// Function to generate numbers from 2 to 280 and write to pipe
int generate() { 
    int p[2]; 

    if (-1 == pipe(p)) {
        printf("Can't initialize pipe\n");
        exit(1);
    }

    int pid = fork();

    if (pid == 0) { // Child process
        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, sizeof(i)); // write 2 to 280 to child process
        } 

        close(p[1]); // child close write end of pipe
        exit(0); // child terminates
    }

    close(p[1]); // parent close write end of pipe

    return p[0]; // return the read end of pipe
}

int primes(int p, int n) {
    int num;
    int rp[2]; // pipe for filtered numbers

    if (-1 == pipe(rp)) {
        printf("Can't initialize pipe\n");
        exit(1);
    }

    int pid = fork();

    if (pid == 0) { // Child process
        while (read(p, &num, sizeof(num))) { // read number from p and filter out multiples of n
            if (num % n != 0) { // if num is not a multiple of n, store num in rp
                write(rp[1], &num, sizeof(num)); 
            }
        }
        close(rp[1]); // child close write end of pipe
        close(p); // close the read end of the old pipe

        exit(0); // child terminates
    }

    close(p); // parent close read end of the old pipe
    close(rp[1]); // parent close write end of pipe

    return rp[0]; // return the read end of pipe
}

// This implementation ensures that the primary pipe (used in generate()) writes
// all the number in range 2 - 280 to the child process before closing the write end.
// Therefore if the first process writes up to 280, the last process still gets all 280 numbers.
int main(int argc, char* argv[]) {
    int prime; 
	
	int in = generate();
	while (read(in, &prime, sizeof(int))) {
		printf("prime %d\n", prime); 
		in = primes(in, prime);
	}

	exit(0);
}