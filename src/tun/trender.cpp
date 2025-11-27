#include "tun/trender.h"

void tun::UpdateCamera(CCamera& camera, float screenWidth, float screenHeight) {
    if (camera.type == EProjType::perspective) {
        if (screenHeight > 0.f) {
            camera.proj = glm::perspective(camera.fovy, screenWidth / screenHeight, camera.znear, camera.zfar);
        } else {
            terror("Screen height should be > 0 in Camera.Update()");
        }
    } else {
        camera.proj = glm::ortho(0.f, screenWidth, 0.f, screenHeight, camera.znear, camera.zfar);
    }
}
