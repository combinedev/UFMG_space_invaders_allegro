//
// Created by hayde on 08/06/2025.
//
#ifndef GAME_STATE_H
#define GAME_STATE_H
#include "stdafx.h"
#define ACCEL  0.5f      // acceleration per frame
#define MAX_SPEED  6.0f // top speed
#define FRICTION  0.3f  // how fast it slows down
#define MAX_TILT  10.0f      // max angle in degrees
#define TILT_SPEED  2.5f     // how fast tilt angular changes per frame
#define alien_move_speed 45 // Pixels per move
#define alien_move_delay 500 // Frames/tics between moves
#define ALIEN_TIC_RATE 1.5  // Move aliens every 1.5 seconds
#define ANIM_OFFSET         0.3    // Time before move to switch sprite
#define ANIM_DURATION       1    // How long the animation frame lasts
#define MAX_ALIEN_PROJECTILES 20  // Supports many bullets on screen

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

typedef struct UFO {
    ALLEGRO_BITMAP *ufo_bitmap;
    float x;
    float y;
    bool alive;
    float vx;
}UFO;

typedef struct Game {
    int alien_direction;
    ALLEGRO_FONT *font;
    int score;
    char score_text[32];
    UFO ufo;
    Explosion explosion;
    Player p1;
    Projectile p_proj;
    Projectile a_proj[MAX_ALIEN_PROJECTILES]; // Alien projectiles array
    ALLEGRO_BITMAP *alien_projectile_bitmap; // single shared bitmap
    ALLEGRO_TIMER *alien_proj_timer; // Timer for alien shooting
    Alien matrix[5][10];
    bool alien_animating;
    double anim_start_time;
    bool running, redraw;
    bool won;
    bool A_pressed;
    bool D_pressed;
    bool game_over;
    ALLEGRO_SAMPLE *shoot_sfx;
    ALLEGRO_SAMPLE *explosion_sfx;
    ALLEGRO_SAMPLE *bg_music;
    ALLEGRO_SAMPLE *alien_shoot_sfx;
    ALLEGRO_TIMER *game_timer;
    ALLEGRO_TIMER *alien_timer;
    ALLEGRO_TIMER *alien_anim_timer;
    ALLEGRO_EVENT_QUEUE *q;
}Game;

void start_game_queue(Game *game, State *current_state);
int setup_game(Game *game, ALLEGRO_DISPLAY **display);
void update_aliens(Alien matrix[5][10], bool *game_over, bool *won, Player *p1, int *direction);
void update_player(bool *A_pressed, bool *D_pressed, Player *p1);
void update_projectile(Projectile *p, Alien matrix[5][10], Explosion *explosion, int *score,
                       ALLEGRO_SAMPLE *explosion_sfx, Projectile a_proj[MAX_ALIEN_PROJECTILES], UFO *ufo);
bool check_collision(Projectile *proj, Alien *alien);
void draw_game(Game *game);
void update_ufo(UFO *ufo);
void alien_fire(Game *game);
void update_alien_projectiles(Game *game, Player *p1, bool *game_over);
void destroy_game(Game *game);
#endif //OBJECTS_H
