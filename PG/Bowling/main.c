#include "Bowling1.h"

int main() {
    Player player;
    int choice;

    while (1) {
        show_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                play_game(&player);
                break;
            case 0:
                return 0;
            default:
                printf("잘못된 선택입니다.\n");
        }
    }
}
