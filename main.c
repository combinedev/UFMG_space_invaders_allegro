/* hello_sprite.c  –  gcc hello_sprite.c `pkg-config --cflags --libs allegro-5 allegro_image-5` */

#include <stdio.h>
#include "game_state.h"

int main(void) {
    /* 1. Boot the engine --------------------------------------------------- */
    if (!al_init()) return -1; /* core */
    al_install_keyboard(); /* input */
    /* 2. Open a window ----------------------------------------------------- */
    ALLEGRO_DISPLAY *display = al_create_display(WIN_W, WIN_H);
    if (!display) return -1;

    /* 3. Load and initialize states -------------------------------------------------- */
    Game game;
    if (setup_game(&game, &display) == 1){//found an error;
        return 1;
    }
    start_game_queue(&game);

    /* 5. Shutdown ---------------------------------------------------------- */
    al_destroy_display(display);
    destroy_game(&game);
    return 0;
}
