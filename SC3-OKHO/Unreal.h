#pragma once

#define GAME_MODULE "splintercell3.exe"
#define GNAMES_OFFSET 0xA0DFC0
#define GOBJECTS_OFFSET 0xA12084
#define PROCESS_EVENT_OFFSET 0x3f4a90

typedef void(__thiscall* _ProcessEvent)(void* pThis, void* pFunction, void* pParams, void* pReturn);

class TArray
{
public:
	void* Data;
	int Count;
	int Max;
};

struct Vector
{
	float x, y, z;
};

class FName
{
private:
	char unk_00[0x0C];
	const char* string;
public:
	const char* GetString()
	{
		return (const char*)(&string);
	}
};

class GenericObject
{
public:
	void* vTable; // 0x00
	int InternalIndex; // 0x04
private:
	char unk_00[0x08]; // 0x08
	void* linkerLoad; // 0x10
	char unk_01[0x04]; // 0x14
public:
	GenericObject* Outer; // 0x18
private:
	int objectFlags; // 0x1C
public:
	int FName; // 0x20
	GenericObject* Class; // 0x24
	GenericObject* SuperField; // 0x28
	GenericObject* Next; // 0x2C
	GenericObject* HashNext; // 0x30
};

class GenericProperty : GenericObject
{
private:
	char unk_02[0x10]; // 0x34
public:
	int Offset; // 0x44
private:
	char unk_03[0x30]; // 0x48
public:
	int TypeSpecific; // 0x78   BoolProperty: Bitmask, ByteProperty: Enum
};