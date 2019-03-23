#pragma once
#include <NovusTypes.h>
#include "../DatabaseCache/CharacterDatabaseCache.h"

namespace CharacterUtils
{
    void GetDisplayIdFromRace(const CharacterData characterData, u32& displayId)
    {
        switch (characterData.race)
        {
            // Human
            case 1:
            {
                displayId = 49 + characterData.gender;
                break;
            }
            // Orc
            case 2:
            {
                displayId = 51 + characterData.gender;
                break;
            }
            // Dwarf
            case 3:
            {
                displayId = 53 + characterData.gender;
                break;
            }
            // Night Elf
            case 4:
            {
                displayId = 55 + characterData.gender;
                break;
            }
            // Undead
            case 5:
            {
                displayId = 57 + characterData.gender;
                break;
            }
            // Tauren
            case 6:
            {
                displayId = 59 + characterData.gender;
                break;
            }
            // Gnome
            case 7:
            {
                displayId = 1563 + characterData.gender;
                break;
            }
            // Gnome
            case 8:
            {
                displayId = 1478 + characterData.gender;
                break;
            }
            // Blood Elf
            case 10:
            {
                displayId = 15476 - characterData.gender;
                break;
            }
            // Draenie
            case 11:
            {
                displayId = 16125 + characterData.gender;
                break;
            }
        }
    }

}