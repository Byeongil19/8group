#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 256

void load_pending_questions();
void approve_or_reject_questions();
void edit_or_delete_questions();
void approve_or_reject_questions() {
    FILE *in = fopen("pending_questions.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!in || !temp) {
        printf("파일 열기에 실패했습니다.\n");
        return;
    }
    char line[MAX_LINE];
    int count = 1;
    while (fgets(line, sizeof(line), in)) {
        char category[50], question[256], answer[100];
        char* token = strtok(line, "|");
        if (token) strcpy(category, token);
        token = strtok(NULL, "|");
        if (token) strcpy(question, token);
        token = strtok(NULL, "\n");
        if (token) strcpy(answer, token);
        printf("\n문제 %d:\n카테고리: %s\n질문: %s\n정답: %s\n", count++, category, question, answer);
        printf("이 문제를 승인하시겠습니까? (y/n): ");
        char ch;
        getchar();  
        scanf("%c", &ch);
        if (ch == 'y' || ch == 'Y') {
            char filename[100];
            if (strcmp(category, "개그") == 0)
                strcpy(filename, "quiz_joke.txt");
            else if (strcmp(category, "과학") == 0)
                strcpy(filename, "quiz_science.txt");
            else if (strcmp(category, "역사") == 0)
                strcpy(filename, "quiz_history.txt");
            else {
                printf("⚠️ 유효하지 않은 카테고리입니다. 이 문제는 저장되지 않습니다.\n");
                continue;
            }
            FILE* quiz_file = fopen(filename, "a");
            if (quiz_file) {
                fprintf(quiz_file, "\n%s | %s", question, answer); 
                fclose(quiz_file);
                printf("✅ 승인 완료: %s에 저장됨\n", filename);
            } else {
                printf("❌ 파일 저장 실패: %s\n", filename);
            }
        } else {
            fprintf(temp, "%s|%s|%s\n", category, question, answer); 
        }
    }
    fclose(in);
    fclose(temp);
    remove("pending_questions.txt");
    rename("temp.txt", "pending_questions.txt");
    printf("\n모든 제안 문제 처리가 완료되었습니다.\n");
}
void admin_mode() {
    int choice;
    while (1) {
        printf("\n====== 관리자 모드 ======\n");
        printf("1. 제안된 문제 확인\n");
        printf("2. 문제 승인 / 거절\n");
        printf("3. 기존 문제 수정 / 삭제\n");
        printf("0. 종료\n");
        printf("항목을 선택해 주세요: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                load_pending_questions();
                break;
            case 2:
                approve_or_reject_questions();
                break;
            case 3:
                edit_or_delete_questions();
                break;
            case 0:
                printf("관리자 모드를 종료합니다.\n");
                return;
            default:
                printf("잘못된 입력입니다. 다시 선택해 주세요.\n");
        }
    }
}
void load_pending_questions() {
    FILE *fp = fopen("pending_questions.txt", "r");
    char line[MAX_LINE];
    int count = 0;
    if (fp == NULL) {
        printf("제안된 문제가 없습니다.\n");
        return;
    }
    printf("\n--- 제안된 문제 목록 ---\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%d) %s", ++count, line);
    }
    if (count == 0) {
        printf("현재 제안된 문제가 없습니다.\n");
    }
    fclose(fp);
}
void edit_or_delete_questions() {
    FILE *in = fopen("questions.txt", "r");
    if (!in) {
        printf("기존 문제 파일이 없습니다.\n");
        return;
    }
    char questions[100][MAX_LINE];
    int count = 0;
    while (fgets(questions[count], MAX_LINE, in)) {
        printf("%d) %s", count + 1, questions[count]);
        count++;
    }
    fclose(in);
    if (count == 0) {
        printf("문제가 없습니다.\n");
        return;
    }
    int select;
    printf("수정/삭제할 문제 번호를 입력하세요 (0: 취소): ");
    scanf("%d", &select);
    if (select == 0 || select > count) return;
    printf("1: 수정, 2: 삭제 선택: ");
    int act;
    scanf("%d", &act);
    getchar();
    if (act == 1) {
        printf("새로운 문제를 입력하세요: ");
        fgets(questions[select - 1], MAX_LINE, stdin);
    } else if (act == 2) {
        for (int i = select - 1; i < count - 1; i++) {
            strcpy(questions[i], questions[i + 1]);
        }
        count--;
    }
    FILE *out = fopen("questions.txt", "w");
    for (int i = 0; i < count; i++) {
        fputs(questions[i], out);
    }
    fclose(out);
    printf("변경사항이 저장되었습니다.\n");
}