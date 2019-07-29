#include "SpellHooks.h"

std::array<std::vector<asIScriptFunction*>, SpellHooks::COUNT> SpellHooks::_hooks;
robin_hood::unordered_map<u32, std::array<std::vector<asIScriptFunction*>, SpellEffectHooks::Hooks::COUNT>> SpellEffectHooks::_hooks;