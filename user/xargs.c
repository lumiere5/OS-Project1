#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"  // MAXARG


int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(2, "Usage: xargs command\n");
		exit(1);
	}

	if (argc > MAXARG) {
		fprintf(2, "xargs: too many arguments\n");
		exit(1);
	}

	char buffer[1024];
	char* xargvs[MAXARG];
	int count;

	for (count = 1; count < argc; count++) {
		xargvs[count - 1] = argv[count];
	}
	--count;

	char c;
	int offset = 0;
	char* p = buffer;

	while (read(0, &c, 1) > 0) {
		if (c == ' ' || c == '\t' || c == '\n') {
			if (offset > 0) {
				buffer[offset++] = 0;
				xargvs[count++] = p;
				p = buffer + offset;
			}

			if (c == '\n') {
				xargvs[count] = 0;

				if (!fork()) {
					exec(argv[1], xargvs);
					fprintf(2, "xargs: exec %s failed\n", argv[1]);
					exit(1);
				}

				wait(0);
				count = argc - 1;
				p = buffer;
				offset = 0;
			}
			continue;
		}
		
		buffer[offset++] = c;
	}

	if (offset > 0) {
		buffer[offset++] = 0;
		xargvs[count++] = p;
		xargvs[count] = 0;

		if (fork() == 0) {
			exec(argv[1], xargvs);
			fprintf(2, "xargs: exec %s failed\n", argv[1]);
			exit(1);
		}

		wait(0);
	}

    exit(0);
}
