/* hello_sprite.c  –  gcc hello_sprite.c `pkg-config --cflags --libs allegro-5 allegro_image-5` */

#include <stdio.h>
#include "game_state.h"
#define FPS 60
#define ALIEN_TIC_RATE 1.5  // Move aliens every 1.5 seconds
#define ANIM_OFFSET         0.3    // Time before move to switch sprite
#define ANIM_DURATION       1    // How long the animation frame lasts


int main(void) {
    /* 1. Boot the engine --------------------------------------------------- */
    if (!al_init()) return -1; /* core */
    al_install_keyboard(); /* input */
    al_init_image_addon(); /* PNG/JPG */
    ALLEGRO_FONT *font = NULL;
    al_init_font_addon();    // Always needed
    al_init_ttf_addon();     // Required for TTF fonts

    /* 2. Open a window ----------------------------------------------------- */
    ALLEGRO_DISPLAY *display = al_create_display(WIN_W, WIN_H);
    if (!display) return -1;

    /* 3. Load and initialize assets -------------------------------------------------- */
    font = al_load_ttf_font("assets/font.ttf", 24, 0);
    if (!font) {
        fprintf(stderr, "Could not load font\n");
        return 1;
    }
    int score = 0;
    char score_text[32];

    Explosion explosion;
    explosion.active = false;
    explosion.start_time = 0.0;

    Player p1;
    p1.ship_bitmap = al_load_bitmap("assets/player.png");
    if (!p1.ship_bitmap) {
        fprintf(stderr, "Failed to load assets/player.png, check the path\n");
        return 1;
    }
    p1.x = WIN_W / 2;
    p1.y = WIN_H -50;

    Projectile p_proj;
    p_proj.projectile_bitmap = al_load_bitmap("assets/player_projectile.png");
    if (!p1.ship_bitmap) {
        fprintf(stderr, "Failed to load assets/player_projectile.png, check the path\n");
        return 1;
    }

    float posx = 0, posy = 70;
    Alien matrix[5][10];
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            matrix[i][j].x = posx;
            matrix[i][j].y = posy;
            matrix[i][j].alive = true;
            matrix[i][j].frame = 0;
            posx +=45;
            if (i == 0){
                matrix[i][j].alien_bitmap[0] = al_load_bitmap("assets/space_A1.png");
                matrix[i][j].type = 'A';
                if (!matrix[i][j].alien_bitmap[0]) {
                    fprintf(stderr, "Failed to load assets/space_A1.png, check the path\n");
                    return 1;
                };
                matrix[i][j].alien_bitmap[1] = al_load_bitmap("assets/space_A2.png");
                if (!matrix[i][j].alien_bitmap[1]) {
                    fprintf(stderr, "Failed to load assets/space_A2.png, check the path\n");
                    return 1;
                };
            }else if (i >= 1 && i <= 2) {
                matrix[i][j].type = 'B';
                matrix[i][j].alien_bitmap[0] = al_load_bitmap("assets/space_B1.png");
                if (!matrix[i][j].alien_bitmap[0]) {
                    fprintf(stderr, "Failed to load assets/space_B1.png, check the path\n");
                    return 1;
                };
                matrix[i][j].alien_bitmap[1] = al_load_bitmap("assets/space_B2.png");
                if (!matrix[i][j].alien_bitmap[1]) {
                    fprintf(stderr, "Failed to load assets/space_B2.png, check the path\n");
                    return 1;
                };
            }else if (i>2){
                matrix[i][j].type = 'C';
                matrix[i][j].alien_bitmap[0] = al_load_bitmap("assets/space_C1.png");
                if (!matrix[i][j].alien_bitmap[0]) {
                    fprintf(stderr, "Failed to load assets/space_C1.png, check the path\n");
                    return 1;
                };
                matrix[i][j].alien_bitmap[1] = al_load_bitmap("assets/space_C2.png");
                if (!matrix[i][j].alien_bitmap[1]) {
                    fprintf(stderr, "Failed to load assets/space_C2.png, check the path\n");
                    return 1;
                };
            }
        }
        posx = 0;
        posy += 48;
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
                        if (!p_proj.alive) {
                            p_proj.alive = true;
                            p_proj.y = p1.y;
                            p_proj.x = p1.x + al_get_bitmap_width(p1.ship_bitmap)/2;
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
                    update_aliens(matrix, &game_over, &p1);  // Only runs every 0.5 seconds
                }
                else if (ev.timer.source == alien_anim_timer) {
                    // Switch all alive aliens to animation frame
                    for (int r = 0; r < 5; r++) {
                        for (int c = 0; c < 10; c++) {
                            if (matrix[r][c].alive)
                                matrix[r][c].frame = 1;
                        }
                    }
                    alien_animating = true;
                    anim_start_time = al_get_time();
                }
            }
            if (ev.timer.source == game_timer) {
                redraw = true;
                update_player(&A_pressed, &D_pressed, &p1);
                if (p_proj.alive) {
                    update_projectile(&p_proj, matrix, &explosion, &score);
                }
                if (explosion.active && al_get_time() - explosion.start_time >= 1) {
                    explosion.active = false;
                }
            }

        }
        //Update
        if (alien_animating && al_get_time() >= anim_start_time + ANIM_DURATION) {
            for (int r = 0; r < 5; r++) {
                for (int c = 0; c < 10; c++) {
                    if (matrix[r][c].alive)
                        matrix[r][c].frame = 0;
                }
            }
            alien_animating = false;
        }
        if (game_over) {
            running = false;
        }
        sprintf(score_text, "SCORE: %d points", score);

        //Draw the game state--------------------------------------------------------- */
        if (redraw && al_is_event_queue_empty(q)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(30, 30, 50)); /* midnight blue */
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 10, 0, score_text);
            int bmp_w = al_get_bitmap_width(p1.ship_bitmap);
            int bmp_h = al_get_bitmap_height(p1.ship_bitmap);
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 10; j++) {
                    if (matrix[i][j].alive) {
                        al_draw_scaled_bitmap(
                            matrix[i][j].alien_bitmap[matrix[i][j].frame],
                            0,
                            0,
                            al_get_bitmap_width(matrix[i][j].alien_bitmap[matrix[i][j].frame]),
                            al_get_bitmap_height(matrix[i][j].alien_bitmap[matrix[i][j].frame]),
                            matrix[i][j].x,
                            matrix[i][j].y,
                            al_get_bitmap_width(matrix[i][j].alien_bitmap[matrix[i][j].frame]) * 1.5,
                            al_get_bitmap_height(matrix[i][j].alien_bitmap[matrix[i][j].frame])*1.5, 0
                            );
                    }
                }
            }
            al_draw_tinted_scaled_rotated_bitmap(
                p1.ship_bitmap,
                al_map_rgb(255, 255, 255), // no tint
                bmp_w / 2, bmp_h / 2, // rotation center in source
                p1.x + bmp_w / 2, // screen position x
                p1.y + bmp_h / 2, // screen position y
                1.5, 1.5, // scale X, scale Y
                p1.angle * ALLEGRO_PI / 180.0f, // rotation in radians
                0 // no flags
            );
            if (explosion.active) {
                al_draw_scaled_bitmap(explosion.explosion_bitmap,
                    0,
                    0,
                    al_get_bitmap_width(explosion.explosion_bitmap),
                    al_get_bitmap_height(explosion.explosion_bitmap),
                    explosion.x,
                    explosion.y,
                    al_get_bitmap_width(explosion.explosion_bitmap)*1.5,
                    al_get_bitmap_height(explosion.explosion_bitmap)*1.5,
                    0);
            }
            if (p_proj.alive) {
                al_draw_scaled_bitmap(p_proj.projectile_bitmap,
                    0,
                    0,
                    al_get_bitmap_width(p_proj.projectile_bitmap),
                    al_get_bitmap_height(p_proj.projectile_bitmap),
                    p_proj.x,
                    p_proj.y,
                    al_get_bitmap_width(p_proj.projectile_bitmap)*3,
                    al_get_bitmap_height(p_proj.projectile_bitmap)*3,
                    0);
            }

            al_flip_display();
        }
    }

    /* 5. Shutdown ---------------------------------------------------------- */
    al_destroy_bitmap(p1.ship_bitmap);
    al_destroy_display(display);
    al_destroy_event_queue(q);
    return 0;
}
