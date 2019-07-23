#pragma once
#include <NovusTypes.h>

class AngelScriptPlayer;
class AngelScriptSpell;
class AngelScriptAura;

// WARNING: Please consult with Pursche before using this, it's ugly and only used in scripts for unrolling a variadic template argument list
class any
{
public:
    enum Type
    {
        Void,
        Bool,
        I8,
        I16,
        I32,
        I64,
        U8,
        U16,
        U32,
        U64,
        F32,
        F64,
        String = 0x400000D,
        Player = 0x4000010,
        Spell = 0x4000014,
        Aura = 0x4000016
    }; // The order of types matches the order found in AngelScripts asETypeIdFlags
    static const size_t TYPECOUNT = 16;

    static constexpr const char* GetTypeName(Type type)
    {
        switch (type)
        {
        case Void:
            return "void";
            break;
        case Bool:
            return "bool";
            break;
        case I8:
            return "i8";
            break;
        case I16:
            return "i16";
            break;
        case I32:
            return "i32";
            break;
        case I64:
            return "i64";
            break;
        case U8:
            return "u8";
            break;
        case U16:
            return "u16";
            break;
        case U32:
            return "u32";
            break;
        case U64:
            return "u64";
            break;
        case F32:
            return "f32";
            break;
        case F64:
            return "f64";
            break;
        case String:
            return "string";
            break;
        case Player:
            return "AngelScriptPlayer";
            break;
        case Spell:
            return "AngelScriptSpell";
            break;
        case Aura:
            return "AngelScriptAura";
            break;
        }
        
        return "UNKNOWN";
    }
    static constexpr const char* GetTypeName(size_t type)
    {
        return GetTypeName(static_cast<Type>(type));
    }

    any(u8 e)
    {
        _data.U8 = e;
        _type = U8;
    }
    any(u16 e)
    {
        _data.U16 = e;
        _type = U16;
    }
    any(u32 e)
    {
        _data.U32 = e;
        _type = U32;
    }
    any(u64 e)
    {
        _data.U64 = e;
        _type = U64;
    }
    any(i8 e)
    {
        _data.I8 = e;
        _type = I8;
    }
    any(i16 e)
    {
        _data.I16 = e;
        _type = I16;
    }
    any(i32 e)
    {
        _data.I32 = e;
        _type = I32;
    }
    any(i64 e)
    {
        _data.I64 = e;
        _type = I64;
    }
    any(f32 e)
    {
        _data.F32 = e;
        _type = F32;
    }
    any(f64 e)
    {
        _data.F64 = e;
        _type = F64;
    }
    any(bool e)
    {
        _data.BOOL = e;
        _type = Bool;
    }
    any(std::string& e)
    {
        _string = e;
        _type = String;
    }
    any(AngelScriptPlayer* e)
    {
        _player = e;
        _type = Player;
    }
    any(AngelScriptSpell* e)
    {
        _spell = e;
        _type = Spell;
    }
    any(AngelScriptAura* e)
    {
        _aura = e;
        _type = Aura;
    }

    Type GetType() const { return _type; }
    u8 GetU8() const { return _data.U8; }
    u16 GetU16() const { return _data.U16; }
    u32 GetU32() const { return _data.U32; }
    u64 GetU64() const { return _data.U64; }
    i8 GetI8() const { return _data.I8; }
    i16 GetI16() const { return _data.I16; }
    i32 GetI32() const { return _data.I32; }
    i64 GetI64() const { return _data.I64; }
    f32 GetF32() const { return _data.F32; }
    f64 GetF64() const { return _data.F64; }
    bool GetBool() const { return _data.BOOL; }
    std::string& GetString() { return _string; }
    AngelScriptPlayer* GetPlayer() { return _player; }
    AngelScriptSpell* GetSpell() { return _spell; }
    AngelScriptAura* GetAura() { return _aura; }

private:
    Type _type;
    union {
        u8 U8;
        u16 U16;
        u32 U32;
        u64 U64;
        i8 I8;
        i16 I16;
        i32 I32;
        i64 I64;
        f32 F32;
        f64 F64;
        bool BOOL;
    } _data;
    std::string _string;
    AngelScriptPlayer* _player;
    AngelScriptSpell* _spell;
    AngelScriptAura* _aura;
};