/* hello_sprite.c  –  gcc hello_sprite.c `pkg-config --cflags --libs allegro-5 allegro_image-5` */

#include <stdio.h>
#include "game_state.h"
#include "menu_state.h"
#include "gameover_state.h"

int main(void) {
    /* 1. Boot the engine --------------------------------------------------- */
    if (!al_init()) return -1; /* core */
    al_install_keyboard(); /* input */
    /* 2. Open a window ----------------------------------------------------- */
    ALLEGRO_DISPLAY *display = al_create_display(WIN_W, WIN_H);
    if (!display) return -1;
    Game game;

    Menu menu;
    if (setup_menu(&menu, &display) == 1) {
        return 1;
    }
    GameOver gameover;

    /* 3. Load and initialize states -------------------------------------------------- */
    //Simple state machine thing
    State current_state = MENU;
    while (true) {
        switch (current_state) {
            case MENU:
                start_menu_queue(&menu, &current_state);
                destroy_menu(&menu);
                break;

            case GAME:
                if (setup_game(&game, &display) == 1){//found an error;
                    return 1;
                }
                start_game_queue(&game, &current_state);
                destroy_game(&game);
                break;

            case GAMEOVER:
                if (setup_gameover(&gameover, &display, game.score, &game.won) == 1) {
                    return 1;
                };
                start_gameover_queue(&gameover, &current_state);
                destroy_gameover(&gameover);
                break;

            case EXIT:
                al_destroy_display(display);
                return 0;
        }
    }

    /* 5. Shutdown ---------------------------------------------------------- */
    return 0;
}
