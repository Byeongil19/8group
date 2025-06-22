#include <stdio.h>
#include <ctype.h> 

typedef struct Quiz_ {
    char question[256];
    char answer;
    char useranswer;
    int score;
    struct Quiz_ *next;
} Quiz;

int totalScore = 0;

void runQuiz(Quiz* head) {
    Quiz* current = head;
    while (current != NULL) {
        printf("%s\n", current->question);
        printf("정답 입력: ");
        scanf(" %c", &current->useranswer);

        if (toupper(current->useranswer) == toupper(current->answer)) {
            printf("정답입니다!\n");
            current->score = 10;
            totalScore += 10;
        } else {
            printf("틀렸습니다. 정답은 %c 입니다.\n", current->answer);
        }

        current = current->next;
        printf("\n");
    }
}
void listToShuffledArray(Quiz* head, Quiz** arr, int n) {
    Quiz* current = head;
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
} 