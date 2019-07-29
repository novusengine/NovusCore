#include "AuraHooks.h"

std::array<std::vector<asIScriptFunction*>, AuraHooks::Hooks::COUNT> AuraHooks::_hooks;
robin_hood::unordered_map<u32, std::array<std::vector<asIScriptFunction*>, AuraEffectHooks::Hooks::COUNT>> AuraEffectHooks::_hooks;