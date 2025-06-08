/* hello_sprite.c  –  gcc hello_sprite.c `pkg-config --cflags --libs allegro-5 allegro_image-5` */

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#define WIN_W  800
#define WIN_H  600
#define SPEED  4        /* pixels per frame */

int main(void) {
    /* 1. Boot the engine --------------------------------------------------- */
    if (!al_init()) return -1; /* core */
    al_install_keyboard(); /* input */
    al_init_image_addon(); /* PNG/JPG */

    /* 2. Open a window ----------------------------------------------------- */
    ALLEGRO_DISPLAY *display = al_create_display(WIN_W, WIN_H);
    if (!display) return -1;

    /* 3. Load your sprite -------------------------------------------------- */
    ALLEGRO_BITMAP *ship = al_load_bitmap("assets/player.png"); /* 64×64, for example */
    if (!ship) return -1;

    float x = WIN_W / 2.0f; /* start middle */
    float y = WIN_H - 100 ;

    /* 4. Prepare event queue ---------------------------------------------- */
    ALLEGRO_EVENT_QUEUE *q = al_create_event_queue();
    al_register_event_source(q, al_get_keyboard_event_source());
    al_register_event_source(q, al_get_display_event_source(display));

    bool running = true;
    bool A_pressed = false;
    bool D_pressed = false;
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
                }
            }
            if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                switch (ev.keyboard.keycode) {
                    case ALLEGRO_KEY_A:  A_pressed = false; break;
                    case ALLEGRO_KEY_D: D_pressed = false; break;
                }
            }
        }
        //Update the game state--------------------------------------------------------- */
        if (A_pressed)  x -= SPEED;
        if (D_pressed) x += SPEED;
        //Draw the game state--------------------------------------------------------- */
        al_clear_to_color(al_map_rgb(30, 30, 50)); /* midnight blue */
        al_draw_bitmap(ship, x, y, 0);
        al_flip_display();
        al_rest(1.0 / 60); /* 60 FPS cap */
    }

    /* 5. Shutdown ---------------------------------------------------------- */
    al_destroy_bitmap(ship);
    al_destroy_display(display);
    al_destroy_event_queue(q);
    return 0;
}
