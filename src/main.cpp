#include "game.h"
#include "sokol_app.h"
#include "sokol_log.h"
#include "sokol_time.h"
#include "stb_image.h"
#include "stb_image_resize2.h"
#include "tun/tgl.h"

sapp_desc sokol_main(int argc, char* argv[]) {
    stm_setup();

    (void)argc;
    (void)argv;
    sapp_desc desc {};
    desc.init_cb = &game::create;
    desc.frame_cb = &game::update;
    desc.cleanup_cb = &game::destroy;
    desc.event_cb = &game::onEvent;

    float ratio = 1920 / 1080.f;
    desc.width = 1517;
    desc.height = 1517 / ratio;
    desc.window_x = 1920 - desc.width - 8;
    desc.window_y = 140;

    desc.sample_count = 0;
    desc.fullscreen = false;
    desc.window_title = "Raves";
    desc.logger.func = slog_func;
    desc.html5_bubble_mouse_events = true;

#if OS_WEB
    desc.icon.sokol_default = true;
#else
    desc.icon.sokol_default = false;
    List<int> dims {256};
    int c = 0;
    for (auto dim : dims) {
        int png_width, png_height, num_channels;
        const int desired_channels = 4;
        stbi_uc* ptr = stbi_load(
                formatToString("res/textures/icon{}.png", dim).c_str(),
                &png_width,
                &png_height,
                &num_channels,
                desired_channels
        );
        sapp_image_desc idesc {};
        idesc.width = png_width;
        idesc.height = png_height;
        idesc.pixels = sapp_range { ptr, (size_t)(png_width * png_height * 4) };
        desc.icon.images[c] = idesc;
        ++c;
    }
#endif

    return desc;
}
