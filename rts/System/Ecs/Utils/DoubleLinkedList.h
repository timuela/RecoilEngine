/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef ECS_DOUBLE_LINKED_LIST_H_
#define ECS_DOUBLE_LINKED_LIST_H_

#include "System/Ecs/EcsMain.hpp"

namespace ecs_dll {

template<class RegType = entt::registry>
class DoubleLinkList {

public:

    DoubleLinkList(RegType& reg)
        : registry(reg)
    {}

template<class T, typename EntityType>
void InsertChain(EntityType head, EntityType newLink) {
    auto& nextChain = registry.template get<T>(head);
    auto& prevChain = registry.template get<T>(nextChain.prev);

    // LOG("%s: added chain link with %x <-> [%x] <-> %x.", __func__
    //         , entt::to_integral(nextChain.prev)
    //         , entt::to_integral(newLink)
    //         , entt::to_integral(prevChain.next));

    registry.template emplace_or_replace<T>(newLink, nextChain.prev, prevChain.next);
    prevChain.next = newLink;
    nextChain.prev = newLink;
}

template<class T, typename EntityType>
void RemoveChain(EntityType removeLink) {
    auto& curChain = registry.template get<T>(removeLink);

    auto& nextChain = registry.template get<T>(curChain.next);
    auto& prevChain = registry.template get<T>(curChain.prev);

    // LOG("%s: removed chain link from %x. Linking %x <-> %x.", __func__
    //         , entt::to_integral(removeLink)
    //         , entt::to_integral(curChain.prev)
    //         , entt::to_integral(curChain.next));

    prevChain.next = curChain.next;
    nextChain.prev = curChain.prev;

    registry.template remove<T>(removeLink);
}

// walk the chain list
template<class T, typename F, typename EntityType>
void ForEachInChain(EntityType head, F&& func) {
    // for (auto* chainLink = &registry.get<T>(head);
    //     chainLink->next != head;
    //     chainLink = &registry.get<T>(chainLink->next)
    // ) {
    //     LOG("%s: walking chain link %x [head %x]", __func__
    //             , entt::to_integral(chainLink->next)
    //             , entt::to_integral(head));
    //     func(chainLink->next);
    // }

    auto curEntity = head;
    do {
        // LOG("%s: walking chain link %x [head %x]", __func__
        //         , entt::to_integral(curEntity)
        //         , entt::to_integral(head));
        func(curEntity);

        auto* chainLink = &registry.template get<T>(curEntity);
        curEntity = chainLink->next;
    } while (curEntity != head);
}

template<class T, typename F, typename EntityType>
void BackWalkWithEarlyExit(EntityType head, F&& func) {
    auto curEntity = head;
    do {
        // LOG("%s: walking chain link %x [head %x]", __func__
        //         , entt::to_integral(curEntity)
        //         , entt::to_integral(head));
        if (!func(curEntity)) { break; }

        auto* chainLink = &registry.template get<T>(curEntity);
        curEntity = chainLink->prev;
    } while (curEntity != head);
}

private:
    RegType& registry;

};

}

#endif