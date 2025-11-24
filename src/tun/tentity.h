#pragma once
#include "entt.hpp"
#include "tun/tlog.h"

using Entity = entt::entity;

template <>
struct std::formatter<Entity> : std::formatter<std::string> {
    auto format(Entity entity, format_context& ctx) const {
        return formatter<string>::format(std::format("[Entity={}]", (int)entity), ctx);
    }
};
