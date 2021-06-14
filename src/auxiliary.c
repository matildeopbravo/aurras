#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 1024

ssize_t readch(int fd, char* buf) {
    static char file_buffer[BUF_SIZE];
    static int next_position = 0;
    static int last_read_bytes = 0;
    int bytes_read = 0;

    if (next_position == last_read_bytes) {
        last_read_bytes = read(fd, file_buffer, BUF_SIZE);
        next_position = 0;
    }
    if (last_read_bytes > 0) {
        *buf = file_buffer[next_position++];
        bytes_read = 1;
    }
    return bytes_read;
}
ssize_t readln(int fd, char* line, size_t size) {
    int next_pos = 0;
    int r = 0;
    while (next_pos < size && (r = readch(fd, line + next_pos)) == 1) {
        if (line[next_pos++] == '\n') break;
    }
    return r <= 0 ? 0 : next_pos;
}
ssize_t read_file(char* filename) {
    int fd = open(filename, O_RDONLY);
    char line[BUF_SIZE];
    int total_read = 0, len = 0;
    while ((len = readln(fd, line, BUF_SIZE)) > 0) {
        total_read += len;
        write(STDOUT_FILENO, line, len);
    }
    return total_read;
}

