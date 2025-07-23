#ifndef camera_H
#define camera_H

#include <elios.h>
#include <raylib/raylib.h>

typedef void (*CameraUpdateCallback)(void);

Elios_Public void            update_camera();
Elios_Public const Camera3D *get_camera();
Elios_Public int32           get_camera_mode();
Elios_Public void            disable_camera_control();
Elios_Public void            enable_camera_control();

#endif // camera_H