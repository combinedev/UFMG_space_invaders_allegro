//
// Created by hayde on 15/06/2025.
//

#ifndef MENU_STATE_H
#define MENU_STATE_H
#include "stdafx.h"
typedef struct Menu {
    ALLEGRO_FONT *font;
    char title[40];
    char options[2][20];
    ALLEGRO_SAMPLE *menu_music;
    ALLEGRO_SAMPLE *option_sfx;
    ALLEGRO_EVENT_QUEUE *q;
    ALLEGRO_TIMER *menu_timer;
    ALLEGRO_SAMPLE_ID menu_music_id;
    bool running;
    bool redraw;
    int selected;
}Menu;
int setup_menu(Menu *menu, ALLEGRO_DISPLAY **display);
void start_menu_queue(Menu *menu, State *current_state);
void process_entry(int *selected, bool *running, State **current_state, ALLEGRO_SAMPLE_ID *bg_music_id);
void draw_menu(Menu *menu);
void destroy_menu(Menu *menu);
#endif //MENU_STATE_H
