#include <ui/ui.h>
#include <controller/controller.h>

#define SIDE_BAR_WIDTH 80.0f
#define SIDE_BAR_BUTTON_WIDTH 72.0f

Elios_Private Button change_mode_button = {0};
Elios_Private Button save_state_button = {0};
Elios_Private Button reload_state_button = {0};

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

Elios_Private void render_button(Button *button) {
	DrawRectangleRounded(button->dimentions, 0.4f, 4, button->color);
	IfTrue (is_mouse_over_button(button)) {
		button->color = GetColor(0xAAAAAAFF);
		IfTrue (is_mouse_clicking_button(button)) {
			button->color = GetColor(0x777777FF);
		}
	} else {
		button->color = GetColor(0xFFFFFFFF);
	}
	DrawText(button->textContent, button->dimentions.x + 5, button->dimentions.y + 5, 7, BLACK);
}

/**
 * Public functions ...
 */

Elios_Private void render_sections_bar() {
	Rectangle rect = {
		.x = 10.f,
		.y = 10.f,
		.width = SIDE_BAR_WIDTH,
		.height = (float) GetScreenHeight() - 20.f
	};
	DrawRectangleRec(rect, GetColor(0x77777799));

	render_button(&change_mode_button);
	render_button(&save_state_button);
	render_button(&reload_state_button);
}

Elios_Public void init_ui() {
	change_mode_button = (Button) {
		.dimentions = (Rectangle) {
			.x = 14.f,
			.y = 14.f,
			.width = SIDE_BAR_BUTTON_WIDTH,
			.height = 22.f
		},
		.callback = &change_mode_callback,
		.color = GetColor(0xFFFFFFFF),
		.textContent = "Change mode",
	};

	save_state_button = (Button) {
		.dimentions = (Rectangle) {
			.x = 14.f,
			.y = 40.f,
			.width = SIDE_BAR_BUTTON_WIDTH,
			.height = 22.f
		},
		.callback = &save_engine_state_callback,
		.color = GetColor(0xFFFFFFFF),
		.textContent = "Save state",
	};

	reload_state_button = (Button) {
		.dimentions = (Rectangle) {
			.x = 14.f,
			.y = 66.f,
			.width = SIDE_BAR_BUTTON_WIDTH,
			.height = 22.f
		},
		.callback = &reload_engine_state_callback,
		.color = GetColor(0xFFFFFFFF),
		.textContent = "Reload state",
	};
}

Elios_Public void render_ui() {
	render_sections_bar();
}

Elios_Public void update_ui() {
	IfTrue (is_mouse_clicking_button(&change_mode_button)) {
		change_mode_button.callback(NULL);
	}

	IfTrue (is_mouse_clicking_button(&save_state_button)) {
		save_state_button.callback(NULL);
	}

	IfTrue (is_mouse_clicking_button(&reload_state_button)) {
		reload_state_button.callback(NULL);
	}
}

Elios_Public void destroy_ui() {
	
}