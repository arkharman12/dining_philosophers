#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>

// Number of philosophers
#define PHILOSOPHER_NUM 5
// Sleep time for each philosopher in seconds
#define SLEEP_TIME_SECS 3
 
// Different states of each philosopher with number
enum {THINKING, HUNGRY, EATING} state[PHILOSOPHER_NUM];

// Condition variables and associated mutex lock
pthread_cond_t cond_vars[PHILOSOPHER_NUM];
pthread_mutex_t mutex_lock;
 
// Return the left philosopher
int left_philosopher(int number) {
    if (number == 0)
        return PHILOSOPHER_NUM - 1;
    else
        return number - 1;
}
 
// Return the right philosopher
int right_philosopher(int number) {
    if (number == PHILOSOPHER_NUM - 1)
        return 0;
    else
        return number + 1;
}

// Call this function to start eating
void test(int i) {
    // If left and right philosophers aren't eating nor hungry then start eating
    if ((state[(i + 4) % 5] != EATING) && (state[i] == HUNGRY) && (state[(i + 1) % 5] != EATING)) {
        state[i] = EATING;
        pthread_cond_signal(&cond_vars[i]);
    }
}
 
// Call this function when a philosopher wants to pickup both of its forks
void pickup(int number) {
    // Lock it
    pthread_mutex_lock(&mutex_lock);
 
    state[number] = HUNGRY;
    test(number);
    while (state[number] != EATING) {
        pthread_cond_wait(&cond_vars[number], &mutex_lock);
    }

    // Unlock it
    pthread_mutex_unlock(&mutex_lock);
}
 
// Call this function when a philosopher wants to return both of its forks
void putdown(int number) {
    // Lock it
    pthread_mutex_lock(&mutex_lock);
 
    state[number] = THINKING;
    test(left_philosopher(number));
    test(right_philosopher(number));
    
    // Unlock it
    pthread_mutex_unlock(&mutex_lock);
}

// Call this function when a philosohper starts to think
void think(int time_to_sleep) {
    sleep(time_to_sleep);
}

// Call this function when a philosohper starts to eat
void eat(int time_to_sleep) {
    sleep(time_to_sleep);
}

// Dine the philosohpers
void *dine(void *ptr) {
    int *lnumber = (int *)ptr;
    int number = *lnumber;
    int time_to_sleep;
 
    srandom((unsigned)time(NULL));
    
    while(1) {
        time_to_sleep = (int)((random() % SLEEP_TIME_SECS) + 1);
        think(time_to_sleep);
 
        pickup(number);
 
        printf("Philosopher %d is eating\n", number);
 
        time_to_sleep = (int)((random() % SLEEP_TIME_SECS) + 1);
        eat(time_to_sleep);
 
        printf("Philosopher %d is thinking\n", number);
        putdown(number);
         
    }
}

// Main or driver method
int main(int argc, char *argv[]) {
    // Thread id for each philosopher
    int thread_id[PHILOSOPHER_NUM];

    // Philosopher threads
    pthread_t tid[PHILOSOPHER_NUM];

    // Initialize
    for (int i = 0; i < PHILOSOPHER_NUM; i++) {
        state[i] = THINKING;
        thread_id[i] = i;
        pthread_cond_init(&cond_vars[i], NULL);
    }
    pthread_mutex_init(&mutex_lock, NULL);

    // Create the philosopher threads
    for (int i = 0; i < PHILOSOPHER_NUM; i++) {
        pthread_create(&tid[i], 0, dine, (void *)&thread_id[i]);
    }

    // Wait for philosopher thread termination
    for (int i = 0; i < PHILOSOPHER_NUM; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
