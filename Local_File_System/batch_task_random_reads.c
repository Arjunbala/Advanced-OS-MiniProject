#define _GNU_SOURCE
#include <errno.h>
#include<fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define READ_SIZE 512
#define SEEK_ITERATIONS 10000

struct read_stats {
    unsigned long long bytesRead;
    double timeTaken;
};

unsigned long long getFileSize(char *filename) {
    struct stat sb;
    if (stat(filename, &sb) == -1) {
        printf("Error in stat. err=%d(%s)\n", errno, strerror(errno));
	return 0;
    }
    return sb.st_size;
}

unsigned long long getFileSizeForBuffered() {
    char filename[] = "temp_big_letters_buffered.txt";
    return getFileSize(filename);
}

unsigned long long getFileSizeForDirect() {
    char filename[] = "temp_big_letters_direct.txt";
    return getFileSize(filename);
}

// Generate a random number in range [0,max)
unsigned long long generateRandomNumber(unsigned long long max) {
    unsigned long long no_read_blocks = max/READ_SIZE;
    unsigned long long selected_block = rand()%no_read_blocks;
    return selected_block*READ_SIZE;
}

void updateStats(struct read_stats *stats, unsigned long long bytes_read, double time_taken) {
    stats->bytesRead += bytes_read;
    stats->timeTaken += time_taken;
}

void printStats(const struct read_stats *stats) {
    double bandwidth = (double) stats->bytesRead/(1024*1024*stats->timeTaken);
    printf("Total data read = %lld bytes\n", stats->bytesRead);
    printf("Total time taken = %lf seconds\n", stats->timeTaken);
    printf("Avg. Disk Bandwidth = %lf MBps\n", bandwidth);
}

int main() {
    printf(".. Starting with file size for buffered = %lld bytes ..\n", getFileSizeForBuffered());
    printf(".. Starting with file size for direct = %lld bytes ..\n", getFileSizeForDirect());

    unsigned long long fileSize = getFileSizeForBuffered();
    struct read_stats buffered_stats = {0,0.0};
    struct read_stats direct_stats = {0,0.0};

    static char buffer[READ_SIZE] __attribute__ ((__aligned__ (READ_SIZE)));
    memset(buffer, 'a', sizeof(buffer)); // Filling with all 'a's works as random for our purpose
    buffer[READ_SIZE-1] = '\0';
    int fd_buffered = open("temp_big_letters_buffered.txt", O_RDONLY);
    if(fd_buffered < 0) {
        printf("Error in opening file temp_big_letters_buffered.txt err=%d(%s)\n", errno, strerror(errno));
    }
    int fd_direct = open("temp_big_letters_direct.txt", O_RDONLY | O_DIRECT);
    if(fd_direct < 0) {
        printf("Error in opening file temp_big_letters_direct.txt err=%d(%s)\n", errno, strerror(errno));
    }

    for(int i=0;i<SEEK_ITERATIONS;i++) {
        unsigned long long offset = generateRandomNumber(fileSize-READ_SIZE);
        if(lseek(fd_buffered, offset, SEEK_SET) < 1) {
		printf("Error in seeking buffered file err=%d(%s)\n", errno, strerror(errno));
	}
	if(lseek(fd_direct, offset, SEEK_SET) < 1) {
                printf("Error in seeking buffered file err=%d(%s)\n", errno, strerror(errno));
        }
        clock_t start, end;

	start = clock();
	ssize_t read_buffered = read(fd_buffered, buffer, sizeof(buffer));
        if(read_buffered < 0) {
            printf("read error buffered. status=%s(%d)\n", strerror(errno), errno);
            exit(0);
        }
	end = clock();
	double time_taken_buffered = ((double) (end - start))/CLOCKS_PER_SEC;
	updateStats(&buffered_stats, read_buffered, time_taken_buffered);

        start = clock();
        ssize_t read_direct = read(fd_direct, buffer, sizeof(buffer));
        if(read_direct < 0) {
            printf("read error direct. status=%s(%d)\n", strerror(errno), errno);
            exit(0);
        }
        end = clock();
        double time_taken_direct = ((double) (end - start))/CLOCKS_PER_SEC;
        updateStats(&direct_stats, read_direct, time_taken_direct);
    }

    printf("---- Buffered Stats ----\n");
    printStats(&buffered_stats);
    printf("\n\n");

    printf("---- Direct Stats ----\n");
    printStats(&direct_stats);
    printf("\n\n");

}
