//유저가 추가할려는 문제가 있을 때 그 문제들만 저장할 수 있는 txt파일 만들기.

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
                load_pending_questions();  // 1번 기능
                break;
            case 2:
                approve_or_reject_questions();  // 2번 기능
                break;
            case 3:
                edit_or_delete_questions();  // 3번 기능
                break;
            case 4:
                manage_users();  // 4번 기능
                break;
            case 0:
                printf("관리자 모드를 종료합니다.\n");
                return;
            default:
                printf("잘못된 입력입니다. 다시 선택해 주세요.\n");
        }
    }
}
//문제가 저장된 파일 열어보기
void load_pending_questions() {
    FILE *fp = fopen("pending_questions.txt", "r");
    char line[256];
    int count = 0;

    if (fp == NULL) {
        printf("제안된 문제가 없습니다.\n");
        return;
    }

    printf("\n--- 제안된 문제 목록 ---\n");

    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%d) %s", ++count, line);
    }

    if (count == 0) {
        printf("현재 제안된 문제가 없습니다.\n");
    }

    fclose(fp);
}