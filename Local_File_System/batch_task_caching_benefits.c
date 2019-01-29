#define _GNU_SOURCE
#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#include<unistd.h>

#define CLI_ARG_COUNT 1 // Number of Command line arguments
#define DIRECT_IO_SPEC_POS 1 // Position of direct i/o spec in CL arguments

#define RANDOM_STRING_SIZE 1024
#define INPUT_FILE_SIZE_KB 1024*10 //10Mb

int modify_file_permissions(char *fileName, char *mode) {
    int i;
    i = strtol(mode, 0, 8);
    if (chmod (fileName,i) < 0) {
        printf("error in chmod(%s, %s) - %d (%s)\n",
                fileName, mode, errno, strerror(errno));
    }
}

void drop_cache() {
    sync();
    int fd;
    char *data = "3";
    fd = open("/proc/sys/vm/drop_caches", O_WRONLY);
    int size = write(fd, data, sizeof(data));
    if (size <= 0) {
        printf("Write error - %d(%s)\n", errno, strerror(errno));
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc != CLI_ARG_COUNT + 1) {
        printf("Program accepts %d argument(s)\n", CLI_ARG_COUNT);
        exit(0);
    }

    int useDirectIO = atoi(argv[1]);
    printf("Should use Direct I/O - %d\n", useDirectIO);

    pid_t childPid = fork();    
    if(childPid == 0) {
        // This is a child process. Write to a file.
        printf("Starting work in child process\n");

        static char buffer[RANDOM_STRING_SIZE] __attribute__ ((__aligned__ (RANDOM_STRING_SIZE)));
        memset(buffer, 'a', sizeof(buffer)); // Filling with all 'a's works as random for our purpose
        buffer[RANDOM_STRING_SIZE-1] = '\0';

        clock_t start, end;
        double cpu_time_used;
        start = clock();
        int fd;
        if(useDirectIO) {
            fd = open("testfile.txt", O_RDWR | O_DIRECT | O_CREAT | O_APPEND | O_TRUNC);
        } else {
            fd = open("testfile.txt", O_WRONLY | O_APPEND | O_CREAT | O_TRUNC);
        }
        if(fd < 0) {
            printf("File open failed. status=%s\n", strerror(errno));
            exit(0);
        }

        unsigned long long totalBytes = 0;
        for(int i=0;i<INPUT_FILE_SIZE_KB;i++) {
            ssize_t written = write(fd, buffer, sizeof(buffer));
            /*if(!useDirectIO) {
                fsync(fd);
            }*/
            if(written == -1) {
                printf("write error. status=%s(%d)\n", strerror(errno), errno);
                exit(0);
            }
            totalBytes += written;
        }
        close(fd);
        end = clock();
        cpu_time_used = ((double) (end - start))/CLOCKS_PER_SEC;
        double bandwidth = (double) totalBytes/(1024*1024*cpu_time_used);
        printf("---- Writes information ----\n");
        printf("Total data written = %lld bytes\n", totalBytes);
        printf("Total time taken = %lf seconds\n", cpu_time_used);
        printf("Avg. Disk Bandwidth = %lf MBps\n", bandwidth);
        printf("\n\n");
        modify_file_permissions("testfile.txt", "777");
    } else {
        // This is parent process.
        // Wait for child process to complete and then read from file.
        int returnStatus;
        waitpid(childPid, &returnStatus, 0);
        printf("Starting work in parent process\n");
        static char buffer[RANDOM_STRING_SIZE] __attribute__ ((__aligned__ (RANDOM_STRING_SIZE)));
        int fd;
        clock_t start, end;
        double cpu_time_used;
        start = clock();
        fd = open("testfile.txt", O_RDONLY);
        unsigned long long total_read = 0;
        int n_read;
        while(1) {
            n_read = read(fd, buffer, sizeof(buffer));
            if(n_read <= 0) {
                break;
            }
            total_read = total_read + n_read;
        }
        close(fd);
        end = clock();
        cpu_time_used = ((double) (end - start))/CLOCKS_PER_SEC;
        double bandwidth = (double) total_read/(1024*1024*cpu_time_used);
        printf("---- Reads information ----\n");
        printf("Total data written = %lld bytes\n", total_read);
        printf("Total time taken = %lf seconds\n", cpu_time_used);
        printf("Avg. Disk Bandwidth = %lf MBps\n", bandwidth);
    }
}
