#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H
#include "quiz_game.h"

int loadQuestionsFromFile(const char* filename, QuizQuestion* questions, int maxQuestions);
int saveQuizResult(const char* playerName, const char* category, int score, int totalQuestions, int timeTaken);
void displaySavedResults(int maxResults);
int parseQuizData(const char* line, char* date, char* time, char* name, char* category, int* score, int* totalQuestions, int* timeTaken);
void displayResultsByCategory(const char* category, int maxResults);
int getAvailableQuizFiles(char fileList[][50], int maxFiles);
int getCategoryFilename(const char* category, char* filename);
void shuffleQuestions(QuizQuestion* questions, int count);
#endif // FILE_MANAGER_H