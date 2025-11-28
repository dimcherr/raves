#pragma once
#include "cgltf.h"
#include "tun/tstring.h"
#include "tun/tlist.h"
#include "tun/tgl.h"

namespace tgltf {
    struct StringParam {
        String key {};
        String value {};
    };

    struct NumberParam {
        String key {};
        float value {};
    };

    struct ModelParams {
        List<StringParam> strings {};
        List<NumberParam> numbers {};

        String getStringParam(StringView key) const;
        float getNumberParam(StringView key) const;
    };

    struct ModelDesc {
        String category {}; 
        void(*func)(const ModelParams& params);
        bool isPBR {true};
    };

    struct Node {
        String name {};
        Vec translation {};
        Quat rotation {};
        Vec scale {};
        tgl::Model* model {};
        tgl::Light* light {};
        Node* parent {};
        List<Node*> children {};
        String category {};
        ModelParams params {};
    };

    struct File {
        cgltf_data* data {};
        Map<cgltf_texture*, tgl::Texture*> textures {};
        Map<cgltf_material*, tgl::Material*> materials {};
        Map<cgltf_primitive*, tgl::Mesh*> meshes {};
        Map<cgltf_mesh*, tgl::Model*> models {};
        List<tgl::Prim*> prims {};
        List<Node*> nodes {};
    };

    void open(StringView path);
}

