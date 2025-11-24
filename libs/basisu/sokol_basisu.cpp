//-----------------------------------------------------------------------------
//  basisu_sokol.cpp
//-----------------------------------------------------------------------------
#ifdef __APPLE__
    #include <TargetConditionals.h>
#endif
#if !(TARGET_OS_IPHONE || defined(__EMSCRIPTEN__) || defined(__ANDROID__))
    #define BASISD_SUPPORT_BC7 (0)
    #define BASISD_SUPPORT_ATC (1)
#endif
#if defined(__ANDROID__)
    #define BASISD_SUPPORT_PVRTC2 (0)
#endif
#define BASISU_NO_ITERATOR_DEBUG_LEVEL (1)
#include "basisu_transcoder.h"
#include "sokol_gfx.h"
#include "sokol_basisu.h"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

static basist::etc1_global_selector_codebook *g_pGlobal_codebook;

void sbasisu_setup(void) {
    basist::basisu_transcoder_init();
    if (!g_pGlobal_codebook) {
        g_pGlobal_codebook = new basist::etc1_global_selector_codebook(
            basist::g_global_selector_cb_size,
            basist::g_global_selector_cb);
    }
}

void sbasisu_shutdown(void) {
    if (g_pGlobal_codebook) {
        delete g_pGlobal_codebook;
        g_pGlobal_codebook = nullptr;
    }
}

static basist::transcoder_texture_format select_basis_textureformat(bool has_alpha) {
    if (has_alpha && sg_query_pixelformat(SG_PIXELFORMAT_BC3_RGBA).sample) {
        return basist::transcoder_texture_format::cTFBC3_RGBA;
    } else if (!has_alpha && sg_query_pixelformat(SG_PIXELFORMAT_BC1_RGBA).sample) {
        return basist::transcoder_texture_format::cTFBC1_RGB;
    } else if (has_alpha && sg_query_pixelformat(SG_PIXELFORMAT_ETC2_RGBA8).sample) {
        return basist::transcoder_texture_format::cTFETC2_RGBA;
    } else if (!has_alpha && sg_query_pixelformat(SG_PIXELFORMAT_ETC2_RGB8).sample) {
        return basist::transcoder_texture_format::cTFETC1_RGB;
    } else {
        // UGH...
        return basist::transcoder_texture_format::cTFRGBA32;
    }
}

static sg_pixel_format basis_to_sg_pixelformat(basist::transcoder_texture_format fmt) {
    switch (fmt) {
        case basist::transcoder_texture_format::cTFBC3_RGBA: return SG_PIXELFORMAT_BC3_RGBA;
        case basist::transcoder_texture_format::cTFBC1_RGB: return SG_PIXELFORMAT_BC1_RGBA;
        case basist::transcoder_texture_format::cTFETC2_RGBA: return SG_PIXELFORMAT_ETC2_RGBA8;
        case basist::transcoder_texture_format::cTFETC1_RGB: return SG_PIXELFORMAT_ETC2_RGB8;
        case basist::transcoder_texture_format::cTFRGBA32: return SG_PIXELFORMAT_RGBA8;
        default: return _SG_PIXELFORMAT_DEFAULT;
    }
}

sg_image_desc sbasisu_transcode(sg_range basisu_data) {
    assert(g_pGlobal_codebook);
    //basist::basisu_transcoder transcoder(g_pGlobal_codebook);
    basist::ktx2_transcoder trans(g_pGlobal_codebook);

    //transcoder.start_transcoding(basisu_data.ptr, (uint32_t)basisu_data.size);
    bool hey = trans.start_transcoding();
    bool successss = trans.init(basisu_data.ptr, (uint32_t)basisu_data.size);

    sg_image_desc d {};
    d.type = SG_IMAGETYPE_2D;
    d.width = (int)trans.get_width();
    d.height = (int)trans.get_height();
    d.pixel_format = SG_PIXELFORMAT_BC7_RGBA;
    d.usage = SG_USAGE_IMMUTABLE;
    d.num_mipmaps = (int)trans.get_levels();
    const uint32_t bytes_per_block = basist::basis_get_bytes_per_block_or_pixel(basist::transcoder_texture_format::cTFBC7_RGBA);
    for (int i = 0; i < d.num_mipmaps; ++i) {
        basist::ktx2_image_level_info levelInfo {};
        trans.get_image_level_info(levelInfo, i, 0, 0);
        uint32_t requiredSize = levelInfo.m_total_blocks * bytes_per_block;
        void* ptr = malloc(requiredSize);
        trans.transcode_image_level(i, 0, 0, ptr, requiredSize, basist::transcoder_texture_format::cTFBC7_RGBA);
        d.data.subimage[0][i].ptr = ptr;
        d.data.subimage[0][i].size = requiredSize;
    }
    return d;
}

void sbasisu_free(const sg_image_desc* desc) {
    assert(desc);
    for (int i = 0; i < desc->num_mipmaps; i++) {
        if (desc->data.subimage[0][i].ptr) {
            free((void*)desc->data.subimage[0][i].ptr);
        }
    }
}

sg_image sbasisu_make_image(sg_range basisu_data) {
    sg_image_desc img_desc = sbasisu_transcode(basisu_data);
    sg_image img = sg_make_image(&img_desc);
    sbasisu_free(&img_desc);
    return img;
}

sg_pixel_format sbasisu_pixelformat(bool has_alpha) {
    return basis_to_sg_pixelformat(select_basis_textureformat(has_alpha));
}
