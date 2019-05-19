#pragma once
#include <NovusTypes.h>

// WARNING: Please consult with Pursche before using this, it's ugly and only used in scripts for unrolling a variadic template argument list
class any {
public:
	enum Type { Void, Bool, I8, I16, I32, I64, U8, U16, U32, U64,  F32, F64, String = 0x400000D}; // The order of types matches the order found in AngelScripts asETypeIdFlags
	static const size_t TYPECOUNT = 13;
	
	static constexpr const char* GetTypeName(Type type)
	{
		switch (type)
		{
		case Void: return "void"; break;
		case Bool: return "bool"; break;
		case I8: return "i8"; break;
		case I16: return "i16"; break;
		case I32: return "i32"; break;
		case I64: return "i64"; break;
		case U8: return "u8"; break;
		case U16: return "u16"; break;
		case U32: return "u32"; break;
		case U64: return "u64"; break;
		case F32: return "f32"; break;
		case F64: return "f64"; break;
		case String: return "string"; break;
		}
		return "UNKNOWN";
	}
	static constexpr const char* GetTypeName(size_t type)
	{
		return GetTypeName(static_cast<Type>(type));
	}

	any(u8 e) { m_data.U8 = e; m_type = U8; }
	any(u16 e) { m_data.U16 = e; m_type = U16; }
	any(u32 e) { m_data.U32 = e; m_type = U32; }
	any(u64 e) { m_data.U64 = e; m_type = U64; }
	any(i8 e) { m_data.I8 = e; m_type = I8; }
	any(i16 e) { m_data.I16 = e; m_type = I16; }
	any(i32 e) { m_data.I32 = e; m_type = I32; }
	any(i64 e) { m_data.I64 = e; m_type = I64; }
	any(f32 e) { m_data.F32 = e; m_type = F32; }
	any(f64 e) { m_data.F64 = e; m_type = F64; }
	any(bool e) { m_data.BOOL = e; m_type = Bool; }
	any(std::string& e) { m_string = e; m_type = String; }

	Type get_type() const { return m_type; }
	u8 get_u8() const { return m_data.U8; }
	u16 get_u16() const { return m_data.U16; }
	u32 get_u32() const { return m_data.U32; }
	u64 get_u64() const { return m_data.U64; }
	i8 get_i8() const { return m_data.I8; }
	i16 get_i16() const { return m_data.I16; }
	i32 get_i32() const { return m_data.I32; }
	i64 get_i64() const { return m_data.I64; }
	f32 get_f32() const { return m_data.F32; }
	f64 get_f64() const { return m_data.F64; }
	bool get_bool() const { return m_data.BOOL; }
	std::string& get_string() { return m_string; }

private:
	Type m_type;
	union {
		u8	U8;
		u16	U16;
		u32	U32;
		u64	U64;
		i8	I8;
		i16	I16;
		i32	I32;
		i64	I64;
		f32 F32;
		f64 F64;
		bool BOOL;
	} m_data;
	std::string m_string;
};