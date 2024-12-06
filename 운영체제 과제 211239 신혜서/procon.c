#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_COUNTER 10

HANDLE semWrite, semRead; // Windows semaphore
int queue[N_COUNTER]; // shared buffer
int wptr = 0; // write pointer for queue[]
int rptr = 0; // read pointer for queue[]
HANDLE mutex; // Windows mutex

DWORD WINAPI producer(LPVOID arg);
DWORD WINAPI consumer(LPVOID arg);
void mywrite(int n);
int myread(void);

// producer thread function
DWORD WINAPI producer(LPVOID arg) {
    for(int i = 0; i < 10; i++) {
        mywrite(i); // write i into the shared memory
        printf("producer : wrote %d\n", i);

        // sleep m milliseconds
        int m = rand() % 10;
        Sleep(m * 10); // m*10
    }
    return 0;
}

// consumer thread function
DWORD WINAPI consumer(LPVOID arg) {
    for(int i = 0; i < 10; i++) {
        int n = myread(); // read a value from the shared memory
        printf("\tconsumer : read %d\n", n);

        // sleep m milliseconds
        int m = rand() % 10;
        Sleep(m * 10); // m*10
    }
    return 0;
}

// write n into the shared memory
void mywrite(int n) {
    WaitForSingleObject(semWrite, INFINITE);
    WaitForSingleObject(mutex, INFINITE);

    queue[wptr] = n;
    wptr = (wptr + 1) % N_COUNTER;

    ReleaseMutex(mutex);
    ReleaseSemaphore(semRead, 1, NULL);
}

// read a value from the shared memory
int myread() {
    WaitForSingleObject(semRead, INFINITE);
    WaitForSingleObject(mutex, INFINITE);

    int n = queue[rptr];
    rptr = (rptr + 1) % N_COUNTER;

    ReleaseMutex(mutex);
    ReleaseSemaphore(semWrite, 1, NULL);

    return n;
}

int main() {
    HANDLE t[2]; // thread handles
    srand((unsigned int)time(NULL));

    // Initialize mutex
    mutex = CreateMutex(NULL, FALSE, NULL);

    // Initialize semaphores
    semWrite = CreateSemaphore(NULL, N_COUNTER, N_COUNTER, NULL);
    semRead = CreateSemaphore(NULL, 0, N_COUNTER, NULL);

    // Create the threads for the producer and consumer
    t[0] = CreateThread(NULL, 0, producer, NULL, 0, NULL);
    t[1] = CreateThread(NULL, 0, consumer, NULL, 0, NULL);

    WaitForMultipleObjects(2, t, TRUE, INFINITE);

    // Close handles
    CloseHandle(t[0]);
    CloseHandle(t[1]);
    CloseHandle(mutex);
    CloseHandle(semWrite);
    CloseHandle(semRead);

    return 0;
}


