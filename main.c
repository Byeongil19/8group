#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "quiz_game.h"
#include "file_manager.h"
#include "admin.h"
#include "quiz_engine.h"

#define MAX_QUESTIONS 100
#define MAX_NAME_LEN 50

#ifdef _WIN32
#include <windows.h>
#endif

void getLine(char* buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

void suggest_question() {
    char category[50];
    char question[256];
    char answer[100];
    FILE* fp = fopen("pending_questions.txt", "a");
    if (!fp) {
        printf("파일을 열 수 없습니다.\n");
        return;
    }
    printf("\n--- 문제 제안 ---\n");
    printf("카테고리를 선택하세요 (개그 / 과학 / 역사): ");
    getLine(category, sizeof(category));
    printf("문제를 입력하세요: ");
    getLine(question, sizeof(question));
    printf("정답을 입력하세요: ");
    getLine(answer, sizeof(answer));
    fprintf(fp, "%s|%s|%s\n", category, question, answer);
    fclose(fp);
    printf("문제가 성공적으로 제안되었습니다!\n");
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    int mode;

    while (1) {
        printf("\n=== 넌센스 퀴즈 게임 ===\n");
        printf("1. 퀴즈 풀기 (텍스트 기반)\n");
        printf("2. 관리자 모드\n");
        printf("3. 제한시간 객관식 퀴즈\n");
        printf("4. 퀴즈 문제 제안하기\n");
        printf("0. 종료\n");
        printf("모드를 선택하세요: ");
        scanf("%d", &mode);
        getchar();

        if (mode == 0) {
            printf("프로그램을 종료합니다.\n");
            break;
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
                continue;
            }

            strcpy(category, fileList[choice - 1]);
            if (!getCategoryFilename(category, filename)) {
                printf("카테고리 파일을 찾을 수 없습니다.\n");
                continue;
            }

            quizCount = loadQuestionsFromFile(filename, questions, MAX_QUESTIONS);
            if (quizCount == 0) {
                printf("퀴즈를 불러오지 못했습니다.\n");
                continue;
            }

            shuffleQuestions(questions, quizCount);
            printf("\n총 %d문제를 불러왔습니다. 퀴즈를 시작합니다!\n", quizCount);

            time_t startTime = time(NULL);
            int score = 0;
            char answer[200];

            for (int i = 0; i < quizCount; i++) {
                printf("\nQ%d. %s\n>> ", i + 1, questions[i].question);
                getLine(answer, sizeof(answer));
                if (strcmp(answer, questions[i].answer) == 0) {
                    printf("정답입니다!\n");
                    score++;
                } else {
                    printf("오답입니다! 정답: %s\n", questions[i].answer);
                }
            }

            time_t endTime = time(NULL);
            int elapsedTime = (int)(endTime - startTime);

            printf("\n퀴즈 종료! %d/%d문제 정답\n", score, quizCount);
            printf("총 소요 시간: %d초\n", elapsedTime);
            saveQuizResult(playerName, category, score, quizCount, elapsedTime);

            char view;
            printf("\n저장된 결과를 보시겠습니까? (y/n): ");
            scanf(" %c", &view);
            getchar();

            if (view == 'y' || view == 'Y') {
                displaySavedResults(999);
                printf("\n엔터를 누르면 메뉴로 돌아갑니다...");
                getchar();  
            }
        }

        else if (mode == 2) {
            admin_mode();
        }

        else if (mode == 3) {
            Quiz* head = NULL;
            Quiz* current = NULL;
            const char* questions[] = {
                "C 언어에서 'int'는 어떤 자료형인가요?\nA) 정수 B) 실수 C) 문자 D) 포인터",
                "대한민국의 수도는 어디인가요?\nA) 부산 B) 대전 C) 서울 D) 인천",
                "바다에서 가장 짠 바다는?\nA) 발트해 B) 홍해 C) 지중해 D) 동해"
            };
            const char answers[] = { 'A', 'C', 'B' };

            for (int i = 0; i < 3; i++) {
                Quiz* newQuiz = (Quiz*)malloc(sizeof(Quiz));
                strcpy(newQuiz->question, questions[i]);
                newQuiz->answer = answers[i];
                newQuiz->score = 0;
                newQuiz->next = NULL;
                if (head == NULL) {
                    head = newQuiz;
                    current = newQuiz;
                } else {
                    current->next = newQuiz;
                    current = newQuiz;
                }
            }

            shuffleLinkedList(&head, 3);
            runQuiz(head);

            while (head != NULL) {
                Quiz* temp = head;
                head = head->next;
                free(temp);
            }

            printf("\n엔터를 누르면 메뉴로 돌아갑니다...");
            getchar();  
        }

        else if (mode == 4) {
            suggest_question();
            printf("\n엔터를 누르면 메뉴로 돌아갑니다...");
            getchar();
        }

        else {
            printf("잘못된 선택입니다.\n");
        }
    }

    return 0;
}
