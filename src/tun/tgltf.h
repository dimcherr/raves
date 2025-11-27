#pragma once
#include "cgltf.h"
#include "tun/tstring.h"
#include "tun/tlist.h"

namespace tgltf {
    struct StringParam {
        String key {};
        String value {};
    };

    struct FloatParam {
        String key {};
        float value {};
    };

    struct ModelParams {
        List<StringParam> strings {};
        List<FloatParam> floats {};

        String GetStringParam(StringView key) const;
        float GetFloatParam(StringView key) const;
    };

    struct ModelDesc {
        String category {}; 
        void(*func)(const ModelParams& params);
        bool isPBR {true};
    };
}

