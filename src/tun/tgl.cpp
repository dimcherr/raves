#include "tun/tgl.h"
#include "tun/tlog.h"
#include "stb_image.h"
#include "stb_image_resize2.h"
#include "sokol_log.h"

void gl::Init() {
    tun::logpush();

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



    sg_image_desc bufDepthDesc {};
    bufDepthDesc.width = gl::state.nativeResolution;
    bufDepthDesc.height = gl::state.nativeResolution;
    bufDepthDesc.render_target = true;
    bufDepthDesc.pixel_format = SG_PIXELFORMAT_DEPTH;
    bufDepthDesc.sample_count = 1;

    sg_image_desc bufLightingDesc {};
    bufLightingDesc.width = gl::state.nativeResolution;
    bufLightingDesc.height = gl::state.nativeResolution;
    bufLightingDesc.render_target = true;
    bufLightingDesc.sample_count = 1;

    gl::state.lightingAtts.depth_stencil.image = sg_make_image(bufDepthDesc);
    bufLightingDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
    gl::state.lightingAtts.colors[0].image = sg_make_image(bufLightingDesc);
    gl::state.lightingPass.attachments = sg_make_attachments(gl::state.lightingAtts);
    gl::state.lightingPass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
    gl::state.lightingPass.action.colors[0].clear_value = {0.f, 0.f, 0.f, 0.f};

    sg_image_desc bufColorDesc {};
    bufColorDesc.width = gl::state.nativeResolution;
    bufColorDesc.height = gl::state.nativeResolution;
    bufColorDesc.render_target = true;
    bufColorDesc.sample_count = 1;

    gl::state.gcolorAtts.depth_stencil.image = sg_make_image(bufDepthDesc);

    bufColorDesc.pixel_format = SG_PIXELFORMAT_RGBA16F;
    gl::state.gcolorAtts.colors[0].image = sg_make_image(bufColorDesc); // ORM

    bufColorDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
    gl::state.gcolorAtts.colors[1].image = sg_make_image(bufColorDesc); // albedo

    bufColorDesc.pixel_format = SG_PIXELFORMAT_RGBA16F;
    gl::state.gcolorAtts.colors[2].image = sg_make_image(bufColorDesc); // world position

    bufColorDesc.pixel_format = SG_PIXELFORMAT_RGBA16F;
    gl::state.gcolorAtts.colors[3].image = sg_make_image(bufColorDesc); // world normal

    //bufColorDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
    //gl::state.gcolorAtts.colors[2].image = sg_make_image(bufColorDesc); // emissive

    gl::state.gcolorPass.attachments = sg_make_attachments(gl::state.gcolorAtts);
    gl::state.gcolorPass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
    gl::state.gcolorPass.action.colors[0].clear_value = {0.f, 0.f, 0.f, 1.f};
    gl::state.gcolorPass.action.colors[1].load_action = SG_LOADACTION_CLEAR;
    gl::state.gcolorPass.action.colors[1].clear_value = {0.f, 0.f, 0.f, 1.f};
    gl::state.gcolorPass.action.colors[2].load_action = SG_LOADACTION_CLEAR;
    gl::state.gcolorPass.action.colors[2].clear_value = {0.f, 0.f, 0.f, 1.f};
    gl::state.gcolorPass.action.colors[3].load_action = SG_LOADACTION_CLEAR;
    gl::state.gcolorPass.action.colors[3].clear_value = {0.f, 0.f, 0.f, 1.f};
    gl::state.gcolorPass.action.depth.load_action = SG_LOADACTION_CLEAR;
    gl::state.gcolorPass.action.depth.store_action = SG_STOREACTION_STORE;
    gl::state.gcolorPass.action.depth.clear_value = 1.f;

    sg_image_desc imageDesc {};
    // TODO fix this, i need to make image the same size as screen, so i would need to recreate render target every time when user resizes the screen
    imageDesc.width = gl::state.nativeResolution;
    imageDesc.height = gl::state.nativeResolution;
    imageDesc.render_target = true;
    imageDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
    imageDesc.sample_count = 1;
    gl::state.colorBuffer = sg_make_image(imageDesc);
    imageDesc.pixel_format = SG_PIXELFORMAT_DEPTH;
    gl::state.depthBuffer = sg_make_image(imageDesc);

    sg_attachments_desc attachmentDesc {};
    attachmentDesc.colors[0].image = gl::state.colorBuffer;
    attachmentDesc.depth_stencil.image = gl::state.gcolorAtts.depth_stencil.image;

    gl::state.offscreenPass.attachments = sg_make_attachments(attachmentDesc);
    gl::state.offscreenPass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
    gl::state.offscreenPass.action.colors[0].clear_value = {0.f, 0.f, 0.f, 0.f};
    gl::state.offscreenPass.action.depth.load_action = SG_LOADACTION_LOAD;



    sg_attachments_desc skyboxAtts {};
    skyboxAtts.colors[0].image = gl::state.gcolorAtts.colors[1].image;
    //skyboxAtts.depth_stencil.image = gl::state.gcolorAtts.depth_stencil.image;
    gl::state.skyboxPass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
    //gl::state.skyboxPass.action.depth.load_action = SG_LOADACTION_CLEAR;
    gl::state.skyboxPass.action.colors[0].clear_value = {0.f, 0.f, 0.f, 1.f};
    gl::state.skyboxPass.attachments = sg_make_attachments(skyboxAtts);





    sg_image_desc textImageDesc {};
    // TODO fix this, i need to make image the same size as screen, so i would need to recreate render target every time when user resizes the screen
    int textNativeResolution {720};
    textImageDesc.width = textNativeResolution;
    textImageDesc.height = textNativeResolution;
    textImageDesc.render_target = true;
    textImageDesc.pixel_format = SG_PIXELFORMAT_RGBA8;
    textImageDesc.sample_count = 1;
    gl::state.textColorBuffer = sg_make_image(textImageDesc);
    textImageDesc.pixel_format = SG_PIXELFORMAT_DEPTH;
    gl::state.textDepthBuffer = sg_make_image(textImageDesc);

    sg_attachments_desc textAttachmentDesc {};
    textAttachmentDesc.colors[0].image = gl::state.textColorBuffer;
    textAttachmentDesc.depth_stencil.image = gl::state.textDepthBuffer;

    gl::state.textOffscreenPass.attachments = sg_make_attachments(textAttachmentDesc);
    gl::state.textOffscreenPass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
    gl::state.textOffscreenPass.action.colors[0].clear_value = {0.f, 0.f, 0.f, 0.f};

    tun::logpop("gl init");
}

gl::Buffer gl::CreateVertexBuffer(const List<float>& vertices) {
    sg_buffer_desc desc {};
    desc.data = sg_range { vertices.data(), vertices.size() * sizeof(float) };
    return sg_make_buffer(desc);
}

gl::Buffer gl::CreateVertexBufferTextParticle() {
    sg_buffer_desc desc {};
    desc.size = gl::State::maxTextCharacters * sizeof(gl::TextParticleData);
    desc.usage = SG_USAGE_STREAM;
    return sg_make_buffer(desc);
}

gl::Buffer gl::CreateVertexBufferParticle() {
    sg_buffer_desc desc {};
    desc.size = gl::State::maxParticles * sizeof(gl::ParticleData);
    desc.usage = SG_USAGE_STREAM;
    return sg_make_buffer(desc);
}

gl::Buffer gl::CreateIndexBuffer(const List<uint16_t>& indices) {
    sg_buffer_desc desc {};
    desc.data = sg_range { indices.data(), indices.size() * sizeof(uint16_t) };
    desc.type = SG_BUFFERTYPE_INDEXBUFFER;
    return sg_make_buffer(desc);
}

gl::Image gl::CreateImageRaw(const Bytes& data) {
    sg_image_desc desc {};
    int ch;

    stbi_uc* buf = stbi_load_from_memory((const stbi_uc*)data.data(), data.size(), &desc.width, &desc.height, &ch, 4);

    desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    desc.data.subimage[0][0] = sg_range { buf, (size_t)(desc.width * desc.height * 4) };
    int maxMipMaps = 8;
    for (int i = 1; i < 8; ++i) {
        int ww = desc.width >> i;
        int hh = desc.height >> i;
        if (ww < 1) break;
        unsigned char* p = (unsigned char*)malloc(ww * hh * 4);
        stbir_resize_uint8_linear((const unsigned char*)buf, desc.width, desc.height, 0, p, ww, hh, 0, stbir_pixel_layout::STBIR_4CHANNEL);
        desc.data.subimage[0][i] = sg_range { p, (size_t)(ww * hh * 4) };
        desc.num_mipmaps = i + 1;
    }

    gl::Image img = sg_make_image(desc);
    return img;
}

gl::Image gl::CreateImageSimpleFromMemory(unsigned char* buffer, int w, int h) {
    sg_image_desc desc {};
    desc.width = w;
    desc.height = h;
    desc.pixel_format = SG_PIXELFORMAT_R8;
    desc.data.subimage[0][0] = sg_range { buffer, (size_t)(w * h) };

    int maxMipMaps = 8;
    for (int i = 1; i < maxMipMaps; ++i) {
        int ww = w >> i;
        int hh = h >> i;
        if (ww < 1) break;
        unsigned char* p = (unsigned char*)malloc(ww * hh * sizeof(unsigned char));
        stbir_resize_uint8_linear(buffer, w, h, 0, p, ww, hh, 0, stbir_pixel_layout::STBIR_1CHANNEL);
        desc.data.subimage[0][i] = sg_range { p, (size_t)(ww * hh) };
        desc.num_mipmaps = i + 1;
    }

    gl::Image img = sg_make_image(desc);
    return img;
}

gl::Image gl::CreateImageSimple(StringView path) {
    int png_width, png_height, num_channels;
    const int desired_channels = 4;
    stbi_uc* ptr = stbi_load(path.data(), &png_width, &png_height, &num_channels, desired_channels);
    sg_image_desc desc {};
    desc.width = png_width;
    desc.height = png_height;

    desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    desc.data.subimage[0][0] = sg_range { ptr, (size_t)(desc.width * desc.height * 4) };
    int maxMipMaps = 8;
    for (int i = 1; i < maxMipMaps; ++i) {
        int ww = desc.width >> i;
        int hh = desc.height >> i;
        if (ww < 1) break;
        unsigned char* p = (unsigned char*)malloc(ww * hh * 4);
        stbir_resize_uint8_linear((const unsigned char*)ptr, desc.width, desc.height, 0, p, ww, hh, 0, stbir_pixel_layout::STBIR_4CHANNEL);
        desc.data.subimage[0][i] = sg_range { p, (size_t)(ww * hh * 4) };
        desc.num_mipmaps = i + 1;
    }

    gl::Image img = sg_make_image(desc);
    stbi_image_free(ptr);
    return img;
}

void gl::UpdateTextParticleBuffer(const sg_bindings& bind, gl::TextParticleData* data, size_t dataLength) {
    sg_update_buffer(bind.vertex_buffers[1], sg_range { data, dataLength });
}

void gl::UpdateParticleBuffer(const sg_bindings& bind, gl::ParticleData* data, size_t dataLength) {
    sg_update_buffer(bind.vertex_buffers[2], sg_range { data, dataLength });
}

void gl::BeginLightingPass() {
    sg_begin_pass(gl::state.lightingPass);
}

void gl::BeginGColorPass() {
    sg_begin_pass(gl::state.gcolorPass);
}

void gl::BeginOffscreenPass() {
    sg_begin_pass(gl::state.offscreenPass);
}

void gl::BeginTextOffscreenPass() {
    sg_begin_pass(gl::state.textOffscreenPass);
}

void gl::BeginRenderPass(Color clearColor, int width, int height) {
    sg_pass pass {};
    pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass.action.colors[0].clear_value = {clearColor.r, clearColor.g, clearColor.b, 1.f};
    sg_swapchain swapchain {};
    swapchain.width = width;
    swapchain.height = height;
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
}

void gl::EndRenderPass() {
    sg_end_pass();
}

void gl::EndDrawing() {
    sg_commit();
}

void gl::Shutdown() {
    sg_shutdown();
}
