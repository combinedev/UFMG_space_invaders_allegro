//
// Created by hayde on 08/06/2025->
//
#include "game_state.h"
#include <limits.h> // for INT_MAX and INT_MIN

void draw_game(Game *game) {
    al_clear_to_color(al_map_rgb(30, 30, 50)); /* midnight blue */
    int title_w = al_get_text_width(game->font, game->score_text);
    al_draw_text(game->font, al_map_rgb(255, 255, 255), (WIN_W - title_w) / 2, 10, 0, game->score_text);
    int bmp_w = al_get_bitmap_width(game->p1.ship_bitmap);
    int bmp_h = al_get_bitmap_height(game->p1.ship_bitmap);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            if (game->matrix[i][j].alive) {
                al_draw_scaled_bitmap(
                    game->matrix[i][j].alien_bitmap[game->matrix[i][j].frame],
                    0,
                    0,
                    al_get_bitmap_width(game->matrix[i][j].alien_bitmap[game->matrix[i][j].frame]),
                    al_get_bitmap_height(game->matrix[i][j].alien_bitmap[game->matrix[i][j].frame]),
                    game->matrix[i][j].x,
                    game->matrix[i][j].y,
                    al_get_bitmap_width(game->matrix[i][j].alien_bitmap[game->matrix[i][j].frame]) * 1.5,
                    al_get_bitmap_height(game->matrix[i][j].alien_bitmap[game->matrix[i][j].frame]) * 1.5, 0
                );
            }
        }
    }
    al_draw_tinted_scaled_rotated_bitmap(
        game->p1.ship_bitmap,
        al_map_rgb(255, 255, 255), // no tint
        bmp_w / 2, bmp_h / 2, // rotation center in source
        game->p1.x + bmp_w / 2, // screen position x
        game->p1.y + bmp_h / 2, // screen position y
        1.5, 1.5, // scale X, scale Y
        game->p1.angle * ALLEGRO_PI / 180.0f, // rotation in radians
        0 // no flags
    );
    if (game->explosion.active) {
        al_draw_scaled_bitmap(game->explosion.explosion_bitmap,
                              0,
                              0,
                              al_get_bitmap_width(game->explosion.explosion_bitmap),
                              al_get_bitmap_height(game->explosion.explosion_bitmap),
                              game->explosion.x,
                              game->explosion.y,
                              al_get_bitmap_width(game->explosion.explosion_bitmap) * 1.5,
                              al_get_bitmap_height(game->explosion.explosion_bitmap) * 1.5,
                              0);
    }
    if (game->p_proj.alive) {
        al_draw_scaled_bitmap(game->p_proj.projectile_bitmap,
                              0,
                              0,
                              al_get_bitmap_width(game->p_proj.projectile_bitmap),
                              al_get_bitmap_height(game->p_proj.projectile_bitmap),
                              game->p_proj.x,
                              game->p_proj.y,
                              al_get_bitmap_width(game->p_proj.projectile_bitmap) * 3,
                              al_get_bitmap_height(game->p_proj.projectile_bitmap) * 3,
                              0);
    }
    for (int i = 0; i < MAX_ALIEN_PROJECTILES; i++) {
        if (game->a_proj[i].alive) {
            al_draw_tinted_scaled_bitmap(
                game->a_proj[i].projectile_bitmap,
                al_map_rgb(100, 255, 100), // ← Light green tint
                0, 0,
                al_get_bitmap_width(game->a_proj[i].projectile_bitmap),
                al_get_bitmap_height(game->a_proj[i].projectile_bitmap),
                game->a_proj[i].x,
                game->a_proj[i].y,
                al_get_bitmap_width(game->a_proj[i].projectile_bitmap) * 2.5,
                al_get_bitmap_height(game->a_proj[i].projectile_bitmap) * 2.5,
                0
            );
        }
    }
    if (game->ufo.alive) {
        al_draw_bitmap(game->ufo.ufo_bitmap,game->ufo.x,game->ufo.y,0);
    }
    //shields
    // Draw shields
    for (int i = 0; i < 4; i++) {
        if (game->shields[i].alive) {
            al_draw_scaled_bitmap(game->shields[i].bitmap[game->shields[i].frame], 0, 0,
                                  al_get_bitmap_width(game->shields[i].bitmap[game->shields[i].frame]),
                                  al_get_bitmap_height(game->shields[i].bitmap[game->shields[i].frame]),
                                  game->shields[i].x,
                                  game->shields[i].y,
                                  al_get_bitmap_width(game->shields[i].bitmap[game->shields[i].frame]) * 2.5,
                                  al_get_bitmap_height(game->shields[i].bitmap[game->shields[i].frame]) * 2.5, 0);
        }
    }
    //draw ui
    float x_spacer = 0;
    for (int i = 0; i < game->p1.lives; i++) {
        al_draw_tinted_bitmap(game->ui.lives[i],al_map_rgb(100, 255, 100),game->ui.x + x_spacer,game->ui.y,0);
        x_spacer += al_get_bitmap_width(game->ui.lives[i]) * 1.5;
    }
    al_flip_display();
}

bool check_collision(Projectile *proj, Alien *alien) {
    //Explain AABB collision algorithm logic
    if (!proj->alive || !alien->alive)
        return false;

    // Get scaled projectile dimensions
    int proj_w = al_get_bitmap_width(proj->projectile_bitmap) * 3.0;
    int proj_h = al_get_bitmap_height(proj->projectile_bitmap) * 3.0;

    // Get scaled alien dimensions
    int alien_w = al_get_bitmap_width(alien->alien_bitmap[alien->frame]) * 1.5;
    int alien_h = al_get_bitmap_height(alien->alien_bitmap[alien->frame]) * 1.5;

    // Bounding box test (AABB)
    return (proj->x < alien->x + alien_w &&
            proj->x + proj_w > alien->x &&
            proj->y < alien->y + alien_h &&
            proj->y + proj_h > alien->y);
}
bool check_projectile_shield_collision(Projectile *proj, Shield *shield) {
    if (!proj->alive || !shield->alive) return false;

    float proj_w = al_get_bitmap_width(proj->projectile_bitmap) * 2.5f;
    float proj_h = al_get_bitmap_height(proj->projectile_bitmap) * 2.5f;
    float shield_w = al_get_bitmap_width(shield->bitmap[0]) * 2.5;
    float shield_h = al_get_bitmap_height(shield->bitmap[0]) *2.5;

    return (proj->x < shield->x + shield_w &&
            proj->x + proj_w > shield->x &&
            proj->y < shield->y + shield_h &&
            proj->y + proj_h > shield->y);
}
void update_alien_projectiles(Game *game, Player *p1, bool *game_over) {
    for (int i = 0; i < MAX_ALIEN_PROJECTILES; i++) {
        if (game->a_proj[i].alive) {
            game->a_proj[i].y += game->a_proj[i].vy;
            // Check for bottom of screen
            if (game->a_proj[i].y > WIN_H) {
                game->a_proj[i].alive = false;
            }
            // Shield collision
            for (int s = 0; s < 4; s++) {
                if (game->shields[s].alive &&
                    check_projectile_shield_collision(&game->a_proj[i], &game->shields[s])){
                    game->a_proj[i].alive = false;
                    game->shields[s].life--;
                    game->shields[s].frame++;
                    if (game->shields[s].life <= 0) {
                        game->shields[s].alive = false;
                    }
                        break;
                    }
            }
            // Check collision with player (AABB)
            float proj_w = al_get_bitmap_width(game->a_proj[i].projectile_bitmap) * 2.5;
            float proj_h = al_get_bitmap_height(game->a_proj[i].projectile_bitmap) * 2.5;

            float player_w = al_get_bitmap_width(p1->ship_bitmap) * 1.5;
            float player_h = al_get_bitmap_height(p1->ship_bitmap) * 1.5;

            if (
                game->a_proj[i].x < p1->x + player_w &&
                game->a_proj[i].x + proj_w > p1->x &&
                game->a_proj[i].y < p1->y + player_h &&
                game->a_proj[i].y + proj_h > p1->y
            ) {
                game->a_proj[i].alive = false;
                p1->lives--;  // Decrement life count

                // Spawn explosion at player position
                game->explosion.x = p1->x;
                game->explosion.y = p1->y;
                game->explosion.start_time = al_get_time();
                game->explosion.active = true;
                al_play_sample(game->hit_sfx, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

                // Only game over when lives reach 0
                if (p1->lives <= 0) {
                    *game_over = true;
                } else {
                    // Reset player position after death
                    p1->x = WIN_W / 2;
                    p1->y = WIN_H - 60;
                    p1->angle = 0;
                }
            }
        }
    }
}

void update_aliens(Alien matrix[5][10], bool *game_over, bool *won, Player *p1, int *direction) {
    const int COLS = 10;
    const int ROWS = 5;

    int leftmost_x = INT_MAX;
    int rightmost_x = INT_MIN;
    bool found = false;

    if (*direction == -1) {
        // Look for the first alive alien from the left
        for (int c = 0; c < COLS && !found; ++c) {
            for (int r = 0; r < ROWS; ++r) {
                if (matrix[r][c].alive) {
                    leftmost_x = matrix[r][c].x;
                    found = true;
                    break;
                }
            }
        }
    } else {
        // Look for the first alive alien from the right
        for (int c = COLS - 1; c >= 0 && !found; --c) {
            for (int r = 0; r < ROWS; ++r) {
                if (matrix[r][c].alive) {
                    rightmost_x = matrix[r][c].x +
                                  al_get_bitmap_width(matrix[r][c].alien_bitmap[matrix[r][c].frame]);
                    found = true;
                    break;
                }
            }
        }
    }

    // Border collision detection
    bool hit_border = false;
    if (*direction == 1 && rightmost_x + alien_move_speed > WIN_W) {
        hit_border = true;
    } else if (*direction == -1 && leftmost_x - alien_move_speed < 0) {
        hit_border = true;
    }

    if (hit_border) {
        *direction = -*direction;
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                Alien *a = &matrix[r][c];
                if (!a->alive) continue;
                a->y += al_get_bitmap_height(a->alien_bitmap[a->frame]);
            }
        }
    } else {
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                Alien *a = &matrix[r][c];
                if (!a->alive) continue;
                a->x += *direction * alien_move_speed;
                // 1. Check ground collision
                if (matrix[r][c].y + al_get_bitmap_height(matrix[r][c].alien_bitmap[matrix[r][c].frame]) * 1.5 >=
                    WIN_H) {
                    *game_over = true;
                    return;
                }
                // 2. Check player collision (AABB)
                float scale = 1.5f;

                float alien_w = al_get_bitmap_width(matrix[r][c].alien_bitmap[matrix[r][c].frame]) * scale;
                float alien_h = al_get_bitmap_height(matrix[r][c].alien_bitmap[matrix[r][c].frame]) * scale;

                float player_w = al_get_bitmap_width(p1->ship_bitmap) * scale;
                float player_h = al_get_bitmap_height(p1->ship_bitmap) * scale;

                bool collision_with_player =
                        matrix[r][c].x < p1->x + player_w &&
                        matrix[r][c].x + alien_w > p1->x &&
                        matrix[r][c].y < p1->y + player_h &&
                        matrix[r][c].y + alien_h > p1->y;

                if (collision_with_player) {
                    *game_over = true;
                    return;
                }
            }
        }
    }
    bool all_dead = true;
    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 10; c++) {
            if (matrix[r][c].alive) {
                all_dead = false;
                break;
            }
        }
        if (!all_dead) break;
    }
    if (all_dead) {
        *won = true;
        *game_over = true;
    }
}


void update_player(bool *A_pressed, bool *D_pressed, Player *p1) {
    //Update the game state--------------------------------------------------------- */

    /*Additional movement physics: acceleration and arrest-> The ship moves smoother*/
    /*Additional angular torque physics->*/
    if (*A_pressed) {
        p1->vx -= ACCEL;
        p1->angle -= TILT_SPEED;
        if (p1->angle < -MAX_TILT)
            p1->angle = -MAX_TILT;
        if (p1->vx < -MAX_SPEED) //truncation
            p1->vx = -MAX_SPEED;
    } else if (*D_pressed) {
        p1->angle += TILT_SPEED;
        if (p1->angle > MAX_TILT)
            p1->angle = MAX_TILT;
        p1->vx += ACCEL;
        if (p1->vx > MAX_SPEED) //truncation
            p1->vx = MAX_SPEED;
    } else {
        // No keys pressed – apply friction
        if (p1->vx > 0) {
            //positive movement, negative friction -> vx = 0
            p1->vx -= FRICTION;
            if (p1->vx < 0) p1->vx = 0;
        } else if (p1->vx < 0) {
            //negative movement, positive friction -> vx = 0
            p1->vx += FRICTION;
            if (p1->vx > 0) p1->vx = 0;
        }
        //return back to 0 degrees
        if (p1->angle > 0) {
            p1->angle -= TILT_SPEED;
            if (p1->angle < 0) p1->angle = 0;
        } else if (p1->angle < 0) {
            p1->angle += TILT_SPEED;
            if (p1->angle > 0) p1->angle = 0;
        }
    }

    // Update position
    p1->x += p1->vx;

    //Boundaries
    if (p1->x < 0) {
        p1->x = 0;
        p1->vx = 0;
    }
    if (p1->x + al_get_bitmap_width(p1->ship_bitmap) > WIN_W) {
        p1->x = WIN_W - al_get_bitmap_width(p1->ship_bitmap);
        p1->vx = 0;
    }
}

void update_projectile(Projectile *p, Alien matrix[5][10], Explosion *explosion, int *score,
                       ALLEGRO_SAMPLE *explosion_sfx, Projectile a_proj[MAX_ALIEN_PROJECTILES], UFO *ufo) {
    p->vy = 0;
    p->vy -= ACCEL * 8;
    if (p->vy < -MAX_SPEED) //truncation
        p->vy = -MAX_SPEED;
    p->y += p->vy;
    //hit the border
    if (p->y < 0) {
        p->alive = false;
    }
    //hit alien
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            if (check_collision(p, &matrix[i][j])) {
                //alien hit -> explodes and dies
                matrix[i][j].alive = false;
                al_play_sample(explosion_sfx, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                switch (matrix[i][j].type) {
                    case 'A':
                        *score += 100;
                        break;
                    case 'B':
                        *score += 50;
                        break;
                    case 'C':
                        *score += 25;
                        break;
                }
                explosion->x = matrix[i][j].x;
                explosion->y = matrix[i][j].y;
                explosion->start_time = al_get_time();
                explosion->active = true;
                p->alive = false; // remove projectile
                break; // Optional: remove only one alien per projectile
            }
        }
    }
    //hit enemy projectile
    for (int i = 0; i < MAX_ALIEN_PROJECTILES; i++) {
        if (a_proj[i].alive) {
            float p_w = al_get_bitmap_width(p->projectile_bitmap) * 2.0; // Assuming player scale
            float p_h = al_get_bitmap_height(p->projectile_bitmap) * 2.0;

            float a_w = al_get_bitmap_width(a_proj[i].projectile_bitmap) * 2.5;
            float a_h = al_get_bitmap_height(a_proj[i].projectile_bitmap) * 2.5;

            if (
                p->x < a_proj[i].x + a_w &&
                p->x + p_w > a_proj[i].x &&
                p->y < a_proj[i].y + a_h &&
                p->y + p_h > a_proj[i].y
            ) {
                // Collision detected → Both projectiles die
                p->alive = false;
                a_proj[i].alive = false;

                // Spawn explosion at collision point
                explosion->x = (p->x + a_proj[i].x) / 2; // Midpoint between projectiles
                explosion->y = (p->y + a_proj[i].y) / 2;
                explosion->start_time = al_get_time();
                explosion->active = true;

                // Play explosion sound
                al_play_sample(explosion_sfx, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

                break; // No need to check further after hit
            }
        }
    }
    //hit ufo -> 1000 points
    if (ufo->alive) {
        float p_w = al_get_bitmap_width(p->projectile_bitmap) * 2.0; // Assuming player scale
        float p_h = al_get_bitmap_height(p->projectile_bitmap) * 2.0;
        float u_w = al_get_bitmap_width(ufo->ufo_bitmap);
        float u_h = al_get_bitmap_height(ufo->ufo_bitmap);
        if ( //AABB
            p->x < ufo->x + u_w &&
            p->x + p_w > ufo->x &&
            p->y < ufo->y + u_h &&
            p->y + p_h > ufo->y
        ) {
            // Collision detected → Both projectiles die
            *score += 1000;
            p->alive = false;
            ufo->alive = false;

            // Spawn explosion at collision point
            explosion->x = ufo->x; // Midpoint between projectiles
            explosion->y = ufo->y;
            explosion->start_time = al_get_time();
            explosion->active = true;
            // Play explosion sound
            al_play_sample(explosion_sfx, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
    }
}

int setup_game(Game *game, ALLEGRO_DISPLAY **display) {
    al_init_image_addon(); /* PNG/JPG */
    al_init_font_addon(); // Always needed
    al_init_ttf_addon(); // Required for TTF fonts
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(16); // Reserve up to 16 simultaneous sounds

    game->running = false;
    game->hit_sfx = al_load_sample("assets/hit.wav");
    game->shoot_sfx = al_load_sample("assets/shoot.wav");
    game->explosion_sfx = al_load_sample("assets/explosion.wav");
    game->bg_music = al_load_sample("assets/glitch.mp3");
    game->font = al_load_ttf_font("assets/font.ttf", 24, 0);

    game->alien_shoot_sfx = al_load_sample("assets/alien_shoot.wav");
    if (!game->alien_shoot_sfx) {
        fprintf(stderr, "Failed to load alien_shoot.wav\n");
        return 1;
    }

    if (!game->font) {
        fprintf(stderr, "Could not load font\n");
        return 1;
    }

    if (!game->shoot_sfx) {
        fprintf(stderr, "Failed to load sound effect!\n");
        return 1;
    }

    if (!game->hit_sfx) {
        fprintf(stderr, "Failed to load sound effect!\n");
    }

    if (!game->explosion_sfx) {
        fprintf(stderr, "Failed to load sound effect!\n");
        return 1;
    }

    if (!game->bg_music) {
        fprintf(stderr, "Failed to load background music.\n");
        return 1;
    }

    game->explosion.active = false;
    game->explosion.start_time = 0.0;
    game->score = 0;
    game->won = false;
    game->alien_direction = 1;

    game->alien_projectile_bitmap = al_load_bitmap("assets/enemy_projectile.png");
    if (!game->alien_projectile_bitmap) {
        fprintf(stderr, "Failed to load assets/alien_projectile.png\n");
        return 1;
    }
    for (int i = 0; i < MAX_ALIEN_PROJECTILES; i++) {
        game->a_proj[i].alive = false;
    }

    game->p1.ship_bitmap = al_load_bitmap("assets/player.png");
    if (!game->p1.ship_bitmap) {
        fprintf(stderr, "Failed to load assets/player.png, check the path\n");
        return 1;
    }
    game->p1.x = WIN_W / 2;
    game->p1.y = WIN_H - 60;
    game->p1.lives = 3; //starting lifes

    //UI setup
    for (int i = 0; i < 3; i++) {
        game->ui.lives[i] = al_load_bitmap("assets/player.png");
    }
    game->ui.y = WIN_H - 30;
    game->ui.x = 20;

    game->explosion.explosion_bitmap = al_load_bitmap("assets/enemy_explosion.png");
    if (!game->explosion.explosion_bitmap) {
        fprintf(stderr, "Failed to load explosion.\n");
        return 1;
    }

    //ufo setup
    game->ufo.ufo_bitmap = al_load_bitmap("assets/ufo.png");
    game->ufo.alive = false;
    game->ufo.vx = 3;
    game->ufo.x = 0 - al_get_bitmap_width(game->ufo.ufo_bitmap);
    game->ufo.y = 70;

    //shield setup
    // Load shield frames
    float shield_spacing = WIN_W / 5.0f; // 4 shields + 5 gaps
    for (int i = 0; i < 4; i++) {
        for (int frame = 0; frame < 9; frame++) {
            char path[64];
            sprintf(path, "assets/shield_%d.png", frame);
            game->shields[i].bitmap[frame] = al_load_bitmap(path);
            if (!game->shields[i].bitmap[frame]) {
                fprintf(stderr, "Failed to load shield frame %d\n", frame);
                return 1;
            }
        }
        game->shields[i].life = 8; // Full health
        game->shields[i].frame = 0;
        game->shields[i].alive = true;
        game->shields[i].x = shield_spacing * (i + 1) -
                             al_get_bitmap_width(game->shields[i].bitmap[0]) / 2;
        game->shields[i].y = WIN_H - 150; // Above player
    }

    //p_proj reset
    game->p_proj.alive = false;
    game->p_proj.vy = 0;
    game->p_proj.y = game->p1.y;
    game->p_proj.x = game->p1.x + al_get_bitmap_width(game->p1.ship_bitmap) / 2;
    game->p_proj.projectile_bitmap = al_load_bitmap("assets/player_projectile.png");
    if (!game->p_proj.projectile_bitmap) {
        fprintf(stderr, "Failed to load assets/player_projectile.png, check the path\n");
        return 1;
    }

    float posx = 0, posy = 100;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            game->matrix[i][j].x = posx;
            game->matrix[i][j].y = posy;
            game->matrix[i][j].alive = true;
            game->matrix[i][j].frame = 0;
            posx += 45;
            if (i == 0) {
                game->matrix[i][j].alien_bitmap[0] = al_load_bitmap("assets/space_A1.png");
                game->matrix[i][j].type = 'A';
                if (!game->matrix[i][j].alien_bitmap[0]) {
                    fprintf(stderr, "Failed to load assets/space_A1.png, check the path\n");
                    return 1;
                };
                game->matrix[i][j].alien_bitmap[1] = al_load_bitmap("assets/space_A2.png");
                if (!game->matrix[i][j].alien_bitmap[1]) {
                    fprintf(stderr, "Failed to load assets/space_A2.png, check the path\n");
                    return 1;
                };
            } else if (i >= 1 && i <= 2) {
                game->matrix[i][j].type = 'B';
                game->matrix[i][j].alien_bitmap[0] = al_load_bitmap("assets/space_B1.png");
                if (!game->matrix[i][j].alien_bitmap[0]) {
                    fprintf(stderr, "Failed to load assets/space_B1.png, check the path\n");
                    return 1;
                };
                game->matrix[i][j].alien_bitmap[1] = al_load_bitmap("assets/space_B2.png");
                if (!game->matrix[i][j].alien_bitmap[1]) {
                    fprintf(stderr, "Failed to load assets/space_B2.png, check the path\n");
                    return 1;
                };
            } else if (i > 2) {
                game->matrix[i][j].type = 'C';
                game->matrix[i][j].alien_bitmap[0] = al_load_bitmap("assets/space_C1.png");
                if (!game->matrix[i][j].alien_bitmap[0]) {
                    fprintf(stderr, "Failed to load assets/space_C1.png, check the path\n");
                    return 1;
                };
                game->matrix[i][j].alien_bitmap[1] = al_load_bitmap("assets/space_C2.png");
                if (!game->matrix[i][j].alien_bitmap[1]) {
                    fprintf(stderr, "Failed to load assets/space_C2.png, check the path\n");
                    return 1;
                };
            }
        }
        posx = 0;
        posy += 48;
    }

    /* 4. Prepare event queue ---------------------------------------------- */
    game->game_timer = al_create_timer(1.0 / FPS); //redraw upon fps rate
    game->alien_timer = al_create_timer(ALIEN_TIC_RATE);
    game->alien_anim_timer = al_create_timer(ALIEN_TIC_RATE - ANIM_OFFSET);
    game->alien_proj_timer = al_create_timer(1.5);
    game->q = al_create_event_queue();
    al_register_event_source(game->q, al_get_keyboard_event_source());
    al_register_event_source(game->q, al_get_display_event_source(*display));
    al_register_event_source(game->q, al_get_timer_event_source(game->game_timer));
    al_register_event_source(game->q, al_get_timer_event_source(game->alien_timer));
    al_register_event_source(game->q, al_get_timer_event_source(game->alien_anim_timer));
    al_register_event_source(game->q, al_get_timer_event_source(game->alien_proj_timer));

    printf("Sucessfully loaded game assets.\n");
    return 0;
}

void start_game_queue(Game *game, State *current_state) {
    // Start timers
    al_play_sample(game->bg_music, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
    al_start_timer(game->game_timer);
    al_start_timer(game->alien_timer);
    al_start_timer(game->alien_anim_timer); /* ← start the animation timer */
    al_start_timer(game->alien_proj_timer);
    game->alien_animating = false;
    game->anim_start_time = 0;
    game->running = true;
    game->redraw = false;
    game->A_pressed = false;
    game->D_pressed = false;
    game->game_over = false;

    while (game->running) {
        /* --- handle all waiting events first --- */
        while (!al_is_event_queue_empty(game->q)) {
            ALLEGRO_EVENT ev;
            al_get_next_event(game->q, &ev);
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                game->running = false;
            *current_state = EXIT;

            if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_A: game->A_pressed = true;
                        break;
                    case ALLEGRO_KEY_D: game->D_pressed = true;
                        break;
                    //close window
                    case ALLEGRO_KEY_ESCAPE: game->running = false;
                        *current_state = EXIT;
                        break;
                    case ALLEGRO_KEY_SPACE:
                        if (!game->p_proj.alive) {
                            al_play_sample(game->shoot_sfx, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                            game->p_proj.alive = true;
                            game->p_proj.y = game->p1.y;
                            game->p_proj.x = game->p1.x + al_get_bitmap_width(game->p1.ship_bitmap) / 2;
                            break;
                        }
                }
            }
            if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_A: game->A_pressed = false;
                        break;
                    case ALLEGRO_KEY_D: game->D_pressed = false;
                        break;
                }
            }
            if (ev.type == ALLEGRO_EVENT_TIMER) {
                if (ev.timer.source == game->alien_timer) {
                    update_aliens(game->matrix, &game->game_over, &game->won, &game->p1, &game->alien_direction);
                    // Only runs every 0.5 seconds
                } else if (ev.timer.source == game->alien_anim_timer) {
                    // Switch all alive aliens to animation frame
                    for (int r = 0; r < 5; r++) {
                        for (int c = 0; c < 10; c++) {
                            if (game->matrix[r][c].alive)
                                game->matrix[r][c].frame = 1;
                        }
                    }
                    game->alien_animating = true;
                    game->anim_start_time = al_get_time();
                }
                if (ev.timer.source == game->alien_proj_timer) {
                    alien_fire(game);
                }
                //Update game draw function according to events -> redraw
                if (ev.timer.source == game->game_timer) {
                    if (ev.timer.source == game->game_timer) {
                        update_player(&game->A_pressed, &game->D_pressed, &game->p1);
                        if (game->p_proj.alive) {
                            update_projectile(&game->p_proj, game->matrix, &game->explosion, &game->score,
                                              game->explosion_sfx, game->a_proj, &game->ufo);
                        }
                        if (game->explosion.active && al_get_time() - game->explosion.start_time >= 0.5) {
                            game->explosion.active = false;
                        }
                        update_ufo(&game->ufo);
                        update_alien_projectiles(game, &game->p1, &game->game_over);
                        game->redraw = true;
                    }
                }
            }

            //Update
            if (game->alien_animating && al_get_time() >= game->anim_start_time + ANIM_DURATION) {
                for (int r = 0; r < 5; r++) {
                    for (int c = 0; c < 10; c++) {
                        if (game->matrix[r][c].alive)
                            game->matrix[r][c].frame = 0;
                    }
                }
                game->alien_animating = false;
            }
            if (game->game_over) {
                *current_state = GAMEOVER;
                game->running = false;
            }
            sprintf(game->score_text, "SCORE: %d points", game->score);

            //Draw the game state--------------------------------------------------------- */
            if (game->redraw && al_is_event_queue_empty(game->q)) {
                game->redraw = false;
                draw_game(game);
            }
        }
    }
}

void destroy_game(Game *game) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            al_destroy_bitmap(game->matrix[i][j].alien_bitmap[0]);
            al_destroy_bitmap(game->matrix[i][j].alien_bitmap[1]);
        }
    }
    for (int i = 0; i < game->p1.lives; i++) {
        al_destroy_bitmap(game->ui.lives[i]);
    }
    al_destroy_timer(game->game_timer);
    al_destroy_timer(game->alien_timer);
    al_destroy_timer(game->alien_anim_timer);
    al_destroy_timer(game->alien_proj_timer);
    al_destroy_bitmap(game->p1.ship_bitmap);
    al_destroy_bitmap(game->p_proj.projectile_bitmap);
    al_destroy_bitmap(game->explosion.explosion_bitmap);
    al_destroy_sample(game->shoot_sfx);
    al_destroy_sample(game->explosion_sfx);
    al_destroy_sample(game->bg_music);
    al_destroy_sample(game->hit_sfx);
    al_destroy_sample(game->alien_shoot_sfx);
    al_destroy_bitmap(game->alien_projectile_bitmap);
    al_destroy_bitmap(game->ufo.ufo_bitmap);
    al_destroy_event_queue(game->q);
}

//Additional
void update_ufo(UFO *ufo) {
    if (ufo->alive) {
        ufo->x += ufo->vx;
        if (ufo->x >= WIN_W + al_get_bitmap_width(ufo->ufo_bitmap)) {
            ufo->alive = false;
            ufo->x = 0 - al_get_bitmap_width(ufo->ufo_bitmap);
        }
    } else {
        static float time_until_spawn = 0.0f; // Not const!
        static float timer = 0.0f;

        // Initialize time_until_spawn on first run
        if (timer == 0.0f) {
            time_until_spawn = 5.0f + (rand() % 10); // 5-15 sec initial delay
        }

        timer += 1.0f / 60.0f; // Assumes 60 FPS (adjust if needed)

        if (timer >= time_until_spawn) {
            ufo->alive = true;
            ufo->x = 0 - al_get_bitmap_width(ufo->ufo_bitmap);
            timer = 0.0f;
            time_until_spawn = 5.0f + (rand() % 10); // New random delay
        }
    }
}

void alien_fire(Game *game) {
    int alive_columns[10];
    int alive_col_count = 0;

    // Find columns with at least one alive alien, reverse row counting, from bottom to top
    for (int col = 0; col < 10; col++) {
        for (int row = 4; row >= 0; row--) {
            if (game->matrix[row][col].alive) {
                alive_columns[alive_col_count] = col;
                alive_col_count++;
                break;
            }
        }
    }

    if (alive_col_count == 0) return; //no alive aliens, abort

    int max_shots = (alive_col_count < 3) ? alive_col_count : 3;
    int shots_this_tick = (rand() % max_shots) + 1;
    /*If alive_col_count is less than 3, then max_shots = alive_col_count.
    Otherwise, max_shots = 3.*/

    int selected[3]; // Max 3 alive columns selections
    int selected_count = 0;

    while (selected_count < shots_this_tick) {
        int r = alive_columns[rand() % alive_col_count]; //grabs a random index up to 0 and  alive_col_count -1
        // Check if already selected
        bool already = false;
        for (int i = 0; i < selected_count; i++) {
            if (selected[i] == r) {
                already = true;
                break;
            }
        }
        if (!already) {
            selected[selected_count] = r;
            selected_count++;
        }
    }

    // Fire from selected columns
    for (int s = 0; s < selected_count; s++) {
        int col = selected[s];
        for (int row = 4; row >= 0; row--) {
            if (game->matrix[row][col].alive) {
                //only the last row alive alien shoots
                // Find a free projectile slot and register the new projectile
                for (int p = 0; p < MAX_ALIEN_PROJECTILES; p++) {
                    if (!game->a_proj[p].alive) {
                        //this centers up the a_proj to the matrix alien's center
                        game->a_proj[p].projectile_bitmap = game->alien_projectile_bitmap;
                        game->a_proj[p].x = game->matrix[row][col].x +
                                            al_get_bitmap_width(game->matrix[row][col].alien_bitmap[0]) * 0.75;
                        game->a_proj[p].y = game->matrix[row][col].y +
                                            al_get_bitmap_height(game->matrix[row][col].alien_bitmap[0]) * 1.5;
                        game->a_proj[p].vy = 4.0;
                        game->a_proj[p].alive = true;
                        // Play alien shoot sound
                        al_play_sample(game->alien_shoot_sfx, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        break;
                    }
                }
                break;
            }
        }
    }
}
