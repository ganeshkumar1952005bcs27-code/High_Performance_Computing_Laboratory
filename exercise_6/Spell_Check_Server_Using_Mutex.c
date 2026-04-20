#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define QUEUE_SIZE 10
#define MAX_WORD_LEN 50
#define RESULT_SIZE 100

// Shared structures
char work_queue[QUEUE_SIZE][MAX_WORD_LEN];
char result_buffer[RESULT_SIZE][MAX_WORD_LEN + 20];
int work_count = 0, result_count = 0;
int head = 0, tail = 0;
int finished = 0;

// Synchronization
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;

// Dictionary
const char *dictionary[] = {"LENOVO","HP","DELL","APPLE","ASUS","ACER","MICROSOFT","SAMSUNG","RAZER"};
int dict_size = 5;

// Spell check
int is_in_dictionary(char *word) {
    for (int i = 0; i < dict_size; i++) {
        if (strcmp(word, dictionary[i]) == 0) return 1;
    }
    return 0;
}

void* spell_check_worker(void* arg) {
    while (1) {
        char word[MAX_WORD_LEN];

        pthread_mutex_lock(&lock);
        // Wait while queue is empty and master is still working
        while (work_count == 0 && !finished) {
            pthread_cond_wait(&cond_not_empty, &lock);
        }

        if (work_count == 0 && finished) {
            pthread_mutex_unlock(&lock);
            break;
        }

        strcpy(word, work_queue[head]);
        head = (head + 1) % QUEUE_SIZE;
        work_count--;

        pthread_cond_signal(&cond_not_full);
        pthread_mutex_unlock(&lock);

        char res_entry[MAX_WORD_LEN + 20];
        if (is_in_dictionary(word))
            sprintf(res_entry, "%s: Correct", word);
        else
            sprintf(res_entry, "%s: Incorrect", word);

        pthread_mutex_lock(&lock);
        if (result_count < RESULT_SIZE) {
            strcpy(result_buffer[result_count++], res_entry);
        }
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t workers[3];
    int num_inputs;

    printf("How many words would you like to check? ");
    if (scanf("%d", &num_inputs) != 1) return 1;

    // Start threads
    for (int i = 0; i < 3; i++) {
        pthread_create(&workers[i], NULL, spell_check_worker, NULL);
    }

    // Master Thread: Get input from user and place in buffer
    for (int i = 0; i < num_inputs; i++) {
        char user_word[MAX_WORD_LEN];
        printf("Enter the laptop brand name for spell check %d: ", i + 1);
        scanf("%s", user_word);

        pthread_mutex_lock(&lock);
        while (work_count == QUEUE_SIZE) {
            pthread_cond_wait(&cond_not_full, &lock);
        }

        strcpy(work_queue[tail], user_word);
        tail = (tail + 1) % QUEUE_SIZE;
        work_count++;

        pthread_cond_signal(&cond_not_empty);
        pthread_mutex_unlock(&lock);
    }

    // Signal workers that no more input is coming
    pthread_mutex_lock(&lock);
    finished = 1;
    pthread_cond_broadcast(&cond_not_empty);
    pthread_mutex_unlock(&lock);

    // Wait for all threads to finish
    for (int i = 0; i < 3; i++) {
        pthread_join(workers[i], NULL);
    }

    printf("\n--- Final Result Buffer ---\n");
    for (int i = 0; i < result_count; i++) {
        printf("%s\n", result_buffer[i]);
    }

    return 0;
}
