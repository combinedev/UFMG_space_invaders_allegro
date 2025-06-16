//
// Created by hayde on 15/06/2025.
//

#ifndef GAMEOVER_STATE_H
#define GAMEOVER_STATE_H
#include "stdafx.h"

#endif //GAMEOVER_STATE_H
typedef struct GameOver {
    ALLEGRO_FONT *font;
    ALLEGRO_EVENT_QUEUE *q;
    ALLEGRO_TIMER *timer;
    ALLEGRO_SAMPLE *gameover_music;
    ALLEGRO_SAMPLE *option_sfx;
    ALLEGRO_SAMPLE_ID gameover_music_id;
    bool running;
    char str[20];
    char score_state[64];  // ← Add this for the status message
    bool redraw;
    int selected;
    int final_score;
    int high_score;
} GameOver;

int setup_gameover(GameOver *gameover, ALLEGRO_DISPLAY **display, int final_score, bool *won);
void start_gameover_queue(GameOver *gameover, State *current_state);
void draw_gameover(GameOver *gameover);
void destroy_gameover(GameOver *gameover);
