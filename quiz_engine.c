#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <conio.h>

typedef struct Quiz_ {
    char question[256];
    char answer;
    char useranswer;
    int score;
    struct Quiz_ *next;
} Quiz;

int totalScore = 0;

int timeout_60s(char* userAnswer) {
    const int timeoutSec = 60;
    time_t startTime = time(NULL);
    while (1) {
        if (kbhit()) {
            char ch = getch();
            if (ch == '\r' || ch == '\n') {
                continue;
            }               
            *userAnswer = ch;
                return 1;
        }
        if (difftime(time(NULL), startTime) >= timeoutSec) {
            return 0;
        }
    }
}


void runQuiz(Quiz* head) {
    Quiz* current = head;
    totalScore = 0;
    while (current != NULL) {
        printf("%s\n", current->question);
        printf("정답 입력 (60초 이내): ");

        if (!timeout_60s(&current->useranswer)) {
            printf("\n시간 초과! 답변이 입력되지 않았습니다.\n");
            current->score = 0;
        } else {
            if (toupper(current->useranswer) == toupper(current->answer)) {
                printf("정답입니다!\n");
                current->score = 10;
                totalScore += 10;
            } else {
                printf("틀렸습니다. 정답은 %c 입니다.\n", current->answer);
                current->score = 0;
            }
        }
        
        current = current->next;
        printf("\n");
    }
    printf("퀴즈 종료! 총 점수는 %d점 입니다.\n", totalScore);
}

void shuffleLinkedList(Quiz** head, int n) {
    Quiz* arr[n];
    Quiz* current = *head;

    for (int i = 0; i < n; i++) {
        arr[i] = current;
        current = current->next;
    }
    srand((unsigned int)time(NULL));
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Quiz* temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
    for (int i = 0; i < n - 1; i++) {
        arr[i]->next = arr[i + 1];
    }
    arr[n - 1]->next = NULL;
    *head = arr[0];
}