//
// Created by hayde on 15/06/2025.
//

#ifndef STDAFX_H
#define STDAFX_H
#define FPS 50
#define WIN_W  800
#define WIN_H  600
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
typedef enum State {
    MENU,
    GAME,
    GAMEOVER,
    EXIT
}State;
#endif //STDAFX_H
