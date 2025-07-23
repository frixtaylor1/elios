#ifndef ui_H
#define ui_H

#include <elios.h>
#include <raylib/raylib.h>

#define WINDOW_PADDING      10.f
#define MAX_UI_BUTTONS      24
#define MAX_LABELED_TEXT    24
#define MAX_INPUTS          24
#define BUTTON_HEIGHT       22.f
#define BUTTON_SPACING      5.f
#define BUTTON_PADDING      30.f
#define FONT_SIZE           7.f
#define FONT_SPACING        5.f
#define FONT_PADDING        5.f
#define INPUT_HEIGHT        20.f
#define INPUT_PADDING       5.f
#define NOT_AN_ENTITY       -1

typedef void (*ButtonClickCallback)(void *);

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

typedef enum { UI_INPUT_TEXT, UI_INPUT_NUMBER } UIInputType;

typedef struct UIInput {
    c_string    id;
    Rectangle   rec;
    char        text[64];
    Color       color;
    int32       maxLength;
    bool        isFocused;
    UIInputType type;
} UIInput;

typedef struct UILabeledInput {
    UILabel label;
    UIInput input;
} UILabeledInput;

typedef struct UIWindow {
    c_string      title;
    UIButton      toggle;
    UIButton      buttons[MAX_UI_BUTTONS];
    UILabeledText texts[MAX_LABELED_TEXT];
    UIInput       inputs[MAX_INPUTS];
    Rectangle     rec;
    int32         nbButtons, nbTexts, nbInputs;
    bool          toggled;
} UIWindow;

typedef enum { 
	ENTITY_SELECTED, 
	ENTITY_ADD, 
	ENTITY_REMOVE, 
	ENTITY_UPDATE 
} UIDevActions;

Elios_Public void         init_ui();
Elios_Public void         render_ui();
Elios_Public void         update_ui();
Elios_Public void         destroy_ui();

Elios_Public bool         ui_is_mouse_over_rect(const Rectangle *rec);
Elios_Public bool         ui_is_button_hovered(const UIButton *btn);
Elios_Public bool         ui_is_button_clicked(const UIButton *btn);

Elios_Public void         ui_render_button(const UIButton *btn);
Elios_Public void         ui_render_input(const UIInput *input);
Elios_Public void         ui_render_window(UIWindow *win);

Elios_Public void         ui_add_labeled_text_to_window(UIWindow *win, c_string label, c_string value, DynamicTextCallback cb);
Elios_Public void         ui_add_button_to_window(UIWindow *win, c_string text, ButtonClickCallback callback);
Elios_Public void         ui_add_input_to_window(UIWindow *win, c_string text, int32 maxLength, UIInputType type);
Elios_Public void         ui_update_window_inputs(UIWindow *win);

Elios_Public void         ui_set_action(const UIDevActions action);
Elios_Public UIDevActions ui_get_action();

#endif // ui_H