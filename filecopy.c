#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Insufficient parameters passed.\n");
        exit(1);
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    int src = open(input_file, O_RDONLY);

    if (src < 0) {
        perror("Error opening input file");
        exit(1);
    }

    int dst = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (dst < 0) {
        perror("Error opening output file");
        close(src);
        exit(1);
    }

    char buffer[4096];
    ssize_t bytes_read;

    while ((bytes_read = read(src, buffer, sizeof(buffer))) > 0) {

        if (write(dst, buffer, bytes_read) != bytes_read) {
            perror("Error writing to output file");
            close(src);
            close(dst);
            exit(1);
        }
    }

    if (bytes_read < 0) {
        perror("Error reading input file");
    }

    close(src);
    close(dst);

    printf("The contents of file '%s' has been successfully copied into '%s' file\n",
           input_file, output_file);

    return 0;
}
