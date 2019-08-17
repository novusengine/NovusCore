enum SpellEffectHooks
{
    HOOK_ON_SPELL_EFFECT_BEFORE_HIT,
    HOOK_ON_SPELL_EFFECT_AFTER_HIT
}

void OnSpellEffectLeap(Player player, Spell spell)
{
    Vector3 pos = player.GetPosition();
    float orientation = player.GetOrientation();
    float height = pos.z;

    uint32 mapId = player.GetMapId();
    Map map = GetMapFromId(mapId);

    uint32 dest = 20;
    for(uint32 i = 0; i < 20; i++)
    {
        Vector2 newPos = Vector2(pos.x + i * Math::Cos(orientation), pos.y + i * Math::Sin(orientation));
        float newHeight = map.GetHeight(newPos);
        float deltaHeight = Math::Abs(height - newHeight);

        if (deltaHeight <= 2.0f || (i == 0 && deltaHeight <= 20))
        {
            dest = i;
            height = newHeight;
        }
    }

    if (dest == 20)
    {
        // TODO: Send failed spell packet
        return;
    }

    /*
        Adding 2.0f to the final height will solve 90%+ of issues where we fall through the terrain, remove this to fully test blink's capabilities.
        This also introduces the bug where after a blink, you might appear a bit over the ground and fall down.
    */

    Vector3 newPos = Vector3(pos.x + dest * Math::Cos(orientation), pos.y + dest * Math::Sin(orientation), height);
    player.SetPosition(newPos);
}

void main()
{
    RegisterSpellEffectCallback(HOOK_ON_SPELL_EFFECT_AFTER_HIT, 29, OnSpellEffectLeap);
}