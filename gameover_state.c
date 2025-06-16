//
// Created by hayde on 15/06/2025.
//
#include "gameover_state.h"

int setup_gameover(GameOver *gameover, ALLEGRO_DISPLAY **display, int final_score, bool *won) {
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(16);

    if (*won) {
        strcpy(gameover->str, "YOU WON!");
        gameover->gameover_music = al_load_sample("assets/win.mp3");
    } else {
        strcpy(gameover->str, "GAME OVER!");
        gameover->gameover_music = al_load_sample("assets/gameover.mp3");
    }
    gameover->font = al_load_ttf_font("assets/font.ttf", 24, 0);
    if (!gameover->font) {
        fprintf(stderr, "Could not load font\n");
    }
    if (!gameover->gameover_music) {
        fprintf(stderr, "Could not load gameover.mp3\n");
    }
    gameover->option_sfx = al_load_sample("assets/navigate.wav");
    if (!gameover->option_sfx) {
        fprintf(stderr, "Could not load navigate.wav\n");
    }
    gameover->timer = al_create_timer(1.0 / FPS);
    gameover->q = al_create_event_queue();

    gameover->final_score = final_score;
    gameover->high_score = 0;

    /*Reading high_score.txt, previous highscore*/
    FILE *f = fopen("highest_score.txt", "r");
    if (f == NULL) {
        // File doesn't exist — create it with current score
        f = fopen("highest_score.txt", "w");
        if (f) {
            //sucessfully opened the file
            fprintf(f, "%d", final_score);
            fclose(f);
            gameover->high_score = final_score;
            sprintf(gameover->score_state, "New high score!");
        } else {
            sprintf(gameover->score_state, "Could not create high score file.");
        }
    } else {
        // File exists — read previous high score
        if (fscanf(f, "%d", &gameover->high_score) == 1) {
            fclose(f);
            if (final_score > gameover->high_score) {
                f = fopen("highest_score.txt", "w");
                if (f) {
                    fprintf(f, "%d", final_score);
                    fclose(f);
                    gameover->high_score = final_score;
                    sprintf(gameover->score_state, "New high score!");
                } else {
                    sprintf(gameover->score_state, "Failed to update high score.");
                }
            } else {
                sprintf(gameover->score_state, "High score: %d", gameover->high_score);
            }
        } else {
            fclose(f);
            sprintf(gameover->score_state, "Invalid score file."); //nan error
        }
    }
    gameover->selected = 0;
    gameover->running = true;
    gameover->redraw = false;

    al_register_event_source(gameover->q, al_get_keyboard_event_source());
    al_register_event_source(gameover->q, al_get_display_event_source(*display));
    al_register_event_source(gameover->q, al_get_timer_event_source(gameover->timer));

    printf("Loaded GameOver assets.\n");
    return 0;
}

void start_gameover_queue(GameOver *gameover, State *current_state) {
    al_play_sample(gameover->gameover_music, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &gameover->gameover_music_id);;
    al_start_timer(gameover->timer);

    while (gameover->running) {
        while (!al_is_event_queue_empty(gameover->q)) {
            ALLEGRO_EVENT ev;
            al_get_next_event(gameover->q, &ev);

            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                *current_state = EXIT;
                gameover->running = false;
            }

            if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_W:
                    case ALLEGRO_KEY_S:
                        gameover->selected = !gameover->selected;
                        al_play_sample(gameover->option_sfx, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        break;
                    case ALLEGRO_KEY_ENTER:
                        if (gameover->selected == 0) {
                            // Play Again
                            *current_state = GAME;
                        } else {
                            // Exit
                            *current_state = EXIT;
                        }
                        gameover->running = false;
                        al_stop_sample(&gameover->gameover_music_id);
                        break;
                }
            }

            if (ev.type == ALLEGRO_EVENT_TIMER) {
                gameover->redraw = true;
            }

            if (gameover->redraw && al_is_event_queue_empty(gameover->q)) {
                gameover->redraw = false;
                draw_gameover(gameover);
            }
        }
    }
};

void draw_gameover(GameOver *gameover) {
    al_clear_to_color(al_map_rgb(0, 0, 0));

    char score_text[64];
    sprintf(score_text, "Your Score: %d", gameover->final_score);

    int title_w = al_get_text_width(gameover->font, gameover->str);
    if (strcmp(gameover->str, "YOU WON!") == 0) {
        al_draw_text(gameover->font, al_map_rgb(125, 192, 121), (WIN_W - title_w) / 2, 30, 0, gameover->str);
    } else {
        al_draw_text(gameover->font, al_map_rgb(255, 0, 0), (WIN_W - title_w) / 2, 30, 0, gameover->str);
    }

    int score_w = al_get_text_width(gameover->font, score_text);
    al_draw_text(gameover->font, al_map_rgb(255, 255, 255), (WIN_W - score_w) / 2, 70, 0, score_text);
    al_draw_text(gameover->font, al_map_rgb(200, 200, 255),
    (WIN_W - al_get_text_width(gameover->font, gameover->score_state)) / 2,
    110, 0, gameover->score_state);
    const char *options[2] = {"Play Again", "Exit"};
    for (int i = 0; i < 2; i++) {
        ALLEGRO_COLOR color = (gameover->selected == i) ? al_map_rgb(125, 192, 121) : al_map_rgb(255, 255, 255);
        int opt_w = al_get_text_width(gameover->font, options[i]);
        al_draw_text(gameover->font, color, (WIN_W - opt_w) / 2, 150 + i * 50, 0, options[i]);
    }



    al_flip_display();
};

void destroy_gameover(GameOver *gameover) {
    al_destroy_font(gameover->font);
    al_destroy_sample(gameover->gameover_music);
    al_destroy_sample(gameover->option_sfx);
    al_destroy_event_queue(gameover->q);
    al_destroy_timer(gameover->timer);
};
