/* hello_sprite.c  –  gcc hello_sprite.c `pkg-config --cflags --libs allegro-5 allegro_image-5` */

#include <stdio.h>
#include "game_state.h"
#include "menu_state.h"
#include "gameover_state.h"

void set_window_icon(ALLEGRO_DISPLAY* display, const char* icon_path) {
    ALLEGRO_BITMAP* icon = al_load_bitmap(icon_path);
    if (icon) {
        ALLEGRO_BITMAP* converted_icon = al_clone_bitmap(icon);
        al_convert_bitmap(converted_icon);
        al_set_display_icon(display, converted_icon);
        al_destroy_bitmap(icon);
        al_destroy_bitmap(converted_icon);
    }
}

int main(void) {
    srand(time(NULL));
    /* 1. Boot the engine --------------------------------------------------- */
    if (!al_init()) return -1; /* core */
    al_install_keyboard(); /* input */
    /* 2. Open a window ----------------------------------------------------- */
    ALLEGRO_DISPLAY *display = al_create_display(WIN_W, WIN_H);
    al_init_image_addon();  // Required for PNG loading
    al_set_window_title(display, "Space Invaders Reloaded");
    set_window_icon(display, "assets/icon.png");
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
