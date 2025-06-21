#include "quiz_game.h"
#include "file_manager.h"

/**
 * 퀴즈 파일에서 문제들을 로드하는 함수
 * @param filename: 로드할 파일명
 * @param questions: 문제를 저장할 배열
 * @param maxQuestions: 최대 문제 수
 * @return: 실제 로드된 문제 수
 */
int loadQuestionsFromFile(const char* filename, QuizQuestion* questions, int maxQuestions) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("파일을 열 수 없습니다: %s\n", filename);
        return 0;
    }
    
    int count = 0;
    char line[500];
    
    while (fgets(line, sizeof(line), file) && count < maxQuestions) {
        // 빈 줄이나 주석 줄('#'로 시작) 건너뛰기
        if (line[0] == '\n' || line[0] == '#') {
            continue;
        }
        
        // 줄바꿈 문자 제거
        line[strcspn(line, "\n")] = 0;
        
        // 구분자 '|'를 찾아서 질문과 답변 분리
        char* delimiter = strchr(line, '|');
        if (delimiter != NULL) {
            *delimiter = '\0'; // 구분자를 null로 변경
            
            // 질문 복사 (앞뒤 공백 제거)
            char* question = line;
            while (*question == ' ') question++; // 앞 공백 제거
            strncpy(questions[count].question, question, sizeof(questions[count].question) - 1);
            questions[count].question[sizeof(questions[count].question) - 1] = '\0';
            
            // 답변 복사 (앞뒤 공백 제거)
            char* answer = delimiter + 1;
            while (*answer == ' ') answer++; // 앞 공백 제거
            strncpy(questions[count].answer, answer, sizeof(questions[count].answer) - 1);
            questions[count].answer[sizeof(questions[count].answer) - 1] = '\0';
            
            // 뒤 공백 제거
            int len = strlen(questions[count].question);
            while (len > 0 && questions[count].question[len-1] == ' ') {
                questions[count].question[--len] = '\0';
            }
            
            len = strlen(questions[count].answer);
            while (len > 0 && questions[count].answer[len-1] == ' ') {
                questions[count].answer[--len] = '\0';
            }
            
            count++;
        }
    }
    
    fclose(file);
    printf("파일 '%s'에서 %d개의 문제를 로드했습니다.\n", filename, count);
    return count;
}

/**
 * 퀴즈 결과를 파일에 저장하는 함수
 * @param playerName: 플레이어 이름
 * @param category: 퀴즈 카테고리
 * @param score: 점수
 * @param totalQuestions: 총 문제 수
 * @param timeTaken: 소요 시간 (초)
 * @return: 성공시 1, 실패시 0
 */
int saveQuizResult(const char* playerName, const char* category, int score, int totalQuestions, int timeTaken) {
    FILE* file = fopen("results/results.txt", "a");
    if (file == NULL) {
        // results 디렉토리가 없으면 생성 시도
        system("mkdir results 2>nul"); // Windows용
        file = fopen("results/results.txt", "a");
        if (file == NULL) {
            printf("결과 파일을 열 수 없습니다.\n");
            return 0;
        }
    }
    
    // 현재 시간 가져오기
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    
    // 결과 저장 형식: 날짜|시간|이름|카테고리|점수|총문제수|소요시간
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
            timeTaken);
    
    fclose(file);
    printf("결과가 성공적으로 저장되었습니다!\n");
    return 1;
}

/**
 * 저장된 퀴즈 결과들을 화면에 표시하는 함수
 * @param maxResults: 표시할 최대 결과 수 (0이면 모두 표시)
 */
void displaySavedResults(int maxResults) {
    FILE* file = fopen("results/results.txt", "r");
    if (file == NULL) {
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
        if (maxResults > 0 && count >= maxResults) {
            break;
        }
        
        // 줄바꿈 제거
        line[strcspn(line, "\n")] = 0;
        
        // 파싱용 임시 변수들
        char date[20], time[20], name[50], category[20];
        int score, totalQuestions, timeTaken;
        
        if (parseQuizData(line, date, time, name, category, &score, &totalQuestions, &timeTaken)) {
            double percentage = (double)score / totalQuestions * 100;
            int minutes = timeTaken / 60;
            int seconds = timeTaken % 60;
            
            printf("%-12s %-8s %-15s %-10s %2d/%2d(%5.1f%%) %2d:%02d\n",
                   date, time, name, category, score, totalQuestions, percentage, minutes, seconds);
            count++;
        }
    }
    
    if (count == 0) {
        printf("표시할 결과가 없습니다.\n");
    } else {
        printf("================================================================\n");
        printf("총 %d개의 결과를 표시했습니다.\n", count);
    }
    
    fclose(file);
}

/**
 * 결과 파일의 한 줄을 파싱하는 함수
 * @param line: 파싱할 문자열
 * @param date, time, name, category: 파싱된 데이터를 저장할 포인터들
 * @param score, totalQuestions, timeTaken: 파싱된 숫자 데이터를 저장할 포인터들
 * @return: 성공시 1, 실패시 0
 */
int parseQuizData(const char* line, char* date, char* time, char* name, char* category, 
                  int* score, int* totalQuestions, int* timeTaken) {
    // 구분자 '|'로 문자열 분리
    char temp[300];
    strcpy(temp, line);
    
    char* tokens[7];
    int tokenCount = 0;
    
    char* token = strtok(temp, "|");
    while (token != NULL && tokenCount < 7) {
        tokens[tokenCount++] = token;
        token = strtok(NULL, "|");
    }
    
    if (tokenCount != 7) {
        return 0; // 파싱 실패
    }
    
    // 각 필드에 값 할당
    strcpy(date, tokens[0]);
    strcpy(time, tokens[1]);
    strcpy(name, tokens[2]);
    strcpy(category, tokens[3]);
    *score = atoi(tokens[4]);
    *totalQuestions = atoi(tokens[5]);
    *timeTaken = atoi(tokens[6]);
    
    return 1; // 파싱 성공
}

/**
 * 특정 카테고리의 결과만 표시하는 함수
 * @param category: 표시할 카테고리
 * @param maxResults: 최대 표시 수
 */
void displayResultsByCategory(const char* category, int maxResults) {
    FILE* file = fopen("results/results.txt", "r");
    if (file == NULL) {
        printf("저장된 결과가 없습니다.\n");
        return;
    }
    
    char line[300];
    int count = 0;
    
    printf("\n=== %s 카테고리 결과 ===\n", category);
    printf("%-12s %-8s %-15s %-6s %-8s\n", 
           "날짜", "시간", "이름", "점수", "소요시간");
    printf("========================================================\n");
    
    while (fgets(line, sizeof(line), file)) {
        if (maxResults > 0 && count >= maxResults) {
            break;
        }
        
        line[strcspn(line, "\n")] = 0;
        
        char date[20], time[20], name[50], fileCategory[20];
        int score, totalQuestions, timeTaken;
        
        if (parseQuizData(line, date, time, name, fileCategory, &score, &totalQuestions, &timeTaken)) {
            if (strcmp(fileCategory, category) == 0) {
                double percentage = (double)score / totalQuestions * 100;
                int minutes = timeTaken / 60;
                int seconds = timeTaken % 60;
                
                printf("%-12s %-8s %-15s %2d/%2d(%5.1f%%) %2d:%02d\n",
                       date, time, name, score, totalQuestions, percentage, minutes, seconds);
                count++;
            }
        }
    }
    
    if (count == 0) {
        printf("해당 카테고리의 결과가 없습니다.\n");
    } else {
        printf("========================================================\n");
        printf("총 %d개의 결과를 표시했습니다.\n", count);
    }
    
    fclose(file);
}

/**
 * 사용 가능한 퀴즈 파일 목록을 반환하는 함수
 * @param fileList: 파일명을 저장할 배열
 * @param maxFiles: 최대 파일 수
 * @return: 찾은 파일 수
 */
int getAvailableQuizFiles(char fileList[][50], int maxFiles) {
    const char* quizFiles[] = {
        "data/quiz_joke.txt",
        "data/quiz_science.txt", 
        "data/quiz_history.txt"
    };
    
    const char* categories[] = {
        "개그",
        "과학",
        "역사"
    };
    
    int count = 0;
    int totalFiles = sizeof(quizFiles) / sizeof(quizFiles[0]);
    
    for (int i = 0; i < totalFiles && count < maxFiles; i++) {
        FILE* file = fopen(quizFiles[i], "r");
        if (file != NULL) {
            strcpy(fileList[count], categories[i]);
            count++;
            fclose(file);
        }
    }
    
    return count;
}

/**
 * 카테고리명으로 파일명을 반환하는 함수
 * @param category: 카테고리명
 * @param filename: 파일명을 저장할 버퍼
 * @return: 성공시 1, 실패시 0
 */
int getCategoryFilename(const char* category, char* filename) {
    if (strcmp(category, "개그") == 0) {
        strcpy(filename, "data/quiz_joke.txt");
        return 1;
    }
    else if (strcmp(category, "과학") == 0) {
        strcpy(filename, "data/quiz_science.txt");
        return 1;
    }
    else if (strcmp(category, "역사") == 0) {
        strcpy(filename, "data/quiz_history.txt");
        return 1;
    }
    
    return 0;
}