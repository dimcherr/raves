#pragma once
#include "cgltf.h"
#include "tun/tfile.h"
#include "tun/tentity.h"
#include "tun/tmath.h"
#include "tun/tgl.h"
#include "tun/tstring.h"

namespace gltf {

struct StringParam {
    String key {};
    String value {};
};

struct FloatParam {
    String key {};
    float value {};
};

struct ModelParams {
    Entity entity {entt::null};
    Entity modelAsset {entt::null};
    List<StringParam> strings {};
    List<FloatParam> floats {};

    String GetStringParam(StringView key) const {
        for (auto& p : strings) {
            if (p.key == key) {
                return p.value;
            }
        }
        return "";
    }

    float GetFloatParam(StringView key) const {
        for (auto& p : floats) {
            if (p.key == key) {
                return p.value;
            }
        }
        return 0.f;
    }
};

struct ModelDesc {
    String category {}; 
    Entity(*func)(const ModelParams& params);
    bool isPBR {true};
};

struct Texture {
    Entity entity {entt::null};
    String name {};
    gl::Image image {};

    Texture(StringView name, gl::Image image) : name(name), image(image) {}
};

struct Material {
    Entity entity {entt::null};
    String name {};
    std::shared_ptr<Texture> baseColorTexture {};
    std::shared_ptr<Texture> normalTexture {};
    std::shared_ptr<Texture> ormTexture {};
    std::shared_ptr<Texture> emissiveTexture {};
    float metallicFactor {1.f};
    float roughnessFactor {1.f};
    float normalScale {1.f};
    Color emissiveFactor {tun::white};
};

struct Model {
    Entity entity {entt::null};
    BoundingBox boundingBox {};
    Vec offset {};
    List<std::shared_ptr<struct Mesh>> meshes {};
};

struct Mesh {
    Entity entity {entt::null};
    std::shared_ptr<Model> model {};
    std::shared_ptr<Material> material {};
    List<float> vertices {};
    List<uint16_t> indices {};
    bool skinned {false};
};

struct Light {
    Color color {};
    float intensity {};
};

struct Node {
    Entity entity {entt::null};
    String name {};
    Vec translation {};
    Quat rotation {};
    Vec scale {};
    std::shared_ptr<Model> model {};
    std::shared_ptr<Light> light {};
    std::shared_ptr<Node> parent {};
    List<std::shared_ptr<Node>> children {};

    String category {};
    ModelParams params {};
};

struct Prim {
    String name {};
    List<float> vertices {};
    List<uint16_t> indices {};
};

struct File {
    cgltf_data* data {};
    Map<cgltf_texture*, std::shared_ptr<Texture>> textures {};
    Map<cgltf_material*, std::shared_ptr<Material>> materials {};
    Map<cgltf_primitive*, std::shared_ptr<Mesh>> meshes {};
    Map<cgltf_mesh*, std::shared_ptr<Model>> models {};
    List<std::shared_ptr<Node>> nodes {};
    List<std::shared_ptr<Prim>> prims {};

    File(StringView path, bool onlyPrimitives = false);

    ~File() {
        if (data) {
            cgltf_free(data);
        }
    }
};

/* Function to parse and print extras JSON for a given cgltf_data and extras field */
char* parse_extras(const cgltf_data* data, const cgltf_extras* extras);


}