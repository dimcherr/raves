#include "tun/tgl.h"
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "tun/tlog.h"

void tgl::init() {
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

    tlog("tgl init");
}

void tgl::render(Color clearColor, std::function<void()> func) {
    sg_pass pass {};
    pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass.action.colors[0].clear_value = {clearColor.r, clearColor.g, clearColor.b, 1.f};
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
    pass.swapchain = swapchain;
    sg_begin_pass(pass);
    func();
    sg_end_pass();
}

void tgl::destroy() {
    sg_shutdown();
}

