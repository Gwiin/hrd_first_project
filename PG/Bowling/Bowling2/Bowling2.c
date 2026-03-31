#include "board.h"

static char board[7][50] = {
    "-------------------------------------------",
    "| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10  |",
    "-------------------------------------------",
    "| | | | | | | | | | | | | | | | | | | | | |",
    "-------------------------------------------",
    "|   |   |   |   |   |   |   |   |   |     |",
    "-------------------------------------------",
};

void initScore(Score * player, const char name[])
{
    strcpy(player->name, name);
    for (int i = 0; i < 12;++i){
        for (int j = 0; j < 3;j++){
            player->score[i][j] = 0;
        }
    }
}
void playGame(Score *player, int i)
{

    // 점수 입력 1cast
    printf("player : %s\n", player->name);
    printf("%d frame 1cast", i);
    scanf("%d", &player->score[i][1]);
    setScore(i, 1, player);

    // 점수 계산
    calScore(player->frameScore, player->score);
    printBoard(player);

    // 스패어 인가? -->
    if(((player->score[i-1][1])+(player->score[i-1][2])) == 10 && ((player->score[i-1][1] != 10)))
        setFrameScore(i - 1, player);
    printBoard(player);
    // 스트라이크 인가? , 스패어 처리를 했는가?
    if ((player->score[i-2][1] == 10) && (player->score[i-1][1] == 10))
        setFrameScore(i - 2, player);
    printBoard(player);

    if (player->score[i][1] == 10)
        return;

    // 점수 입력 2cast
    printf("player : %s\n", player->name);
    printf("%d frame 2cast", i);
    scanf("%d", &player->score[i][2]);
    setScore(i, 1, player);

    // 점수 계산
    calScore(player->frameScore, player->score);
    printBoard(player);

    // 합계가 10이였나 스패어처리 성공인가?
    if((player->score[i-1][1]) == 10){
        printf("%d frame: %d\n", i - 1, player->frameScore[i - 1]);
        setFrameScore(i - 1, player);
        printBoard(player);
    }
    // ..
    if ((player->score[i][1] + player->score[i][2]) < 10 && (i>0))
    {
        printf("%d frame: %d\n", i, player->frameScore[i]);
        setFrameScore(i, player);
        printBoard(player);
    }
    printBoard(player);
}
void playGame10Frame(Score *player)
{
    // 점수 입력 1cast
    printf("player : %s\n", player->name);
    printf("10 frame 1cast");
    scanf("%d", &player->score[10][1]);
    setScore(10, 1, player);

    // 점수 계산
    calScore(player->frameScore, player->score);
    printBoard(player);

    // 스트라이크 인가? , 스패어 처리를 했는가?
    if ((player->score[8][1] == 10) && (player->score[9][1] == 10)){
        setFrameScore(8, player);
        printBoard(player);
    }
    // 스패어 인가? -->
    if ((player->score[9][1] + player->score[9][2] == 10) && (player->score[9][1] != 10)){
        setFrameScore(9, player);
        printBoard(player);
    }

    // 점수 입력 2cast
    printf("player : %s\n", player->name);
    printf("10 frame 2cast");
    scanf("%d", &player->score[10][2]);
    setScore(10, 2, player);

    // 점수 계산
    calScore(player->frameScore, player->score);
    printBoard(player);
    fflush(stdout);

    // 합계가 10이였나 스패어처리 성공인가?
    if ((player->score[9][1]) == 10)
    {
        setFrameScore(9, player);
        printBoard(player);
    }
    // ..
    if ((player->score[10][1] + player->score[10][2]) < 10 )
    {
        setFrameScore(10, player);
        printBoard(player);
    }
    // 세번째 던질때
    if(player->score[10][1] == 10 || player->score[10][1]+player->score[10][2] == 10){
        printf("player : %s\n", player->name);
        printf("10 frame 3cast");
        scanf("%d", &player->score[10][3]);
        setScore(10, 3, player);
        // 점수 계산
        player->frameScore[10] = player->frameScore[9] + player->score[10][1] + player->score[10][2] + player->score[10][3];
        setFrameScore(10, player);
        printBoard(player);
    }
    printBoard(player);
}
void printBoard(Score *player)
{
    system("clear");
    printf("\nPlayer name: %s\n", player->name);
    for (int i = 0; i < 7;++i){
        printf("%s\n", board[i]);
    }
}

// board[][] 에 char 를 넣는 함수
void setScore(int frame, int bow, Score *player)
{
    // 초기화 스코어 지우기
    for (int i = 1; i <= frame;i++){
        for (int j = 1; j <= 2;j++){
            board[3][1 + (i - 1) * 4 + (j - 1) * 2] = ' ';
        }
    }
    // 초기화 10번째 스코어 지우기
    if (frame == 10 && bow == 1){
        board[3][1 + (frame - 1) * 4] = ' ';
        board[3][1 + (frame - 1) * 4 + 2] = ' ';
        board[3][1 + (frame - 1) * 4 + 4] = ' ';
    }
    for (int i = 1; i <= frame;++i){
        for (int j = 1; j <= 2;++j){
            if(i == frame && bow == 1 && j == 2 && i <= 9)
                break;
            // 스트라이크 일때
            if (player->score[i][j] == 10 && j == 1){
                board[3][1 + (i - 1) * 4 + (j - 1) * 2] = 'X';
            }else if (player->score[i][j] == 10 && i ==10 && j == 2){
                board[3][1 + (i - 1) * 4 + (j - 1) * 2] = 'X';
                // 스패어 처리가 됏을 때
            }else if (player->score[i][j]+player->score[i][j-1] == 10 && j == 2 && player->score[i][j-1] != 10){
                board[3][1 + (i - 1) * 4 + (j - 1) * 2] = '/';
                // 0~9 사이의 점수 일때
            }else if (player->score[i][j]+player->score[i][j-1]< 10){
                board[3][1 + (i - 1) * 4 + (j - 1) * 2] = '0' + player->score[i][j];
            }
        }
    }
    if(frame == 10 && bow ==3)
        if(player->score[10][3] == 10)
            board[3][1 + (10 - 1) * 4 + (3 - 1) * 2] = 'X';
        else
            board[3][1 + (10 - 1) * 4 + (3 - 1) * 2] = '0' + player->score[10][3];
}
void setFrameScore(int frame, Score *player)
{
    char buf[4];
    sprintf(buf, "%3d", player->frameScore[frame]);
    strncpy(board[5] + (frame - 1) * 4 + 1, buf, 3);
}
void calScore(int *frameScore, const int (*score)[3])
{
    // 초기화
    for (int i = 0; i < 11;++i)
        frameScore[i] = 0;
    for (int i = 1; i < 11;++i){
        frameScore[i] += frameScore[i - 1] + score[i][1] + score[i][2];
        // 스트라이크 보너스
        if (score[i][1] == 10){
            frameScore[i] += score[i + 1][1] + score[i + 1][2];
            if(score[i+1][1] == 10)
                frameScore[i] += score[i + 2][1];
        // 스패어 보너스
        }else if(score[i][1]+score[i][2] == 10){
            frameScore[i] += score[i + 1][1];
        }
    }
}