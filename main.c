#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "quiz_game.h"
#include "file_manager.h"
#include "quiz_engine.h"
#include "ui_utils.h"
#include "admin.h"  

#define MAX_QUESTIONS 100
#define MAX_NAME_LEN 50

void getLine(char* buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

int main() {
    int mode;
    printf("=== 넌센스 퀴즈 게임 ===\n\n");
    printf("1. 퀴즈 풀기\n");
    printf("2. 관리자 모드\n");
    printf("0. 종료\n");
    printf("모드를 선택하세요: ");
    scanf("%d", &mode);
    getchar();  

    if (mode == 3) {
        printf("프로그램을 종료합니다.\n");
        return 0;
    }
    else if (mode == 2) {
        admin_mode(); 
        return 0;
    }
    else if (mode == 1) {
        QuizQuestion questions[MAX_QUESTIONS];
        char playerName[MAX_NAME_LEN];
        char category[50];
        char filename[100];
        char fileList[10][50];
        int quizCount = 0;
        int fileCount = 0;

        printf("당신의 이름을 입력하세요: ");
        getLine(playerName, sizeof(playerName));

        printf("\n[사용 가능한 카테고리 목록]\n");
        fileCount = getAvailableQuizFiles(fileList, 10);
        for (int i = 0; i < fileCount; i++) {
            printf("%d. %s\n", i + 1, fileList[i]);
        }

        int choice;
        printf("\n카테고리를 선택하세요 (번호): ");
        scanf("%d", &choice);
        getchar();

        if (choice < 1 || choice > fileCount) {
            printf("잘못된 선택입니다.\n");
            return 1;
        }

        strcpy(category, fileList[choice - 1]);

        if (!getCategoryFilename(category, filename)) {
            printf("카테고리 파일을 찾을 수 없습니다.\n");
            return 1;
        }

        quizCount = loadQuestionsFromFile(filename, questions, MAX_QUESTIONS);
        if (quizCount == 0) {
            printf("퀴즈를 불러오지 못했습니다.\n");
            return 1;
        }

        printf("\n총 %d문제를 불러왔습니다. 퀴즈를 시작합니다!\n", quizCount);

        time_t startTime = time(NULL);

        int score = 0;
        char answer[200];

        for (int i = 0; i < quizCount; i++) {
            printf("\nQ%d. %s\n>> ", i + 1, questions[i].question);
            getLine(answer, sizeof(answer));

            if (strcmp(answer, questions[i].answer) == 0) {
                printf("정답!\n");
                score++;
            }
            else {
                printf("오답! 정답: %s\n", questions[i].answer);
            }
        }

        time_t endTime = time(NULL);
        int elapsedTime = (int)(endTime - startTime);

        printf("\n 퀴즈 종료! %d/%d문제 정답\n", score, quizCount);
        printf("총 소요 시간: %d초\n", elapsedTime);

        saveQuizResult(playerName, category, score, quizCount, elapsedTime);

        char view;
        printf("\n저장된 결과를 보시겠습니까? (y/n): ");
        scanf(" %c", &view);
        getchar();

        if (view == 'y' || view == 'Y') {
            displaySavedResults(10);
        }
    }
    else {
        printf("잘못된 선택입니다.\n");
        return 1;
    }

    return 0;
}

