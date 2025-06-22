#include "quiz_game.h"
#include "file_manager.h"
#include <sys/stat.h>  // mkdir
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755)
#endif

int loadQuestionsFromFile(const char* filename, QuizQuestion* questions, int maxQuestions) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("파일을 열 수 없습니다: %s\n", filename);
        return 0;
    }

    int count = 0;
    char line[500];

    while (fgets(line, sizeof(line), file) && count < maxQuestions) {
        if (line[0] == '\n' || line[0] == '#') continue;
        line[strcspn(line, "\n")] = 0;

        char* delimiter = strchr(line, '|');
        if (delimiter) {
            *delimiter = '\0';

            char* question = line;
            char* answer = delimiter + 1;

            while (*question == ' ') question++;
            while (*answer == ' ') answer++;

            strncpy(questions[count].question, question, sizeof(questions[count].question) - 1);
            questions[count].question[sizeof(questions[count].question) - 1] = '\0';

            strncpy(questions[count].answer, answer, sizeof(questions[count].answer) - 1);
            questions[count].answer[sizeof(questions[count].answer) - 1] = '\0';

            // 뒤 공백 제거
            int len = strlen(questions[count].question);
            while (len > 0 && questions[count].question[len - 1] == ' ')
                questions[count].question[--len] = '\0';

            len = strlen(questions[count].answer);
            while (len > 0 && questions[count].answer[len - 1] == ' ')
                questions[count].answer[--len] = '\0';

            count++;
        }
    }

    fclose(file);
    printf("파일 '%s'에서 %d개의 문제를 로드했습니다.\n", filename, count);
    return count;
}

int saveQuizResult(const char* playerName, const char* category, int score, int totalQuestions, int timeTaken) {
    FILE* file = fopen("results/results.txt", "a");
    if (!file) {
        MKDIR("results");
        file = fopen("results/results.txt", "a");
        if (!file) {
            printf("결과 파일을 열 수 없습니다.\n");
            return 0;
        }
    }

    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);

    fprintf(file, "%04d-%02d-%02d|%02d:%02d:%02d|%s|%s|%d|%d|%d\n",
        timeinfo->tm_year + 1900,
        timeinfo->tm_mon + 1,
        timeinfo->tm_mday,
        timeinfo->tm_hour,
        timeinfo->tm_min,
        timeinfo->tm_sec,
        playerName,
        category,
        score,
        totalQuestions,
        timeTaken
    );

    fclose(file);
    printf("결과가 성공적으로 저장되었습니다!\n");
    return 1;
}

int parseQuizData(const char* line, char* date, char* timeStr, char* name, char* category,
                  int* score, int* totalQuestions, int* timeTaken) {
    char temp[300];
    strncpy(temp, line, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    char* tokens[7];
    int tokenCount = 0;

    char* token = strtok(temp, "|");
    while (token && tokenCount < 7) {
        tokens[tokenCount++] = token;
        token = strtok(NULL, "|");
    }

    if (tokenCount != 7) return 0;

    strncpy(date, tokens[0], 19);
    strncpy(timeStr, tokens[1], 19);
    strncpy(name, tokens[2], 49);
    strncpy(category, tokens[3], 19);
    *score = atoi(tokens[4]);
    *totalQuestions = atoi(tokens[5]);
    *timeTaken = atoi(tokens[6]);

    return 1;
}

void displaySavedResults(int maxResults) {
    FILE* file = fopen("results/results.txt", "r");
    if (!file) {
        printf("저장된 결과가 없습니다.\n");
        return;
    }

    char line[300];
    int count = 0;

    printf("\n=== 저장된 퀴즈 결과 ===\n");
    printf("%-12s %-8s %-15s %-10s %-6s %-8s\n",
           "날짜", "시간", "이름", "카테고리", "점수", "소요시간");
    printf("================================================================\n");

    while (fgets(line, sizeof(line), file)) {
        if (maxResults > 0 && count >= maxResults) break;

        line[strcspn(line, "\n")] = '\0';

        char date[20], timeStr[20], name[50], category[20];
        int score, totalQuestions, timeTaken;

        if (parseQuizData(line, date, timeStr, name, category, &score, &totalQuestions, &timeTaken)) {
            double percentage = (double)score / totalQuestions * 100;
            int minutes = timeTaken / 60;
            int seconds = timeTaken % 60;

            printf("%-12s %-8s %-15s %-10s %2d/%2d(%5.1f%%) %2d:%02d\n",
                   date, timeStr, name, category, score, totalQuestions, percentage, minutes, seconds);
            count++;
        }
    }

    if (count == 0)
        printf("표시할 결과가 없습니다.\n");
    else
        printf("================================================================\n총 %d개의 결과를 표시했습니다.\n", count);

    fclose(file);
}

void displayResultsByCategory(const char* category, int maxResults) {
    FILE* file = fopen("results/results.txt", "r");
    if (!file) {
        printf("저장된 결과가 없습니다.\n");
        return;
    }

    char line[300];
    int count = 0;

    printf("\n=== %s 카테고리 결과 ===\n", category);
    printf("%-12s %-8s %-15s %-6s %-8s\n", "날짜", "시간", "이름", "점수", "소요시간");
    printf("========================================================\n");

    while (fgets(line, sizeof(line), file)) {
        if (maxResults > 0 && count >= maxResults) break;

        line[strcspn(line, "\n")] = '\0';

        char date[20], timeStr[20], name[50], fileCategory[20];
        int score, totalQuestions, timeTaken;

        if (parseQuizData(line, date, timeStr, name, fileCategory, &score, &totalQuestions, &timeTaken)) {
            if (strcmp(fileCategory, category) == 0) {
                double percentage = (double)score / totalQuestions * 100;
                int minutes = timeTaken / 60;
                int seconds = timeTaken % 60;

                printf("%-12s %-8s %-15s %2d/%2d(%5.1f%%) %2d:%02d\n",
                       date, timeStr, name, score, totalQuestions, percentage, minutes, seconds);
                count++;
            }
        }
    }

    if (count == 0)
        printf("해당 카테고리의 결과가 없습니다.\n");
    else
        printf("========================================================\n총 %d개의 결과를 표시했습니다.\n", count);

    fclose(file);
}

int getAvailableQuizFiles(char fileList[][50], int maxFiles) {
    const char* quizFiles[] = {
        "quiz_joke.txt",
        "quiz_science.txt",
        "quiz_history.txt"
    };

    const char* categories[] = {
        "개그", "과학", "역사"
    };

    int count = 0;
    int totalFiles = sizeof(quizFiles) / sizeof(quizFiles[0]);

    for (int i = 0; i < totalFiles && count < maxFiles; i++) {
        FILE* file = fopen(quizFiles[i], "r");
        if (file) {
            strncpy(fileList[count], categories[i], 49);
            fclose(file);
            count++;
        }
    }

    return count;
}

int getCategoryFilename(const char* category, char* filename) {
    if (strcmp(category, "개그") == 0) {
        strcpy(filename, "quiz_joke.txt");
        return 1;
    }
    else if (strcmp(category, "과학") == 0) {
        strcpy(filename, "quiz_science.txt");
        return 1;
    }
    else if (strcmp(category, "역사") == 0) {
        strcpy(filename, "quiz_history.txt");
        return 1;
    }

    return 0;
}

void shuffleQuestions(QuizQuestion* questions, int count) {
    srand((unsigned int)time(NULL));  // 시드 설정

    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        // Swap
        QuizQuestion temp = questions[i];
        questions[i] = questions[j];
        questions[j] = temp;
    }
}
