#include "asset/apip.h"

namespace apip {

static List<BasePip*> pips {};

BasePip::BasePip(PipFunc pipFunc, Type type) : pipFunc(pipFunc), type(type) {
    pips.push_back(this);
}

void CreatePips() {
    tlogpush();

    for (auto* pip : pips) {
        switch (pip->type) {
            case apip::Type::mesh:
                break;
            case apip::Type::text:
                pip->bind.vertex_buffers[1] = gl::CreateVertexBufferTextParticle();
                break;
            case apip::Type::particle:
                pip->bind.vertex_buffers[2] = gl::CreateVertexBufferParticle();
                break;
        }
        pip->pip = sg_make_pipeline(pip->pipFunc());
    }

    tlogpop("pips create");
}

}
