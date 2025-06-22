#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 256

void load_pending_questions();
void approve_or_reject_questions();
void edit_or_delete_questions();
void manage_users(); // 아직 기본 구조만 제공

// 관리자 모드 메뉴
void admin_mode() {
    int choice;

    while (1) {
        printf("\n====== 관리자 모드 ======\n");
        printf("1. 제안된 문제 확인\n");
        printf("2. 문제 승인 / 거절\n");
        printf("3. 기존 문제 수정 / 삭제\n");
        printf("4. 플레이어 유저 관리\n");
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
            case 4:
                manage_users();
                break;
            case 0:
                printf("관리자 모드를 종료합니다.\n");
                return;
            default:
                printf("잘못된 입력입니다. 다시 선택해 주세요.\n");
        }
    }
}

// 1) 제안된 문제 보기
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
// 2) 승인/거절
void approve_or_reject_questions() {
    FILE *in = fopen("pending_questions.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    FILE *approved = fopen("questions.txt", "a");

    if (!in || !temp || !approved) {
        printf("파일 열기에 실패했습니다.\n");
        return;
    }

    char line[MAX_LINE];
    int count = 1;
    while (fgets(line, sizeof(line), in)) {
        printf("\n문제 %d: %s", count++, line);
        printf("이 문제를 승인하시겠습니까? (y/n): ");
        char ch;
        getchar(); // 개행 제거
        scanf("%c", &ch);
        if (ch == 'y' || ch == 'Y') {
            fputs(line, approved);  // 승인 → questions.txt로
        } else {
            fputs(line, temp);      // 거절 → 임시로 유지
        }
    }

    fclose(in);
    fclose(temp);
    fclose(approved);

    remove("pending_questions.txt");
    rename("temp.txt", "pending_questions.txt");

    printf("\n처리가 완료되었습니다.\n");
}

// 3) 기존 문제 수정 및 삭제
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
    getchar(); // 개행 제거

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

// 4) 유저 관리 (기본 구조)
void manage_users() {
    printf("\n[유저 관리 기능은 아직 구현되지 않았습니다.]\n");
}
