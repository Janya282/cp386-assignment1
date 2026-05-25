#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ftw.h>

void create_directory(const char *dir_name, mode_t mode) {
    if (mkdir(dir_name, mode) == 0) {
        printf("The directory %s is created successfully with mode %o\n", dir_name, mode);
    } else {
        perror("mkdir failed");
    }
}

void create_write_file(const char *file_name, const char *content) {
    FILE *fp = fopen(file_name, "w");

    if (fp == NULL) {
        perror("File creation failed");
        return;
    }

    fprintf(fp, "%s", content);
    fclose(fp);

    printf("File %s is created and written successfully.\n", file_name);
}

void read_proc_mem_info_to_file(const char *file_name) {
    FILE *src = fopen("/proc/meminfo", "r");
    if (src == NULL) {
        perror("Failed to open /proc/meminfo");
        return;
    }

    FILE *dst = fopen(file_name, "w");
    if (dst == NULL) {
        perror("Failed to create output file");
        fclose(src);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), src) != NULL) {
        fprintf(dst, "%s", line);
    }

    fclose(src);
    fclose(dst);

    printf("The file name id %s\nProc info written to %s successfully.\n", file_name, file_name);
}

void directory_listing(const char *start_dir) {
    DIR *dir;
    struct dirent *entry;
    char path[1024];

    dir = opendir(start_dir);
    if (dir == NULL) {
        perror("opendir failed");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);

        snprintf(path, sizeof(path), "%s/%s", start_dir, entry->d_name);

        struct stat path_stat;
        if (stat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
        
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
        
            directory_listing(path);
        }
    }

    closedir(dir);
}

static int remove_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    (void)sb;
    (void)typeflag;
    (void)ftwbuf;
    int rv = remove(fpath);
    if (rv != 0)
        perror(fpath);
    return rv;
}

void remove_directory(const char *dir_name) {
    char confirm;
    printf("Warning: You are about to delete the directory %s and all its contents recursively. Proceed? (y/n) ", dir_name);
    scanf(" %c", &confirm);

    if (confirm == 'y' || confirm == 'Y') {
        if (nftw(dir_name, remove_callback, 64, FTW_DEPTH | FTW_PHYS) == 0) {
            printf("Directory %s removed successfully.\n", dir_name);
        } else {
            perror("Failed to remove directory");
        }
    } else {
        printf("Operation cancelled.\n");
    }
}

int main() {
    int choice;
    char dir_name[256];
    char file_name[256];
    char content[1024];
    unsigned int mode;

    while (1) {
        printf("\n----- Menu -----\n");
        printf("1: Create a directory\n");
        printf("2: Create and write to a file\n");
        printf("3: Write contents of /proc/meminfo to a file\n");
        printf("4: List directory contents\n");
        printf("5: Remove a directory\n");
        printf("99: Quit\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);

        if (choice == 99) {
            break;
        }

        switch (choice) {
            case 1:
                printf("Enter the directory name you want to create: ");
                scanf("%s", dir_name);
                printf("Enter the mode of the created directory (in octal, e.g., 0755): ");
                scanf("%o", &mode);
                create_directory(dir_name, (mode_t)mode);
                break;

            case 2:
                printf("Enter the file name you want to change to: ");
                scanf("%s", file_name);
                printf("Enter the contents that you want to write to the file: ");
                getchar();
                fgets(content, sizeof(content), stdin);
                content[strcspn(content, "\n")] = '\0';
                create_write_file(file_name, content);
                break;

            case 3:
                printf("Enter the file name you want to store memory information to: ");
                scanf("%s", file_name);
                read_proc_mem_info_to_file(file_name);
                break;

            case 4:
                printf("Enter the directory name you want to list the contents for (if you want to list the contents of the current directory, then pass '.'): ");
                scanf("%s", dir_name);
                directory_listing(dir_name);
                break;

            case 5:
                printf("Enter the directory name you want to remove: ");
                scanf("%s", dir_name);
                remove_directory(dir_name);
                break;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}
