#include "quiz_game.h"

// 전역 변수
static int currentScore = 0;
static int totalQuestions = 0;

/**
 * 배열 셔플 함수 (Fisher-Yates 알고리즘)
 * @param array: 셔플할 배열
 * @param size: 배열 크기
 */
void shuffleArray(int* array, int size) {
    if (size <= 1) return;
    
    srand((unsigned int)time(NULL));
    
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        // 요소 교환
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

/**
 * 시간 제한이 있는 사용자 입력 함수
 * @param buffer: 입력을 저장할 버퍼
 * @param bufferSize: 버퍼 크기
 * @param timeLimit: 시간 제한 (초)
 * @return: 시간 내 입력 완료 시 1, 시간 초과 시 0
 */
int timedUserInput(char* buffer, int bufferSize, int timeLimit) {
    time_t startTime = time(NULL);
    int index = 0;
    
    printf("답을 입력하세요 (제한시간: %d초): ", timeLimit);
    fflush(stdout);
    
    while (1) {
        time_t currentTime = time(NULL);
        int elapsed = (int)(currentTime - startTime);
        
        // 시간 초과 체크
        if (elapsed >= timeLimit) {
            printf("\n시간 초과! 다음 문제로 넘어갑니다.\n");
            buffer[0] = '\0';  // 빈 문자열로 설정
            return 0;
        }
        
        // 키 입력이 있는지 체크 (Windows 전용)
        if (_kbhit()) {
            char ch = _getch();
            
            if (ch == '\r' || ch == '\n') {  // Enter 키
                buffer[index] = '\0';
                printf("\n");
                return 1;
            }
            else if (ch == '\b' && index > 0) {  // Backspace
                index--;
                printf("\b \b");  // 백스페이스 처리
            }
            else if (ch >= 32 && ch <= 126 && index < bufferSize - 1) {  // 출력 가능한 문자
                buffer[index++] = ch;
                printf("%c", ch);
            }
        }
        
        // CPU 사용률을 줄이기 위한 짧은 대기
        Sleep(50);
    }
}

/**
 * 점수 계산 함수
 * @param isCorrect: 정답 여부
 * @param timeLeft: 남은 시간 (초)
 * @param maxTime: 최대 시간 (초)
 * @return: 획득 점수
 */
int calculateScore(int isCorrect, int timeLeft, int maxTime) {
    if (!isCorrect) {
        return 0;  // 틀리면 0점
    }
    
    // 기본 점수 100점 + 시간 보너스 (최대 50점)
    int baseScore = 100;
    int timeBonus = (timeLeft * 50) / maxTime;
    
    return baseScore + timeBonus;
}

/**
 * 단일 퀴즈 실행 함수
 * @param question: 퀴즈 문제
 * @param questionNum: 문제 번호
 * @param totalQuestions: 전체 문제 수
 * @param timeLimit: 시간 제한
 * @return: 획득 점수
 */
int runSingleQuiz(QuizQuestion* question, int questionNum, int totalQuestions, int timeLimit) {
    char userAnswer[100];
    time_t startTime, endTime;
    
    printf("\n=== 문제 %d/%d ===\n", questionNum, totalQuestions);
    printf("문제: %s\n", question->question);
    printf("────────────────────────────────────────\n");
    
    startTime = time(NULL);
    int inputSuccess = timedUserInput(userAnswer, sizeof(userAnswer), timeLimit);
    endTime = time(NULL);
    
    int timeUsed = (int)(endTime - startTime);
    int timeLeft = timeLimit - timeUsed;
    
    if (timeLeft < 0) timeLeft = 0;
    
    // 정답 체크 (대소문자 무시, 공백 제거)
    int isCorrect = compareIgnoreCaseTrimmed(userAnswer, question->answer);
    
    if (inputSuccess && isCorrect) {
        int score = calculateScore(1, timeLeft, timeLimit);
        printf("✓ 정답입니다! (+%d점)\n", score);
        printf("소요시간: %d초 (보너스: %d초)\n", timeUsed, timeLeft);
        return score;
    }
    else {
        printf("✗ 틀렸습니다. 정답: %s\n", question->answer);
        if (!inputSuccess) {
            printf("시간 초과로 인한 오답 처리\n");
        }
        return 0;
    }
}

/**
 * 플레이어를 위한 퀴즈 실행 함수
 * @param questions: 퀴즈 문제 배열
 * @param questionCount: 문제 수
 * @param playerName: 플레이어 이름
 * @param category: 퀴즈 카테고리
 * @param timeLimit: 문제당 시간 제한
 * @return: 총 점수
 */
int runQuizForPlayer(QuizQuestion* questions, int questionCount, const char* playerName, const char* category, int timeLimit) {
    if (!questions || questionCount <= 0) {
        printf("오류: 유효하지 않은 퀴즈 데이터입니다.\n");
        return -1;
    }
    
    printf("\n🎯 퀴즈가 시작됩니다!\n");
    printf("플레이어: %s\n", playerName);
    printf("카테고리: %s\n", category);
    printf("문제 수: %d개\n", questionCount);
    printf("문제당 제한시간: %d초\n", timeLimit);
    printf("════════════════════════════════════════\n");
    
    // 문제 순서 셔플을 위한 인덱스 배열 생성
    int* questionOrder = (int*)malloc(questionCount * sizeof(int));
    if (!questionOrder) {
        printf("메모리 할당 오류!\n");
        return -1;
    }
    
    // 인덱스 배열 초기화
    for (int i = 0; i < questionCount; i++) {
        questionOrder[i] = i;
    }
    
    // 문제 순서 셔플
    shuffleArray(questionOrder, questionCount);
    
    currentScore = 0;
    totalQuestions = questionCount;
    
    // 각 문제 실행
    for (int i = 0; i < questionCount; i++) {
        int questionIndex = questionOrder[i];
        int score = runSingleQuiz(&questions[questionIndex], i + 1, questionCount, timeLimit);
        currentScore += score;
        
        // 중간 점수 표시
        printf("현재 점수: %d점\n", currentScore);
        
        // 마지막 문제가 아니면 잠시 대기
        if (i < questionCount - 1) {
            printf("\n다음 문제로 넘어가려면 아무 키나 누르세요...");
            _getch();
            printf("\n");
        }
    }
    
    // 최종 결과 표시
    printf("\n🏆 퀴즈 완료!\n");
    printf("════════════════════════════════════════\n");
    printf("플레이어: %s\n", playerName);
    printf("카테고리: %s\n", category);
    printf("총 문제 수: %d개\n", questionCount);
    printf("총 점수: %d점\n", currentScore);
    printf("평균 점수: %.1f점\n", (float)currentScore / questionCount);
    
    // 등급 계산 및 표시
    displayGrade(currentScore, questionCount);
    
    free(questionOrder);
    return currentScore;
}

/**
 * 등급 표시 함수
 * @param totalScore: 총 점수
 * @param questionCount: 문제 수
 */
void displayGrade(int totalScore, int questionCount) {
    int maxPossibleScore = questionCount * 150;  // 문제당 최대 150점
    float percentage = ((float)totalScore / maxPossibleScore) * 100;
    
    printf("정답률: %.1f%%\n", percentage);
    
    if (percentage >= 90) {
        printf("등급: A+ (최우수) 🌟\n");
    }
    else if (percentage >= 80) {
        printf("등급: A (우수) ⭐\n");
    }
    else if (percentage >= 70) {
        printf("등급: B (보통) 👍\n");
    }
    else if (percentage >= 60) {
        printf("등급: C (미흡) 📚\n");
    }
    else {
        printf("등급: D (재도전!) 💪\n");
    }
}

/**
 * 현재 게임 상태 반환 함수들
 */
int getCurrentScore(void) {
    return currentScore;
}

int getTotalQuestions(void) {
    return totalQuestions;
}

/**
 * 게임 상태 초기화
 */
void resetGameState(void) {
    currentScore = 0;
    totalQuestions = 0;
}