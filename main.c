/* hello_sprite.c  –  gcc hello_sprite.c `pkg-config --cflags --libs allegro-5 allegro_image-5` */

#include <stdio.h>
#include "game_state.h"

int main(void) {
    /* 1. Boot the engine --------------------------------------------------- */
    al_init_image_addon(); /* PNG/JPG */
    al_init_font_addon();    // Always needed
    al_init_ttf_addon();     // Required for TTF fonts
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(16);  // Reserve up to 16 simultaneous sounds
    if (!al_init()) return -1; /* core */
    al_install_keyboard(); /* input */
    /* 2. Open a window ----------------------------------------------------- */
    ALLEGRO_DISPLAY *display = al_create_display(WIN_W, WIN_H);
    if (!display) return -1;

    /* 3. Load and initialize states -------------------------------------------------- */
    Game game;
    if (setup_game(&game) == 1){//found an error;
        return 1;
    }
    start_game_queue(&game, &display);

    /* 5. Shutdown ---------------------------------------------------------- */
    al_destroy_bitmap(game.p1.ship_bitmap);
    al_destroy_sample(game.shoot_sfx);
    al_destroy_display(display);
    al_destroy_event_queue(game.q);
    return 0;
}
