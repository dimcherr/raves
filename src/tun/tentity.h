#pragma once
#include "entt.hpp"
#include "tun/tlog.h"

inline entt::registry reg {};

struct Entity {
    entt::entity value {entt::null};

    Entity() = default;
    Entity(const entt::entity& value) : value(value) {}
    operator entt::entity() const {
        return value;
    }
};

template <>
struct std::formatter<Entity> : std::formatter<std::string> {
    auto format(Entity entity, format_context& ctx) const {
        return formatter<string>::format(std::format("[Entity={}]", (int)entity.value), ctx);
    }
};

template <>
struct std::formatter<entt::entity> : std::formatter<std::string> {
    auto format(entt::entity entity, format_context& ctx) const {
        return formatter<string>::format(std::format("[Entity={}]", (int)entity), ctx);
    }
};

