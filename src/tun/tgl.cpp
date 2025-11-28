#include "tun/tgl.h"
#include "stb_image.h"
#include "stb_image_resize2.h"
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "tun/tlog.h"

static sg_pass passScreen {};

static sg_swapchain getSwapchain() {
    sg_swapchain swapchain {};
    swapchain.width = sapp_width();
    swapchain.height = sapp_height();
    swapchain.sample_count = sapp_sample_count();
    swapchain.color_format = (sg_pixel_format)sapp_color_format();
    swapchain.depth_format = SG_PIXELFORMAT_DEPTH;
    swapchain.metal.current_drawable = sapp_metal_get_current_drawable();
    swapchain.metal.depth_stencil_texture = sapp_metal_get_depth_stencil_texture();
    swapchain.metal.msaa_color_texture = sapp_metal_get_msaa_color_texture();
    swapchain.d3d11.render_view = sapp_d3d11_get_render_view();
    swapchain.d3d11.resolve_view = sapp_d3d11_get_resolve_view();
    swapchain.d3d11.depth_stencil_view = sapp_d3d11_get_depth_stencil_view();
    swapchain.wgpu.render_view = sapp_wgpu_get_render_view();
    swapchain.wgpu.resolve_view = sapp_wgpu_get_resolve_view();
    swapchain.wgpu.depth_stencil_view = sapp_wgpu_get_depth_stencil_view();
    swapchain.gl.framebuffer = sapp_gl_get_framebuffer();
    return swapchain;
}

void tgl::create() {
    sg_desc desc {};
    sg_environment env {};
    env.defaults.color_format = (sg_pixel_format) sapp_color_format();
    env.defaults.depth_format = (sg_pixel_format) sapp_depth_format();
    env.defaults.sample_count = sapp_sample_count();
    env.metal.device = sapp_metal_get_device();
    env.d3d11.device = sapp_d3d11_get_device();
    env.d3d11.device_context = sapp_d3d11_get_device_context();
    env.wgpu.device = sapp_wgpu_get_device();
    desc.environment = env;
    desc.logger.func = slog_func;
    desc.buffer_pool_size = 2048;
    desc.uniform_buffer_size = 4 * 1024 * 1024;
    desc.image_pool_size = 256;
    sg_setup(desc);

    passScreen.action.colors[0].load_action = SG_LOADACTION_CLEAR;
    passScreen.action.colors[0].clear_value = {0.5, 0.5, 0.25, 1.f};

    tlog("tgl create");
}

void tgl::update() {
    passScreen.swapchain = getSwapchain();
    sg_begin_pass(passScreen);
    sg_end_pass();
    sg_commit();
}

void tgl::destroy() {
    sg_shutdown();
}

sg_buffer tgl::createVertexBuffer(const List<float>& vertices) {
    sg_buffer_desc desc {};
    desc.data = sg_range { vertices.data(), vertices.size() * sizeof(float) };
    return sg_make_buffer(desc);
}

sg_buffer tgl::createIndexBuffer(const List<uint16_t>& indices) {
    sg_buffer_desc desc {};
    desc.data = sg_range { indices.data(), indices.size() * sizeof(uint16_t) };
    desc.type = SG_BUFFERTYPE_INDEXBUFFER;
    return sg_make_buffer(desc);
}

sg_image tgl::createImageFromMemory(const List<Byte>& bytes) {
    sg_image_desc desc {};
    int ch;
    stbi_uc* buf = stbi_load_from_memory(
        (const stbi_uc*)bytes.data(),
        bytes.size(),
        &desc.width,
        &desc.height,
        &ch,
        4
    );

    desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    desc.data.subimage[0][0] = sg_range { buf, (size_t)(desc.width * desc.height * 4) };
    static int maxMipMaps = 8;
    for (int i = 1; i < 8; ++i) {
        int ww = desc.width >> i;
        int hh = desc.height >> i;
        if (ww < 1) break;
        unsigned char* p = (unsigned char*)malloc(ww * hh * 4);
        stbir_resize_uint8_linear(
            (const unsigned char*)buf,
            desc.width,
            desc.height,
            0,
            p,
            ww,
            hh,
            0,
            stbir_pixel_layout::STBIR_4CHANNEL
        );
        desc.data.subimage[0][i] = sg_range { p, (size_t)(ww * hh * 4) };
        desc.num_mipmaps = i + 1;
    }

    sg_image img = sg_make_image(desc);
    return img;
}

