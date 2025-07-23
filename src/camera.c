#include <camera/camera.h>
#include <raylib/raymath.h>

typedef void (*CameraUpdateCallback)(void);

Elios_Private Camera3D camera = {
    .position = { 50.0f, 50.0f, 150.0f },
    .target = { 0.0f, 0.0f, 0.0f },
    .up = { 0.0f, 1.0f, 0.0f },
    .fovy = 45.0f,
    .projection = CAMERA_PERSPECTIVE
};
Elios_Private int32 cameraMode  = CAMERA_FIRST_PERSON;
Elios_Private bool  blockCamera = false;

Elios_Private void change_camera_to_free_mode() {
    cameraMode = CAMERA_CUSTOM;
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
}

Elios_Private void change_camera_to_first_person() {
    cameraMode = CAMERA_FIRST_PERSON;
    camera.up = (Vector3){ 0.0f, 3.0f, 0.0f };
}

Elios_Private void change_camera_to_third_person() {
    cameraMode = CAMERA_THIRD_PERSON;
    camera.up = (Vector3){ 0.0f, 3.0f, 0.0f };
}

Elios_Private void change_camera_to_orbital_mode() {
    cameraMode = CAMERA_ORBITAL;
    camera.up = (Vector3){ 0.0f, 3.0f, 0.0f };
}

Elios_Private void change_camera_to_orthographic_mode() {
    cameraMode = CAMERA_THIRD_PERSON;
    camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.projection = CAMERA_ORTHOGRAPHIC;
    camera.fovy = 20.0f;
}

Elios_Private void change_camera_to_custom_mode() {
    cameraMode = CAMERA_CUSTOM;
    camera.fovy = 20.0f;
}

Elios_Private void change_camera_to_perspective_mode() {
    cameraMode = CAMERA_THIRD_PERSON;
    camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy = 60.0f;
}

Elios_Private void update_camera_position_forward_add() {
    float speed = 150.0f * GetFrameTime();
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    
    camera.position = Vector3Add(camera.position, Vector3Scale(forward, speed));
}

Elios_Private void update_camera_position_forward_substract() {
    float speed = 150.0f * GetFrameTime();
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    
    camera.position = Vector3Subtract(camera.position, Vector3Scale(forward, speed));
}

Elios_Private void update_camera_position_sides_add() {
    float speed = 150.0f * GetFrameTime();

    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));

    camera.position = Vector3Add(camera.position, Vector3Scale(right, speed));
}

Elios_Private void update_camera_position_sides_substract() {
    float speed = 150.0f * GetFrameTime();

    Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera.up));
    
    camera.position = Vector3Subtract(camera.position, Vector3Scale(right, speed));
}

Elios_Private void update_camera_fovy_up() {
    camera.position.y += 100.0f * GetFrameTime();
}

Elios_Private void update_camera_fovy_down() {
    camera.position.y -= 100.0f * GetFrameTime();
}

Elios_Private bool is_camera_changing_to_orthographic_mode() {
    return ((IsKeyPressed(KEY_P) && camera.projection == CAMERA_PERSPECTIVE)) && camera.projection != CAMERA_ORTHOGRAPHIC;  
}

Elios_Private bool is_camera_changing_to_perspective_mode() {
    return !is_camera_changing_to_orthographic_mode() && camera.projection != CAMERA_PERSPECTIVE;
}

Elios_Private void update_camera_by_condition(bool condition, CameraUpdateCallback callback) {
    IfTrue (condition) callback();
}

Elios_Public void update_camera() {
    IfTrue (blockCamera) return;
    
    update_camera_by_condition(IsKeyPressed(KEY_ONE), &change_camera_to_free_mode);
    update_camera_by_condition(IsKeyPressed(KEY_TWO), &change_camera_to_first_person);
    update_camera_by_condition(IsKeyPressed(KEY_THREE), &change_camera_to_third_person);
    update_camera_by_condition(IsKeyPressed(KEY_FOUR), &change_camera_to_orbital_mode);
    update_camera_by_condition(IsKeyPressed(KEY_ZERO), &change_camera_to_custom_mode);
    update_camera_by_condition(is_camera_changing_to_orthographic_mode(), &change_camera_to_orthographic_mode);
    update_camera_by_condition(is_camera_changing_to_perspective_mode(), &change_camera_to_perspective_mode);

    update_camera_by_condition(IsKeyDown(KEY_W), &update_camera_position_forward_add);
    update_camera_by_condition(IsKeyDown(KEY_S), &update_camera_position_forward_substract);
    update_camera_by_condition(IsKeyDown(KEY_D), &update_camera_position_sides_add);
    update_camera_by_condition(IsKeyDown(KEY_A), &update_camera_position_sides_substract);


    update_camera_by_condition(IsKeyDown(KEY_UP) && cameraMode == CAMERA_CUSTOM, &update_camera_fovy_up);
    update_camera_by_condition(IsKeyDown(KEY_DOWN) && cameraMode == CAMERA_CUSTOM, &update_camera_fovy_down);

    UpdateCamera(&camera, cameraMode);
}

Elios_Public const Camera3D *get_camera() {
	return &camera;
}

Elios_Public int32 get_camera_mode() {
	return cameraMode;
}

Elios_Public void disable_camera_control() {
    blockCamera = true;
}

Elios_Public void enable_camera_control() {
    blockCamera = false;
}