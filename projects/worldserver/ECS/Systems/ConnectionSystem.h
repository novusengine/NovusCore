#pragma once
#include <entt.hpp>
#include <NovusTypes.h>

#include "../Connections/NovusConnection.h"
#include "../Components/ConnectionComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/PositionComponent.h"

namespace ConnectionSystem
{
    template <typename T>
    void SetFieldValue(PlayerUpdateDataComponent& updateData, u16 index, T value, u8 offset = 0)
    {
        updateData.playerFields.WriteAt<T>(value, (index * 4) + offset);
        updateData.updateMask.SetBit(index);
    }
    void Update(entt::registry &registry);
}
