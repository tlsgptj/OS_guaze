#include <windows.h>
#include <stdio.h>

// 공유 자원
int sharedResource = 0;

// 뮤텍스 핸들
HANDLE hMutex;

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    // 뮤텍스를 얻기 위해 대기
    WaitForSingleObject(hMutex, INFINITE);

    // 공유 자원에 접근
    sharedResource++;
    printf("Thread %d has updated the shared resource: %d\n %d\n", GetCurrentThreadId(), sharedResource);

    // 뮤텍스 해제
    ReleaseMutex(hMutex);

    return 0;
}

int main() {
    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == NULL) {
        printf("Failed to create Mutex\n");
        return 1;
    }

    // 스레드 생성
    HANDLE hThread1 = CreateThread(NULL, 0, ThreadFunction, NULL, 0, NULL);
    HANDLE hThread2 = CreateThread(NULL, 0, ThreadFunction, NULL, 0, NULL);

    // 스레드 종료 대기
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    // 핸들 닫기
    CloseHandle(hThread1);
    CloseHandle(hThread2);
    CloseHandle(hMutex);

    return 0;
}
