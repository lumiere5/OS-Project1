#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* getFilename(char* path) {
    if (!path || !*path) { // Check for empty string or null path
        return path; 
    }

    char* p;

    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;

    // return the pointer to the first character after the last '/'
    return p;
}

void find(char* path, char* target) {
    char buffer[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // getFilename returns the file name in the path
    // If the file name in path is the target, print it
    if (0 == strcmp(getFilename(path), target)) {
        printf("%s\n", path);
    }

    // Open the path
    fd = open(path, O_RDONLY);

    if (fd < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // Get the status of the file (fstat == 0 if successful, -1 otherwise)
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // If the file is not a directory, close the file and return
    if (st.type != T_DIR) {
        close(fd);
        return;
    }
    
    // If the path is too long, close the file and return
    if (strlen(path) + DIRSIZ + 2 > sizeof(buffer)) {
        close(fd);
        printf("find: path too long\n");
        return;
    }

    // Copy the path to the buffer
    strcpy(buffer, path);
    p = buffer + strlen(buffer);
    *p++ = '/';

    // Read directory entries
    while (sizeof(de) == read(fd, &de, sizeof(de))) {
        if (de.inum == 0) { // Skip empty entries
            continue;
        }

        // Copy the name of the directory entry to the buffer
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        // Skip "." and ".."
        if (0 == strcmp(de.name, ".") || 0 == strcmp(de.name, "..")) {
            continue;
        }

        // Recursively call find on the new path
        find(buffer, target);
    }



    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: find <path> <target>\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}
