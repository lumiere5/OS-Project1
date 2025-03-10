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
	int blanks = 0;

	while (read(0, &c, 1) > 0) {
		if (c == ' ' || c == '\t') {
			blanks++;
			continue;
		}

		if (blanks) {
			buffer[offset++] = 0;
			xargvs[count++] = p;
			p = buffer + offset;
			blanks = 0;
		}

		if (c != '\n') {
			buffer[offset++] = c;
		}
		else {
			xargvs[count++] = p;
			p = buffer + offset;

			if (!fork()) {
				exit(exec(argv[1], xargvs));
			}

			wait(0);

			count = argc - 1;
		}
	}

	exit(0);
}
