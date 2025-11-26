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

struct Unit {
    virtual void create() {}
    virtual void preUpdate() {}
    virtual void update() {}
    virtual void drawGeometry() {}
    virtual void drawLighting() {}
    virtual void drawCombined() {}
    virtual void drawUI() {}
    virtual void drawMain() {}
    virtual void postUpdate() {}
};

inline List<Unit*> units {};

template <typename T>
void addUnit(T*& ptr) {
    ptr = new T();
    units.push_back(ptr);
}

template <typename T>
struct Thing {
    Entity entity {};

    T* maybe() {
        return reg.try_get<T>(entity);
    }

    T& operator()() {
        if (reg.any_of<T>(entity)) {
            return reg.get<T>(entity);
        } else {
            terror("no component on thing {}", entity);
            exit(1);
        }
    }

    operator bool() {
        return reg.any_of<T>(entity);
    }
    
    operator Entity() {
        return entity;
    }

    operator entt::entity() {
        return entity.value;
    }
};

