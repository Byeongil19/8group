#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "quiz_game.h"

// 퀴즈 파일에서 문제들을 로드하는 함수
int loadQuestionsFromFile(const char* filename, QuizQuestion* questions, int maxQuestions);

// 퀴즈 결과를 파일에 저장하는 함수
int saveQuizResult(const char* playerName, const char* category, int score, int totalQuestions, int timeTaken);

// 저장된 퀴즈 결과들을 화면에 표시하는 함수
void displaySavedResults(int maxResults);

// 결과 파일의 한 줄을 파싱하는 함수
int parseQuizData(const char* line, char* date, char* time, char* name, char* category, 
                  int* score, int* totalQuestions, int* timeTaken);

// 특정 카테고리의 결과만 표시하는 함수
void displayResultsByCategory(const char* category, int maxResults);

// 사용 가능한 퀴즈 파일 목록을 반환하는 함수
int getAvailableQuizFiles(char fileList[][50], int maxFiles);

// 카테고리명으로 파일명을 반환하는 함수
int getCategoryFilename(const char* category, char* filename);

#endif // FILE_MANAGER_H