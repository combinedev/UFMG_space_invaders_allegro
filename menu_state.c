//
// Created by hayde on 15/06/2025.
//
#include "menu_state.h"
int setup_menu(Menu *menu, ALLEGRO_DISPLAY **display) {
    al_init_image_addon(); /* PNG/JPG */
    al_init_font_addon();    // Always needed
    al_init_ttf_addon();     // Required for TTF fonts
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(16);  // Reserve up to 16 simultaneous sounds

    menu->banner = al_load_bitmap("assets/banner.png");
    if (!menu->banner) {
        fprintf(stderr, "Could not load banner\n");
        return 1;
    }
    menu->font = al_load_ttf_font("assets/font.ttf", 24, 0);
    if (!menu->font) {
        fprintf(stderr, "Could not load font\n");
        return 1;
    }
    menu->menu_music = al_load_sample("assets/oasis.mp3");
    if (!menu->menu_music) {
        fprintf(stderr, "Could not load sfx\n");
        return 1;
    }
    menu->option_sfx = al_load_sample("assets/navigate.wav");
    if (!menu->option_sfx) {
        fprintf(stderr, "Could not load sfx\n");
        return 1;
    }
    sprintf(menu->title, "Space Invaders");
    sprintf(menu->options[0], "Play");
    sprintf(menu->options[1], "Exit");
    menu->selected = 0;

    menu->q = al_create_event_queue();
    menu->redraw = false;
    menu->menu_timer = al_create_timer(1.0 / FPS);
    al_register_event_source(menu->q, al_get_keyboard_event_source());
    al_register_event_source(menu->q, al_get_display_event_source(*display));
    al_register_event_source(menu->q, al_get_timer_event_source(menu->menu_timer));
    printf("Sucessfully loaded menu assets.\n");
    return 0;
}

void start_menu_queue(Menu *menu, State *current_state) {
    al_start_timer(menu->menu_timer);
    al_play_sample(menu->menu_music, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &menu->menu_music_id);
    menu->running = true;
    while (menu->running) {
        /* --- handle all waiting events first --- */
        while (!al_is_event_queue_empty(menu->q)) {
            ALLEGRO_EVENT ev;
            al_get_next_event(menu->q, &ev);
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                menu->running = false;
                *current_state = EXIT;
            if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_S:
                        if (menu->selected == 0) {
                            al_play_sample(menu->option_sfx, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                            menu->selected = 1;
                        }
                        break;
                    case ALLEGRO_KEY_W:
                        if (menu->selected == 1) {
                            al_play_sample(menu->option_sfx, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                            menu->selected = 0;
                        }
                        break;
                    case ALLEGRO_KEY_ESCAPE: menu->running = false; *current_state = EXIT;
                        break;
                    case ALLEGRO_KEY_ENTER:
                        process_entry(&menu->selected, &menu->running, &current_state, &menu->menu_music_id);
                        break;
                }
            }
            if (ev.timer.source == menu->menu_timer) {
                menu->redraw = true;
            }
            if (menu->redraw && al_is_event_queue_empty(menu->q)) {
                menu->redraw = false;
                draw_menu(menu);
            }
        }
    }
}

void draw_menu(Menu *menu) {
    al_clear_to_color(al_map_rgb(30, 30, 50)); /* midnight blue */
    int text_w = al_get_text_width(menu->font, menu->title);
    al_draw_text(menu->font, al_map_rgb(255, 255, 255), (WIN_W - text_w)/2, 100, 0, menu->title);

    int option_w = al_get_text_width(menu->font, menu->options[0]);
    if (menu->selected == 0) {
        al_draw_text(menu->font, (menu->selected == 0 ? al_map_rgb(125,192,121) : al_map_rgb(255,255,255)),
    (WIN_W - option_w)/2, 150, 0, menu->options[0]);
    }else {
        al_draw_text(menu->font, al_map_rgb(255, 255, 255), (WIN_W - option_w)/2, 150, 0, menu->options[0]);
    }
    if (menu->selected == 1) {
        option_w = al_get_text_width(menu->font, menu->options[1]);
        al_draw_text(menu->font, (menu->selected == 1 ? al_map_rgb(125,192,121) : al_map_rgb(255,255,255)),
            (WIN_W - option_w)/2, 200, 0, menu->options[1]);
    }else {
        option_w = al_get_text_width(menu->font, menu->options[1]);
        al_draw_text(menu->font, al_map_rgb(255, 255, 255), (WIN_W - option_w)/2, 200, 0, menu->options[1]);
    }
    if (menu->banner) {
        int banner_width = al_get_bitmap_width(menu->banner) * 0.4;
        int banner_height = al_get_bitmap_height(menu->banner) * 0.4;
        int banner_x = ((WIN_W - banner_width)/ 2) -10;
        int banner_y = 250;  // Position below menu options

        al_draw_scaled_bitmap(
            menu->banner,
            0, 0,  // Source x, y
            al_get_bitmap_width(menu->banner),  // Source width
            al_get_bitmap_height(menu->banner), // Source height
            banner_x, banner_y,  // Destination x, y
            banner_width, banner_height,  // Scaled width, height
            0  // Flags
        );
    }
    al_flip_display();

}

void process_entry(int *selected, bool *running, State **current_state, ALLEGRO_SAMPLE_ID *bg_music_id) {
    if (*selected == 0) {
        **current_state = GAME;
    }
    if (*selected == 1) {
        **current_state = EXIT;
    }
    al_stop_sample(bg_music_id);
    *running = false;
}

void destroy_menu(Menu *menu) {
    al_destroy_event_queue(menu->q);
    al_destroy_timer(menu->menu_timer);
    al_destroy_sample(menu->menu_music);
    al_destroy_sample(menu->option_sfx);
}