#pragma once
#include <functional>
#include "sokol_gfx.h"
#include "tun/tmath.h"
#include "tun/tcolor.h"
#include "tun/tfile.h"

namespace tgl {
    struct Texture {
        String name {};
        sg_image image {};
    };

    struct Material {
        String name {};
        Texture* texAlbedo {};
        Texture* texNormal {};
        Texture* texORM {};
        Texture* texEmissive {};
        float metallicFactor {1.f};
        float roughnessFactor {1.f};
        float normalScale {1.f};
        Color emissiveFactor {tcolor::white};
    };

    struct Model {
        BoundingBox boundingBox {};
        Vec offset {};
        List<struct Mesh*> meshes {};
    };

    struct Mesh {
        Model* model {};
        Material* material {};
        sg_buffer vertexBuffer {};
        sg_buffer indexBuffer {};
    };

    struct Prim {
        String name {};
        sg_buffer vertexBuffer {};
        sg_buffer indexBuffer {};
    };

    struct Light {
        Color color {};
        float intensity {};
    };

    void create();
    void update();
    void destroy();

    sg_buffer createVertexBuffer(const List<float>& vertices);
    sg_buffer createIndexBuffer(const List<uint16_t>& indices);
    sg_image createImageFromMemory(const List<Byte>& bytes);
}

