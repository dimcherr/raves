#include "tun/trender.h"

void tun::UpdateCamera(CameraComp& camera, float screenWidth, float screenHeight) {
    if (camera.type == tun::perspective) {
        if (screenHeight > 0.f) {
            camera.projection = glm::perspective(camera.fovy, screenWidth / screenHeight, camera.znear, camera.zfar);
        } else {
            tun::error("Screen height should be > 0 in Camera.Update()");
        }
    } else {
        camera.projection = glm::ortho(0.f, screenWidth, 0.f, screenHeight, camera.znear, camera.zfar);
    }
}
