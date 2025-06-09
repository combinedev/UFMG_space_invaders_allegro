//
// Created by hayde on 08/06/2025->
//
#include "game_state.h"

#include <stdio.h>

#include <limits.h> // for INT_MAX and INT_MIN

bool check_collision(Projectile *proj, Alien *alien) { //Explain AABB collision algorithm logic
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


void update_aliens(Alien matrix[5][10], bool *game_over, Player *p1) {
    static int direction = 1; // 1 = right, -1 = left
    const int COLS = 10;
    const int ROWS = 5;

    int leftmost_x = INT_MAX;
    int rightmost_x = INT_MIN;
    bool found = false;

    if (direction == -1) {
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
    if (direction == 1 && rightmost_x + alien_move_speed > WIN_W) {
        hit_border = true;
    } else if (direction == -1 && leftmost_x - alien_move_speed < 0) {
        hit_border = true;
    }

    if (hit_border) {
        direction = -direction;
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
                a->x += direction * alien_move_speed;
                // 1. Check ground collision
                if (matrix[r][c].y + al_get_bitmap_height(matrix[r][c].alien_bitmap[matrix[r][c].frame])*1.5 >= WIN_H) {
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
void update_projectile(Projectile *p, Alien matrix[5][10], Explosion *explosion, int *score) {
    p->vy = 0;
    p->vy -= ACCEL*8;
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
                matrix[i][j].alive = false;
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
                explosion->explosion_bitmap = al_load_bitmap("assets/enemy_explosion.png"); // load once or reuse
                explosion->start_time = al_get_time();
                explosion->active = true;
                p->alive = false;  // remove projectile
                break; // Optional: remove only one alien per projectile
            }
        }
    }


}
