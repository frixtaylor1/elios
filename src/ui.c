// Refactored UI System for better reuse and modularity
#include <ui/ui.h>
#include <controller/controller.h>

#define MAX_UI_BUTTONS      64
#define MAX_LABELED_TEXT    64
#define BUTTON_HEIGHT       22.f
#define BUTTON_SPACING      5.f
#define WINDOW_PADDING      10.f
#define FONT_SIZE           7
#define FONT_SIZE_SPACING   14.f

typedef struct UIButton {
    Rectangle rec;
    c_string  text;
    Color     color;
    ButtonClickCallback callback;
} UIButton;

typedef c_string (*DynamicTextCallback)(void); 

typedef struct UILabel {
    Vector2 pos;
    float size;
    Color color;

    DynamicTextCallback dynamicText;
    c_string staticText;
} UILabel;

typedef struct UILabeledText {
    UILabel label;
    UILabel content;
} UILabeledText;

typedef struct UIWindow {
    c_string title;
    Rectangle rec;
    UIButton toggle;
    UIButton buttons[MAX_UI_BUTTONS];
    int32 nbButtons;
    UILabeledText texts[MAX_LABELED_TEXT];
    int32 nbTexts;
    bool toggled;
} UIWindow;


/**
 * UI Engine State
 */

Elios_Private UIWindow leftPanel;
Elios_Private UIWindow rightPanel;

/**
 * Core UI Utility...
 */

Elios_Private bool mouse_over_rect(const Rectangle *rec) {
    Rectangle mouse = { GetMousePosition().x, GetMousePosition().y, 4.f, 4.f };
    return CheckCollisionRecs(*rec, mouse);
}

Elios_Private bool is_button_hovered(const UIButton *btn) {
    return mouse_over_rect(&btn->rec);
}

Elios_Private bool is_button_clicked(const UIButton *btn) {
    return is_button_hovered(btn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

Elios_Private void render_button(UIButton *btn) {
    Color base = btn->color;
    IfTrue (is_button_hovered(btn)) base = GetColor(0xAAAAAAFF);
    IfTrue (is_button_clicked(btn)) base = GetColor(0x777777FF);

    DrawRectangleRounded(btn->rec, 0.4f, 4, base);
    DrawText(btn->text, btn->rec.x + 5, btn->rec.y + 5, FONT_SIZE, BLACK);
}

Elios_Private void add_button_to_window(UIWindow *win, const char *text, ButtonClickCallback callback) {
    IfTrue (win->nbButtons >= MAX_UI_BUTTONS) return;

    float y = win->rec.y + WINDOW_PADDING + (win->nbButtons * (BUTTON_HEIGHT + BUTTON_SPACING)) + 20.f;

    win->buttons[win->nbButtons++] = (UIButton){
        .rec = { win->rec.x + WINDOW_PADDING, y, win->rec.width - 2 * WINDOW_PADDING, BUTTON_HEIGHT },
        .text = text,
        .color = GetColor(0xFFFFFFFF),
        .callback = callback
    };
}

Elios_Private void add_labeled_text_to_window(UIWindow *win, const char *label, const char *value, DynamicTextCallback dynamicTextCallback) {
    IfTrue (win->nbTexts >= MAX_LABELED_TEXT) return;

    float y = win->rec.y + 20 + win->nbTexts * 20.f;

    win->texts[win->nbTexts++] = (UILabeledText){
        .label = { .pos = { win->rec.x + 10, y }, .staticText = label, .size = FONT_SIZE, .color = GRAY, .dynamicText = dynamicTextCallback },
        .content = { .pos = { win->rec.x + 20, y + 10 }, .staticText = value, .size = FONT_SIZE, .color = WHITE }
    };
}

Elios_Private void render_window(UIWindow *win) {
    float height = (win->toggled) ? (win->nbButtons * (BUTTON_HEIGHT + BUTTON_SPACING) + 30.f) + (win->nbTexts * FONT_SIZE_SPACING) : 20.f;
    win->rec.height = height;

    DrawRectangleRec(win->rec, GetColor(0x444444CC));
    DrawRectangleLinesEx(win->rec, 1, GetColor(0x888888FF));
    DrawText(win->title, win->rec.x + 20, win->rec.y + 5, 8, WHITE);
    render_button(&win->toggle);

    IfTrue (win->toggled) {
        ForRange (int32, i, 0, win->nbButtons)
            render_button(&win->buttons[i]);
        EForRange;
        ForRange (int32, i, 0, win->nbTexts)
        	const char *text = win->texts[i].label.dynamicText ? win->texts[i].label.dynamicText() : win->texts[i].label.staticText;
            DrawText(win->texts[i].label.staticText, win->texts[i].label.pos.x, win->texts[i].label.pos.y, FONT_SIZE, win->texts[i].label.color);
            DrawText(text, win->texts[i].content.pos.x, win->texts[i].content.pos.y, FONT_SIZE, win->texts[i].content.color);
        EForRange;
    }
}

Elios_Private void update_window(UIWindow *win) {
    IfTrue (is_button_clicked(&win->toggle)) win->toggled = !win->toggled;
    IfFalse (win->toggled) return;

    ForRange (int32, i, 0, win->nbButtons)
        IfTrue (is_button_clicked(&win->buttons[i]) && win->buttons[i].callback) {
            win->buttons[i].callback(NULL);
        }
    EForRange;
}

Elios_Private c_string get_fps_text() {
    static char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d", GetFPS());
    return buffer;
}

/**
 * Initialization...
 */

Elios_Private void init_left_panel() {
    leftPanel = (UIWindow){
        .title = "Systems",
        .rec = { 10, 10, 120, 0 },
        .toggle = {
            .rec = { 14, 14, 10, 10 },
            .text = "^",
            .color = GetColor(0xFFFFFFFF),
            .callback = NULL
        },
        .toggled = false,
    };
    leftPanel.toggle.callback = NULL;

    add_button_to_window(&leftPanel, "Change mode",  change_mode_callback);
    add_button_to_window(&leftPanel, "Save state",   save_engine_state_callback);
    add_button_to_window(&leftPanel, "Reload state", reload_engine_state_callback);
}

Elios_Private void init_right_panel() {
    rightPanel = (UIWindow){
        .title = "System props",
        .rec = { (float)GetScreenWidth() - 314, 14, 300, 50 },
        .toggle = {
            .rec = { (float)GetScreenWidth() - 310, 20, 10, 10 },
            .text = "^",
            .color = GetColor(0xFFFFFFFF),
            .callback = NULL
        },
        .toggled = true
    };
    rightPanel.toggle.callback = NULL;

    add_labeled_text_to_window(&rightPanel, "Fps", "0", &get_fps_text);
}
 
/**
 * Public functions...
 */

Elios_Public void init_ui() {
    init_left_panel();
    init_right_panel();
}

Elios_Public void render_ui() {
    render_window(&leftPanel);
    render_window(&rightPanel);
}

Elios_Public void update_ui() {
    update_window(&leftPanel);
    update_window(&rightPanel);
}

Elios_Public void destroy_ui() {
    // Nothing to do yet
}
