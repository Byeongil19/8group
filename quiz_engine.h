#ifndef QUIZ_ENGINE_H
#define QUIZ_ENGINE_H

typedef struct Quiz_ {
    char question[256];
    char answer;
    char useranswer;
    int score;
    struct Quiz_ *next;
} Quiz;

int timeout_60s(char* userAnswer);
void runQuiz(Quiz* head);
void shuffleLinkedList(Quiz** head, int n);
#endif 