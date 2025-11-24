#pragma once
#include "state.h"
#include "tentity.h"

template <typename T>
struct Thing {
    Entity entity {entt::null};

    T* Maybe() {
        return reg.try_get<T>(entity);
    }

    T& operator()() {
        if (reg.any_of<T>(entity)) {
            return reg.get<T>(entity);
        } else {
            tun::error("NO COMPONENT ON THING {}", entity);
            exit(1);
        }
    }
    
    operator Entity() {
        return entity;
    }
};
