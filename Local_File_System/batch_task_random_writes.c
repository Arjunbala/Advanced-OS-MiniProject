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

#define SEEK_ITERATIONS 10000

struct write_stats {
    unsigned long long bytesWritten;
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
off_t generateRandomNumber(unsigned long long max) {
    unsigned long long no_write_blocks = max/WRITE_SIZE;
    int selected_block = rand()%no_write_blocks;
    return selected_block*WRITE_SIZE;
}

void updateStats(struct write_stats *stats, unsigned long long bytes_written, double time_taken) {
    stats->bytesWritten += bytes_written;
    stats->timeTaken += time_taken;
}

void printStats(const struct write_stats *stats) {
    double bandwidth = (double) stats->bytesWritten/(1024*1024*stats->timeTaken);
    printf("Total data written = %lld bytes\n", stats->bytesWritten);
    printf("Total time taken = %lf seconds\n", stats->timeTaken);
    printf("Avg. Disk Bandwidth = %lf MBps\n", bandwidth);
}

int main() {
    printf(".. Starting with file size for buffered = %lld bytes ..\n", getFileSizeForBuffered());
    printf(".. Starting with file size for direct = %lld bytes ..\n", getFileSizeForDirect());

    int fileSize = getFileSizeForBuffered();
    struct write_stats buffered_stats = {0,0.0};
    struct write_stats direct_stats = {0,0.0};

    static char buffer[WRITE_SIZE] __attribute__ ((__aligned__ (WRITE_SIZE)));
    memset(buffer, 'a', sizeof(buffer)); // Filling with all 'a's works as random for our purpose
    buffer[WRITE_SIZE-1] = '\0';
    int fd_buffered = open("temp_big_letters_buffered.txt", O_WRONLY);
    if(fd_buffered < 0) {
        printf("Error in opening file temp_big_letters_buffered.txt err=%d(%s)\n", errno, strerror(errno));
    }
    int fd_direct = open("temp_big_letters_direct.txt", O_WRONLY | O_DIRECT | O_SYNC);
    if(fd_direct < 0) {
        printf("Error in opening file temp_big_letters_direct.txt err=%d(%s)\n", errno, strerror(errno));
    }

    for(int i=0;i<SEEK_ITERATIONS;i++) {
        off_t offset = generateRandomNumber(fileSize-WRITE_SIZE);
        if(lseek(fd_buffered, offset, SEEK_SET) < 1) {
		printf("Error in seeking buffered file err=%d(%s)\n", errno, strerror(errno));
	}
	if(lseek(fd_direct, offset, SEEK_SET) < 1) {
                printf("Error in seeking buffered file err=%d(%s)\n", errno, strerror(errno));
        }
        clock_t start, end;

	start = clock();
	ssize_t written_buffered = write(fd_buffered, buffer, sizeof(buffer));
        if(written_buffered < 0) {
            printf("write error buffered. status=%s(%d)\n", strerror(errno), errno);
            exit(0);
        }
	end = clock();
	double time_taken_buffered = ((double) (end - start))/CLOCKS_PER_SEC;
	updateStats(&buffered_stats, written_buffered, time_taken_buffered);

        start = clock();
        ssize_t written_direct = write(fd_direct, buffer, sizeof(buffer));
        if(written_direct < 0) {
            printf("write error direct. status=%s(%d)\n", strerror(errno), errno);
            exit(0);
        }
        end = clock();
        double time_taken_direct = ((double) (end - start))/CLOCKS_PER_SEC;
        updateStats(&direct_stats, written_direct, time_taken_direct);
    }

    printf("---- Buffered Stats ----\n");
    printStats(&buffered_stats);
    printf("\n\n");

    printf("---- Direct Stats ----\n");
    printStats(&direct_stats);
    printf("\n\n");

}
