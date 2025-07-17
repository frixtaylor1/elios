#include <ui/ui.h>
#include <controller/controller.h>

Elios_Private Button change_mode_button = {0};

Elios_Private bool mouse_over_rect(const Rectangle *rect) {
	Rectangle mouse = (Rectangle) {
		.x = GetMousePosition().x,
		.y = GetMousePosition().y,
		.width = 4.f,
		.height = 4.f
	};
	return (CheckCollisionRecs(*rect, mouse));
}

Elios_Private bool is_mouse_over_button(const Button *button) {
	return mouse_over_rect(&button->dimentions);
}

Elios_Private bool is_mouse_clicking_button(const Button *button) {
	return (is_mouse_over_button(button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

/**
 * Public functions ...
 */

Elios_Private void render_sections_bar() {
	Rectangle rect = {
		.x = 10.f,
		.y = 10.f,
		.width = 30.f,
		.height = (float) GetScreenHeight() - 20.f
	};
	DrawRectangleRec(rect, GetColor(0x777777FF));
}


Elios_Public void init_ui() {
	change_mode_button = (Button) {
		.dimentions = (Rectangle) {
			.x = 14.f,
			.y = 14.f,
			.width = 22.f,
			.height = 22.f
		},
		.callback = &change_mode_callback,
		.color = GetColor(0xFFFFFFFF)
	};
}

Elios_Public void render_ui() {
	render_sections_bar();
	DrawRectangleRec(change_mode_button.dimentions, change_mode_button.color);
	IfTrue (is_mouse_over_button(&change_mode_button)) {
		change_mode_button.color = GetColor(0xAAAAAAFF);
	} else {
		change_mode_button.color = GetColor(0xFFFFFFFF);
	}
}

Elios_Public void update_ui() {
	IfTrue (is_mouse_clicking_button(&change_mode_button)) {
		change_mode_button.callback(NULL);
	}
}

Elios_Public void destroy_ui() {

}