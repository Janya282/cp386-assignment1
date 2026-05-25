#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Insufficient parameters passed.\n");
        exit(1);
    }

    const char *input_file  = argv[1];
    const char *output_file = argv[2];

    FILE *src = fopen(input_file, "r");
    if (src == NULL) {
        perror("Error opening input file");
        exit(1);
    }

    FILE *dst = fopen(output_file, "w");
    if (dst == NULL) {
        perror("Error opening output file");
        fclose(src);
        exit(1);
    }

    char buffer[4096];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            perror("Error writing to output file");
            fclose(src);
            fclose(dst);
            exit(1);
        }
    }

    if (ferror(src)) {
        perror("Error reading input file");
        fclose(src);
        fclose(dst);
        exit(1);
    }

    fclose(src);
    fclose(dst);

    printf("The contents of file '%s' has been successfully copied into '%s' file\n", input_file, output_file);

    return 0;
}