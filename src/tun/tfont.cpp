#include "tun/tfont.h"
#include "stb_image_write.h"
#include "stb_image_resize2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tun/tlog.h"
#include "tun/tgl.h"

static void initRange(stbtt_pack_range& range, float fontSize, int firstCodepoint, int lastCodepoint) {
    range.font_size = fontSize;
    range.num_chars = lastCodepoint - firstCodepoint + 1;
    range.first_unicode_codepoint_in_range = firstCodepoint;
    range.chardata_for_range = (stbtt_packedchar*)malloc(range.num_chars * sizeof(stbtt_packedchar));
}

unsigned char* tfont::readFont(const char* path) {
    FILE* fontFile = fopen(path, "rb");
    fseek(fontFile, 0, SEEK_END);
    long size = ftell(fontFile); /* how long is the file ? */
    fseek(fontFile, 0, SEEK_SET); /* reset */
    unsigned char* fontBuffer = (unsigned char*)malloc(size);
    memset(fontBuffer, 0, size);
    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);
    return fontBuffer;
}

stbtt_pack_range* tfont::packFont(stbtt_pack_context* ctx, const char* path, float fontSize, int numRanges) {
    unsigned char* fontBuffer = readFont(path);
    stbtt_pack_range* ranges = (stbtt_pack_range*)malloc(numRanges * sizeof(stbtt_pack_range));
    memset(ranges, 0, sizeof(stbtt_pack_range) * numRanges);

    if (numRanges == 4) {
        initRange(ranges[0], fontSize, 0x0020, 0x007E); // latin
        initRange(ranges[1], fontSize, 0x0410, 0x044F); // cyrillic
        initRange(ranges[2], fontSize, 0x0401, 0x0401); // Ё
        initRange(ranges[3], fontSize, 0x0451, 0x0451); // ё
    } else if (numRanges == 9) {
        initRange(ranges[0], fontSize, 0x0020, 0x0022); // punctuation [space ! "]
        initRange(ranges[1], fontSize, 0x0027, 0x0029); // punctuation [` ( )]
        initRange(ranges[2], fontSize, 0x002C, 0x002E); // punctuation [, - .]
        initRange(ranges[3], fontSize, 0x003A, 0x003B); // punctuation [: ;]
        initRange(ranges[4], fontSize, 0x003F, 0x003F); // punctuation [?]
        initRange(ranges[5], fontSize, 0x0049, 0x0049); // capital letter I 
        initRange(ranges[6], fontSize, 0x0061, 0x007A); // latin lowercase
        initRange(ranges[7], fontSize, 0x0430, 0x044F); // cyrillic lowercase
        initRange(ranges[8], fontSize, 0x0451, 0x0451); // ё 
    }
    int pr = stbtt_PackFontRanges(ctx, fontBuffer, 0, ranges, numRanges);
    free(fontBuffer);
    return ranges;
}

tfont::FontData tfont::packFonts(const List<FontDesc>& fonts, int atlasSize, bool saveAtlas) {
    tfont::FontData fontData {};
    fontData.atlasSize = atlasSize;
    stbtt_pack_context ctx;

    unsigned char* pixels = (unsigned char*)malloc(atlasSize * atlasSize * sizeof(unsigned char));
    memset(pixels, 0, sizeof(*pixels));

    int r = stbtt_PackBegin(&ctx, pixels, atlasSize, atlasSize, 0, 4, NULL);
    stbtt_PackSetOversampling(&ctx, 2, 2);
    for (auto& fontDesc : fonts) {
        fontData.fonts.push_back({fontDesc.numRanges, fontDesc.fontSize, fontDesc.xoffset, fontDesc.yoffset, packFont(&ctx, fontDesc.fontPath.c_str(), fontDesc.fontSize, fontDesc.numRanges)});
    }
    fontData.fontAtlas = gl::CreateImageSimpleFromMemory(ctx.pixels, ctx.width, ctx.height);
    stbtt_PackEnd(&ctx);

    if (saveAtlas) {
        stbi_write_png("fontatlas.png", ctx.width, ctx.height, 1, ctx.pixels, ctx.width);
    }

    return fontData;
}

