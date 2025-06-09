//
// Created by hayde on 08/06/2025->
//
#include "game_state.h"

#include <stdio.h>

#include <limits.h> // for INT_MAX and INT_MIN

void update_aliens(Alien matrix[5][10]) {
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
    if (direction == 1 && rightmost_x + move_speed > WIN_W) {
        hit_border = true;
    } else if (direction == -1 && leftmost_x - move_speed < 0) {
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
                a->x += direction * move_speed;
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
