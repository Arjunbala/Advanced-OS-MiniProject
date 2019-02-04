#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SEC_TO_NS 1000000000

pthread_mutex_t lock;
pthread_spinlock_t spinlock;

struct thread_args {
    int thread_no;
    int use_spin_lock;
    int sleep_outside_usec;
    int sleep_inside_usec;
};

void keep_cpu_busy(int time_usec) {
    int count = 0;
    while(count < time_usec) {
        usleep(1);
        count++;
    }
}

void *worker_routine(void *args) {
    struct thread_args *thread_args = (struct thread_args *) args;
    const pthread_t pid = pthread_self();

    cpu_set_t cpuset;
    // Set CPU set to be empty
    CPU_ZERO(&cpuset);
    // Attach thread i to core (i % NUM_CORES)
    CPU_SET(thread_args->thread_no % NUM_CORES, &cpuset);

    // Set core affinity
    if(pthread_setaffinity_np(pid, sizeof(cpu_set_t), &cpuset)) {
        printf("Error in setting CPU affinity in thread %d\n", thread_args->thread_no);
	return NULL;
    }
    if(pthread_getaffinity_np(pid, sizeof(cpu_set_t), &cpuset)) {
        printf("Error in getting CPU affinity in thread %d\n", thread_args->thread_no);
        return NULL;
    }

    // Verify that thread is bound to right core
    if (!CPU_ISSET(thread_args->thread_no % NUM_CORES, &cpuset)) {
        printf("Thread %d not bound to right core\n", thread_args->thread_no);
	return NULL;
    }

    printf("Starting work in thread %d\n", thread_args->thread_no);
    
    // Sleep outside critical section
    keep_cpu_busy(thread_args->sleep_outside_usec);

    printf("Finished work outside critical section in thread %d\n", thread_args->thread_no);
    // Lock
    if(thread_args->use_spin_lock) {
	pthread_spin_lock(&spinlock);
    } else {
	pthread_mutex_lock(&lock);
    }

    // Sleep inside critical section
    keep_cpu_busy(thread_args->sleep_inside_usec);

    // Unlock
    if(thread_args->use_spin_lock) {
        pthread_spin_unlock(&spinlock);
    } else {
        pthread_mutex_unlock(&lock);
    }
    printf("Finished work inside critical section in thread %d\n", thread_args->thread_no);
    return NULL;
}

int main(int argc, char *argv[]) {
    // Get command line arguments
    int useSpinLock = atoi(argv[1]);
    int sleepOutside_usec = atoi(argv[2]);
    int sleepInside_usec = atoi(argv[3]);

    // Initialize the locking variable
    if(useSpinLock) {
        pthread_spin_init(&spinlock, 0);
    } else {
	pthread_mutex_init(&lock, NULL);
    }

    pthread_t worker_threads[NUM_THREADS];
    struct timespec start,end;
    clock_gettime(CLOCK_REALTIME, &start);

    // Initialize and start threads
    for(int i=0;i<NUM_THREADS;i++) {
        struct thread_args *t_args = malloc(sizeof(struct thread_args));
	t_args->thread_no = i;
	t_args->use_spin_lock = useSpinLock;
	t_args->sleep_outside_usec = sleepOutside_usec;
	t_args->sleep_inside_usec = sleepInside_usec;
        if (pthread_create(&worker_threads[i], NULL, worker_routine, t_args)) {
            printf("Error in creating worker thread %d\n", i);
	    return 1;
        }	
    }

    // Wait for all threads to finish
    for(int i=0;i<NUM_THREADS;i++) {
        if(pthread_join(worker_threads[i], NULL)) {
            printf("Error joining thread\n");
            return 1;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    unsigned long long wall_time_ns = (unsigned long long) ((end.tv_sec - start.tv_sec)*SEC_TO_NS) + (end.tv_nsec - start.tv_nsec);
    printf("Wall time used = %lld nanoseconds\n", wall_time_ns);
    // Destroy the locking variable
    if(useSpinLock) {
        pthread_spin_destroy(&spinlock);
    } else {
        pthread_mutex_destroy(&lock);
    }
}
