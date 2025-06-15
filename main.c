/* hello_sprite.c  –  gcc hello_sprite.c `pkg-config --cflags --libs allegro-5 allegro_image-5` */

#include <stdio.h>
#include "game_state.h"
#define FPS 60
#define ALIEN_TIC_RATE 1.5  // Move aliens every 1.5 seconds
#define ANIM_OFFSET         0.3    // Time before move to switch sprite
#define ANIM_DURATION       1    // How long the animation frame lasts


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

    /* 3. Load and initialize assets -------------------------------------------------- */
    Game game;
    if (setup_game(&game.p1, &game.p_proj, game.matrix, &game.explosion,
    &game.shoot_sfx, &game.explosion_sfx, &game.bg_music, &game.font, &game.score) == 1){//found an error;
        return 1;
    }

    /* 4. Prepare event queue ---------------------------------------------- */
    ALLEGRO_TIMER *game_timer = al_create_timer(1.0 / FPS);
    ALLEGRO_TIMER *alien_timer = al_create_timer(ALIEN_TIC_RATE);
    ALLEGRO_TIMER *alien_anim_timer = al_create_timer(ALIEN_TIC_RATE - ANIM_OFFSET);
    ALLEGRO_EVENT_QUEUE *q = al_create_event_queue();
    al_register_event_source(q, al_get_keyboard_event_source());
    al_register_event_source(q, al_get_display_event_source(display));
    al_register_event_source(q, al_get_timer_event_source(game_timer));
    al_register_event_source(q, al_get_timer_event_source(alien_timer));
    al_register_event_source(q, al_get_timer_event_source(alien_anim_timer));
    al_register_event_source(q, al_get_display_event_source(display));

    // Start timers
    al_play_sample(game.bg_music, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
    al_start_timer(game_timer);
    al_start_timer(alien_timer);
    al_start_timer(alien_anim_timer);   /* ← start the animation timer */
    bool alien_animating = false;
    double anim_start_time = 0;
    bool running = true, redraw;
    bool A_pressed = false;
    bool D_pressed = false;
    bool game_over = false;

    while (running) {
        /* --- handle all waiting events first --- */
        while (!al_is_event_queue_empty(q)) {
            ALLEGRO_EVENT ev;
            al_get_next_event(q, &ev);
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                running = false;

            if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_A: A_pressed = true;
                        break;
                    case ALLEGRO_KEY_D: D_pressed = true;
                        break;
                        //close window
                    case ALLEGRO_KEY_ESCAPE: running = false;
                        break;
                    case ALLEGRO_KEY_SPACE:
                        if (!game.p_proj.alive) {
                            al_play_sample(game.shoot_sfx, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                            game.p_proj.alive = true;
                            game.p_proj.y = game.p1.y;
                            game.p_proj.x = game.p1.x + al_get_bitmap_width(game.p1.ship_bitmap)/2;
                            break;
                        }
                }
            }
            if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_A: A_pressed = false;
                        break;
                    case ALLEGRO_KEY_D: D_pressed = false;
                        break;
                }
            }
            if (ev.type == ALLEGRO_EVENT_TIMER) {
                if (ev.timer.source == alien_timer) {
                    update_aliens(game.matrix, &game_over, &game.p1);  // Only runs every 0.5 seconds
                }
                else if (ev.timer.source == alien_anim_timer) {
                    // Switch all alive aliens to animation frame
                    for (int r = 0; r < 5; r++) {
                        for (int c = 0; c < 10; c++) {
                            if (game.matrix[r][c].alive)
                                game.matrix[r][c].frame = 1;
                        }
                    }
                    alien_animating = true;
                    anim_start_time = al_get_time();
                }
            }
            if (ev.timer.source == game_timer) {
                redraw = true;
                update_player(&A_pressed, &D_pressed, &game.p1);
                if (game.p_proj.alive) {
                    update_projectile(&game.p_proj, game.matrix, &game.explosion, &game.score, game.explosion_sfx);
                }
                if (game.explosion.active && al_get_time() - game.explosion.start_time >= 1) {
                    game.explosion.active = false;
                }
            }

        }
        //Update
        if (alien_animating && al_get_time() >= anim_start_time + ANIM_DURATION) {
            for (int r = 0; r < 5; r++) {
                for (int c = 0; c < 10; c++) {
                    if (game.matrix[r][c].alive)
                        game.matrix[r][c].frame = 0;
                }
            }
            alien_animating = false;
        }
        if (game_over) {
            running = false;
        }
        sprintf(game.score_text, "SCORE: %d points", game.score);

        //Draw the game state--------------------------------------------------------- */
        if (redraw && al_is_event_queue_empty(q)) {
            redraw = false;
            draw_game(game.font, &game.p1, game.score_text, game.matrix, &game.p_proj, &game.explosion);
        }
    }

    /* 5. Shutdown ---------------------------------------------------------- */
    al_destroy_bitmap(game.p1.ship_bitmap);
    al_destroy_sample(game.shoot_sfx);
    al_destroy_display(display);
    al_destroy_event_queue(q);
    return 0;
}
