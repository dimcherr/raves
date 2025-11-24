#pragma once
#include "tun/tgl.h"

namespace apip {

using PipFunc = sg_pipeline_desc (*)();

enum class Type {
    mesh,
    text,
    particle,
};

struct BasePip {
    PipFunc pipFunc {};
    sg_pipeline pip {};
    sg_bindings bind {};
    Type type {};

    BasePip(PipFunc pipFunc, Type type);
    void Use() {
        sg_apply_pipeline(pip);
    }
};

template <typename VS, typename FS>
struct Pip : BasePip {
    SOKOL_SHDC_ALIGN(16) VS vs {};
    SOKOL_SHDC_ALIGN(16) FS fs {};

    Pip(PipFunc pipFunc, Type type = Type::mesh) : BasePip(pipFunc, type) {}
    void Draw(const gl::DrawData& data, int instanceCount = 1) {
        sg_apply_uniforms(0, SG_RANGE(vs));
        sg_apply_uniforms(1, SG_RANGE(fs));
        bind.vertex_buffers[0] = data.vertexBuffer;
        bind.index_buffer = data.indexBuffer;
        sg_apply_bindings(&bind);
        sg_draw(0, data.elementCount, instanceCount);
    }
};

void CreatePips();

}
