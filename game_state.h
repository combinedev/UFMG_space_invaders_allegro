//
// Created by hayde on 08/06/2025.
//

#ifndef OBJECTS_H
#define OBJECTS_H
#define WIN_W  800
#define WIN_H  600
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#define ACCEL  0.5f      // acceleration per frame
#define MAX_SPEED  6.0f // top speed
#define FRICTION  0.3f  // how fast it slows down
#define MAX_TILT  10.0f      // max angle in degrees
#define TILT_SPEED  2.5f     // how fast tilt angular changes per frame
#define alien_move_speed 45 // Pixels per move
#define alien_move_delay 500 // Frames/tics between moves


typedef struct Player {
    ALLEGRO_BITMAP *ship_bitmap;
    float x;
    float vx;
    float y;
    float angle;     // rotation angle in degrees
}Player;

typedef struct Alien {
    ALLEGRO_BITMAP *alien_bitmap[2];
    float x;
    float y;
    bool alive;
    int frame;
    char type;
}Alien;

typedef struct Projectile {
    ALLEGRO_BITMAP *projectile_bitmap;
    float x;
    float y;
    float vy;
    bool alive;
}Projectile;
typedef struct Explosion {
    ALLEGRO_BITMAP *explosion_bitmap;
    float x;
    float y;
    bool active;
    float start_time;
}Explosion;
void update_aliens(Alien matrix[5][10], bool *game_over ,Player *p1);
void update_player(bool *A_pressed, bool *D_pressed, Player *p1);
void update_projectile(Projectile *p, Alien matrix[5][10], Explosion *explosion, int *score,  ALLEGRO_SAMPLE * explosion_sfx);
bool check_collision(Projectile *proj, Alien *alien);
#endif //OBJECTS_H
