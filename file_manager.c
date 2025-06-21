#ifndef QUIZ_ENGINE_H
#define QUIZ_ENGINE_H

// 퀴즈 엔진 함수 선언

/**
 * 배열 셔플 함수
 * @param array: 셔플할 정수 배열
 * @param size: 배열 크기
 */
void shuffleArray(int* array, int size);

/**
 * 시간 제한이 있는 사용자 입력 함수
 * @param buffer: 입력을 저장할 버퍼
 * @param bufferSize: 버퍼 크기
 * @param timeLimit: 시간 제한 (초)
 * @return: 성공 시 1, 시간 초과 시 0
 */
int timedUserInput(char* buffer, int bufferSize, int timeLimit);

/**
 * 점수 계산 함수
 * @param isCorrect: 정답 여부 (1: 정답, 0: 오답)
 * @param timeLeft: 남은 시간 (초)
 * @param maxTime: 최대 시간 (초)
 * @return: 계산된 점수
 */
int calculateScore(int isCorrect, int timeLeft, int maxTime);

/**
 * 단일 퀴즈 실행 함수
 * @param question: QuizQuestion 구조체 포인터
 * @param questionNum: 현재 문제 번호
 * @param totalQuestions: 전체 문제 수
 * @param timeLimit: 문제당 시간 제한 (초)
 * @return: 획득한 점수
 */
int runSingleQuiz(QuizQuestion* question, int questionNum, int totalQuestions, int timeLimit);

/**
 * 플레이어를 위한 퀴즈 실행 메인 함수
 * @param questions: QuizQuestion 배열
 * @param questionCount: 문제 수
 * @param playerName: 플레이어 이름
 * @param category: 퀴즈 카테고리
 * @param timeLimit: 문제당 시간 제한 (초)
 * @return: 총 획득 점수 (오류 시 -1)
 */
int runQuizForPlayer(QuizQuestion* questions, int questionCount, const char* playerName, const char* category, int timeLimit);

/**
 * 등급 표시 함수
 * @param totalScore: 총 점수
 * @param questionCount: 문제 수
 */
void displayGrade(int totalScore, int questionCount);

/**
 * 현재 점수 반환
 * @return: 현재 게임 점수
 */
int getCurrentScore(void);

/**
 * 전체 문제 수 반환
 * @return: 전체 문제 수
 */
int getTotalQuestions(void);

/**
 * 게임 상태 초기화
 */
void resetGameState(void);

#endif // QUIZ_ENGINE_H