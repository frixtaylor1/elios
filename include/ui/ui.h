#ifndef ui_H
#define ui_H

#include <elios.h>
#include <raylib/raylib.h>

typedef void (*ButtonClickCallback)(void *);

typedef struct Button {
	Rectangle           dimentions;
	ButtonClickCallback callback;
	Color               color;
} Button;

Elios_Public void init_ui();
Elios_Public void render_ui();
Elios_Public void update_ui();
Elios_Public void destroy_ui();

#endif // ui_H