#include <ui/ui.h>
#include <controller/controller.h>
#include <string.h>
#include <ctype.h>
#include <raylib/raymath.h>

Elios_Private UIWindow     leftPanel;
Elios_Private UIWindow     rightPanel;
Elios_Private UIDevActions devActionsState;
Elios_Private int          selectedEntityId = NOT_AN_ENTITY;
Elios_Private bool         draggingTile     = false;
Elios_Private bool         clickingUI       = false;
Elios_Private Vector3      dragStartWorld   = {0};

Elios_Private Vector3 get_mouse_world_position_on_ground() {
    Ray ray = GetMouseRay(GetMousePosition(), *get_camera());
    float t = -ray.position.y / ray.direction.y;
    return Vector3Add(ray.position, Vector3Scale(ray.direction, t));
}

Elios_Public void ui_set_action(const UIDevActions action) {
    devActionsState = action;
}

Elios_Public UIDevActions ui_get_action() {
    return devActionsState;
}

Elios_Public void ui_set_selected_entity(const int32 entityId) {
    selectedEntityId = entityId;
}

Elios_Public bool ui_is_mouse_over_rect(const Rectangle *rec) {
    Rectangle mouse = { GetMousePosition().x, GetMousePosition().y, 4.f, 4.f };
    return CheckCollisionRecs(*rec, mouse);
}

Elios_Public bool ui_is_button_hovered(const UIButton *btn) {
    return ui_is_mouse_over_rect(&btn->rec);
}

Elios_Public bool ui_is_button_clicked(const UIButton *btn) {
    return ui_is_button_hovered(btn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

Elios_Public void ui_render_button(const UIButton *btn) {
    Color base = btn->color;
    IfTrue (ui_is_button_hovered(btn)) base = GetColor(0xAAAAAAFF);
    IfTrue (ui_is_button_clicked(btn)) base = GetColor(0x777777FF);

    DrawRectangleRounded(btn->rec, 0.4f, 4, base);
    DrawText(btn->text, btn->rec.x + 5, btn->rec.y + 5, FONT_SIZE, BLACK);
}

Elios_Public void ui_render_input(const UIInput *input) {
    Color base = input->isFocused ? GetColor(0xAAAAAAFF) : input->color;

    DrawRectangleRec(input->rec, GetColor(0x444444CC));
    DrawRectangleLinesEx(input->rec, 1, GetColor(0x888888FF));
    DrawText(input->text, input->rec.x + 5, input->rec.y + 5, FONT_SIZE, base);
}

Elios_Public void ui_render_window(UIWindow *win) {
    win->rec.height = win->toggled
        ? (win->nbButtons * (BUTTON_HEIGHT + BUTTON_SPACING) + BUTTON_PADDING)
        + (win->nbTexts * (FONT_SIZE + FONT_SPACING + FONT_PADDING))
        + (win->nbInputs * (INPUT_HEIGHT + INPUT_PADDING))
        : 20.f;

    DrawRectangleRec(win->rec, GetColor(0x444444CC));
    DrawRectangleLinesEx(win->rec, 1, GetColor(0x888888FF));
    DrawText(win->title, win->rec.x + 20, win->rec.y + 5, 8, WHITE);
    ui_render_button(&win->toggle);

    IfTrue (win->toggled) {
        ForRange (int32, i, 0, win->nbButtons) ui_render_button(&win->buttons[i]); EForRange;
        ForRange (int32, i, 0, win->nbTexts) {
            c_string text = win->texts[i].label.dynamicText ? win->texts[i].label.dynamicText() : win->texts[i].content.staticText;
            DrawText(win->texts[i].label.staticText, win->texts[i].label.pos.x, win->texts[i].label.pos.y, FONT_SIZE, win->texts[i].label.color);
            DrawText(text, win->texts[i].content.pos.x, win->texts[i].content.pos.y, FONT_SIZE, win->texts[i].content.color);
        } EForRange;
        ForRange (int32, i, 0, win->nbInputs) ui_render_input(&win->inputs[i]); EForRange;
    }
}

Elios_Public void ui_add_button_to_window(UIWindow *win, c_string text, ButtonClickCallback callback) {
    IfTrue (win->nbButtons >= MAX_UI_BUTTONS) return;

    float y = WINDOW_PADDING + FONT_PADDING + win->rec.y
            + (win->nbTexts * (FONT_SIZE + FONT_SPACING) + FONT_PADDING)
            + (win->nbButtons * (BUTTON_HEIGHT + BUTTON_SPACING));

    win->buttons[win->nbButtons++] = (UIButton){
        .rec = { win->rec.x + WINDOW_PADDING, y, win->rec.width - 2 * WINDOW_PADDING, BUTTON_HEIGHT },
        .text = text,
        .color = GetColor(0xFFFFFFFF),
        .callback = callback
    };
}

Elios_Public void ui_add_labeled_text_to_window(UIWindow *win, c_string label, c_string value, DynamicTextCallback cb) {
    IfTrue (win->nbTexts >= MAX_LABELED_TEXT) return;

    float y = win->rec.y + 20 + win->nbTexts * 20.f;

    win->texts[win->nbTexts++] = (UILabeledText){
        .label = { .pos = { win->rec.x + 10, y }, .staticText = label, .size = FONT_SIZE, .color = GRAY, .dynamicText = cb },
        .content = { .pos = { win->rec.x + 20, y + 10 }, .staticText = value, .size = FONT_SIZE, .color = WHITE }
    };
}

Elios_Public void ui_add_input_to_window(UIWindow *win, c_string text, int32 maxLength, UIInputType type) {
    IfTrue (win->nbInputs >= MAX_INPUTS) return;

    float y = WINDOW_PADDING + FONT_PADDING + win->rec.y
            + (win->nbTexts * (FONT_SIZE + FONT_SPACING) + FONT_PADDING)
            + (win->nbButtons * (BUTTON_HEIGHT + BUTTON_SPACING))
            + win->nbInputs * (INPUT_HEIGHT + INPUT_PADDING);

    UIInput *input = &win->inputs[win->nbInputs++];
    strcpy(input->text, text);
    *input = (UIInput){
        .rec = { win->rec.x + 14.f, y + 20.f, win->rec.width - 28.f, INPUT_HEIGHT },
        .color = GetColor(0xFFFFFFFF),
        .maxLength = maxLength,
        .isFocused = false,
        .type = type
    };
}

Elios_Public void ui_update_window_inputs(UIWindow *win) {
    ForRange (int32, i, 0, win->nbInputs) {
        UIInput *input = &win->inputs[i];

        IfTrue (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            input->isFocused = ui_is_mouse_over_rect(&input->rec);
        }

        IfTrue (input->isFocused) {
            disable_camera_control_callback();

            IfTrue (IsKeyPressed(KEY_BACKSPACE) && strlen(input->text) > 0) {
                input->text[strlen(input->text) - 1] = '\0';
            } else {
                ForRange (int32, j, 32, 127) {
                    IfTrue (IsKeyPressed(j) && strlen(input->text) < input->maxLength - 1) {
                        char c = (char)j;
                        IfTrue (input->type == UI_INPUT_NUMBER && !isdigit(c) && c != '.') continue;
                        input->text[strlen(input->text)] = c;
                        input->text[strlen(input->text) + 1] = '\0';
                    }
                } EForRange;
            }
        } else {
            enable_camera_control_callback();
        }
    } EForRange;
}

Elios_Private void update_placing_items() {
    IfFalse (ui_get_action() == ENTITY_ADD) return;

    IfTrue (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !clickingUI && !draggingTile) {
        draggingTile = true;
        dragStartWorld = get_mouse_world_position_on_ground();
    }

    IfTrue (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && draggingTile) {
        draggingTile = false;
        Vector3 dragEndWorld = get_mouse_world_position_on_ground();
        create_tile_area_callback($void &dragStartWorld, $void &dragEndWorld);
    }
}

Elios_Private void update_window(UIWindow *win) {
    Rectangle mouse = { GetMousePosition().x, GetMousePosition().y, 2.f, 2.f };
    clickingUI = clickingUI || CheckCollisionRecs(win->rec, mouse);

    IfTrue (ui_is_button_clicked(&win->toggle)) win->toggled = !win->toggled;
    IfFalse (win->toggled) return;

    ForRange (int32, i, 0, win->nbButtons)
        IfTrue (ui_is_button_clicked(&win->buttons[i]) && win->buttons[i].callback)
            win->buttons[i].callback(NULL);
    EForRange;

    ui_update_window_inputs(win);
}

Elios_Private c_string get_fps_text() {
    static char buffer[6];
    snprintf(buffer, sizeof(buffer), "%d", GetFPS());
    return buffer;
}

Elios_Private c_string get_entity_id_text() {
    IfTrue (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !clickingUI) {
        selectedEntityId = get_entity_under_mouse_callback();
    }

    IfTrue (selectedEntityId == NOT_AN_ENTITY) return "Nothing selected";

    static char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d", selectedEntityId);
    return buffer;
}

Elios_Private void change_mode_to_place_item_callback(void *) {
    devActionsState = ENTITY_ADD;
}

Elios_Private void init_left_panel() {
    leftPanel = (UIWindow){
        .title = "Systems",
        .rec = { 10, 10, 120, 0 },
        .toggle = { .rec = { 14, 14, 10, 10 }, .text = "^", .color = GetColor(0xFFFFFFFF) },
        .toggled = false
    };

    ui_add_button_to_window(&leftPanel, "Change mode",  change_mode_callback);
    ui_add_button_to_window(&leftPanel, "Save state",   save_engine_state_callback);
    ui_add_button_to_window(&leftPanel, "Reload state", reload_engine_state_callback);
    ui_add_button_to_window(&leftPanel, "Add tile",     change_mode_to_place_item_callback);
}

Elios_Private void init_right_panel() {
    float screenW = (float)GetScreenWidth();
    rightPanel = (UIWindow){
        .title = "System props",
        .rec = { screenW - 314.f, 14.f, 300.f, 50.f },
        .toggle = { .rec = { screenW - 310.f, 20.f, 10.f, 10.f }, .text = "^", .color = GetColor(0xFFFFFFFF) },
        .toggled = true
    };

    ui_add_labeled_text_to_window(&rightPanel, "Fps", "0", get_fps_text);
    ui_add_labeled_text_to_window(&rightPanel, "Entity selected: ", "-1", get_entity_id_text);
    ui_add_input_to_window(&rightPanel, "something", 30, UI_INPUT_TEXT);
}


Elios_Public void init_ui() {
    init_left_panel();
    init_right_panel();
}

Elios_Public void render_ui() {
    ui_render_window(&leftPanel);
    ui_render_window(&rightPanel);
}

Elios_Public void update_ui() {
    clickingUI = false;
    update_placing_items();
    update_window(&leftPanel);
    update_window(&rightPanel);
}

Elios_Public void destroy_ui() {}