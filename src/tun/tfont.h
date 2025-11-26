#pragma once
#include "stb_truetype.h"
#include "tun/tgl.h"
#include "tun/tlist.h"

namespace tfont {
    struct FontDesc {
        String fontPath {};
        float fontSize {48.f};
        int numRanges {4};
        float xoffset {0.f};
        float yoffset {0.21f};

        FontDesc(StringView fontPath, float fontSize = 48.f, int numRanges = 4, float xoffset = 0.f, float yoffset = 0.21f) : fontPath(fontPath), fontSize(fontSize), numRanges(numRanges), xoffset(xoffset), yoffset(yoffset) {}
    };

    struct Font {
        int numRanges {4};
        float fontSize {48.f};
        float xoffset {0.f};
        float yoffset {0.21f};
        stbtt_pack_range* ranges {};
    };

    struct FontData {
        sg_image fontAtlas {};
        float atlasSize {};
        List<Font> fonts {};
    };

    unsigned char* readFont(const char* path);
    stbtt_pack_range* packFont(stbtt_pack_context* ctx, const char* path, float fontSize, int numRanges);
    FontData packFonts(const List<FontDesc>& fonts, int atlasSize = 2048, bool saveAtlas = false);
}

