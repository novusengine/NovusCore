#include "CharacterPacketUtils.h"
#include "../../NovusEnums.h"
#include "../../MessageHandler.h"
#include "../../WorldNodeHandler.h"
#include "../../ECS/Components/Singletons/CharacterDatabaseCacheSingleton.h"
#include "../../ECS/Components/PlayerConnectionComponent.h"

#include "../../Utils/ServiceLocator.h"
#include <tracy/Tracy.hpp>

void CharacterPacketUtils::RegisterCharacterPacketHandlers()
{
    MessageHandler* messageHandler = MessageHandler::Instance();

}