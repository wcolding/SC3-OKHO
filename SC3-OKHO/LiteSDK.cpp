#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include "LiteSDK.h"
#include <format>

LiteSDK::LiteSDK()
{
	BaseAddress = (int)GetModuleHandleA(GAME_MODULE);
	ProcessEvent = (_ProcessEvent)(BaseAddress + PROCESS_EVENT_OFFSET);
	GNames = (TArray*)(BaseAddress + GNAMES_OFFSET);
	GObjects = (TArray*)(BaseAddress + GOBJECTS_OFFSET);
}

LiteSDK::GameObject LiteSDK::GetGameObject(std::string name, bool useFullName)
{
	int curAddr = 0;
	GenericObject* curObjPtr;

	for (int i = 0; i < GObjects->Count; i++)
	{
		curAddr = (int)GObjects->Data + (i * 4);
		if (curAddr == 0)
			continue;

		curObjPtr = (GenericObject*)(*(GenericObject**)curAddr);
		GameObject curObj(curObjPtr, this);

		std::string curObjName;
		if (useFullName)
			curObjName = curObj.GetFullName();
		else
			curObjName = curObj.GetName();

		std::string nameStr = std::string(name);

		if (nameStr.compare(curObjName) == 0)
			return curObj;
	}

	return GameObject(nullptr, this);
}

LiteSDK::GameObject LiteSDK::GetBaseObject(std::string name)
{
	return GetGameObject(name, true);
}

std::vector<LiteSDK::GameObject> LiteSDK::GetInstances(std::string className)
{
	std::vector<GameObject> instances;

	auto classObj = GetBaseObject(className);

	if (classObj.ObjectPtr == nullptr)
		classObj = GetGameObject(className, false); // try partial name if full name fails

	if (classObj.ObjectPtr != nullptr)
	{
		int curAddr = 0;
		GenericObject* curObjPtr = nullptr;

		for (int i = 0; i < GObjects->Count; i++)
		{
			curAddr = (int)GObjects->Data + (i * 4);
			if (curAddr == 0)
				continue;

			curObjPtr = (GenericObject*)(*(GenericObject**)curAddr);
			
			if (curObjPtr != nullptr)
				if (curObjPtr->Class == classObj.ObjectPtr)
				{
					GameObject newObj(curObjPtr, this);
					instances.push_back(newObj);
				}
		}
	}

	return instances;
}

LiteSDK::GameObject LiteSDK::GetFirstInstance(std::string className)
{
	auto instances = GetInstances(className);
	if (instances.empty())
		return GameObject(nullptr, this);

	return instances[0];
}

LiteSDK::GameObject LiteSDK::GetNamedInstance(std::string name, std::string className)
{
	auto instances = GetInstances(className);
	if (instances.empty())
		return GameObject(nullptr, this);

	std::string curName;

	for (auto instance : instances)
	{
		curName = instance.GetName();
		if (name.compare(curName) == 0)
			return instance;

		curName = instance.GetFullName();
		if (name.compare(curName) == 0)
			return instance;
	}

	return GameObject(nullptr, this);
}

std::string LiteSDK::ResolveObjectName(GenericObject* object, bool recursive) 
{
	if (object == nullptr)
		return "";

	int masked = object->FName & 0x7FFFF;

	if (masked > GNames->Count)
		return "";

	std::string workingStr = GetNameFromIndex(masked);
	
	if ((object->Outer != nullptr) && recursive)
	{
		workingStr = std::format("{}.{}", ResolveObjectName((GenericObject*)object->Outer), workingStr);
	}

	int instanceNum = object->FName >> 0x13;
	if (instanceNum > 0)
		workingStr = std::format("{}_{}", workingStr, instanceNum - 1);

	return workingStr;
}

void LiteSDK::GameObject::ProcessEvent(void* pFunction, void* pParams, void* pReturn)
{
	sdk->ProcessEvent(ObjectPtr, pFunction, pParams, pReturn);
}

void LiteSDK::GameObject::ProcessEvent(GameObject functionObj, void* pParams, void* pReturn)
{
	sdk->ProcessEvent(ObjectPtr, functionObj.ObjectPtr, pParams, pReturn);
}

std::vector<std::string> LiteSDK::GetNames()
{
	std::vector<std::string> names;
	std::string curString;

	for (int i = 0; i < GNames->Count; i++)
	{
		curString = GetNameFromIndex(i);
		if (!curString.empty())
			names.push_back(curString);
	}
	
	return names;
}

std::string LiteSDK::GetNameFromIndex(int index)
{
	if (index >= GNames->Count)
		return {};

	FName* fName = (FName*)(*(FName**)((int)GNames->Data + (index * 4)));
	if (fName != nullptr)
		return std::string(fName->GetString());
	else
		return {};
}

int LiteSDK::GetIndexOfName(std::string name)
{
	std::string curString;

	for (int i = 0; i < GNames->Count; i++)
	{
		curString = GetNameFromIndex(i);
		if (!curString.empty())
		{
			if (curString.compare(name) == 0)
				return i;
		}
	}

	return INVALID_NAME_INDEX;
}

bool LiteSDK::ValidateObjects()
{
	if (nullCount == 0)
		return true;
	
	nullCount = 0;
	return false;
}

std::string LiteSDK::GameObject::GetName()
{
	return sdk->ResolveObjectName(ObjectPtr, false);
}

std::string LiteSDK::GameObject::GetFullName()
{
	return sdk->ResolveObjectName(ObjectPtr, true);
}

LiteSDK::ObjectProperty* LiteSDK::GameObject::GetProperty(std::string propertyName)
{
	auto propertyDef = (ObjectProperty*)sdk->GetGameObject(propertyName, true).ObjectPtr; // todo: validate properties and make this false
	return propertyDef;
}

bool LiteSDK::GameObject::GetBool(std::string boolPropertyName)
{
	auto propertyObj = GetProperty(boolPropertyName);
	return (GetPropertyValue<int>(boolPropertyName) & propertyObj->TypeSpecific) != 0;
}

void LiteSDK::GameObject::SetBool(std::string boolPropertyName, bool value)
{
	auto propertyObj = GetProperty(boolPropertyName);
	int bitfield = GetPropertyValue<int>(boolPropertyName);
	bitfield |= propertyObj->TypeSpecific;

	if (!value)
		bitfield ^= propertyObj->TypeSpecific;

	SetPropertyValue<int>(boolPropertyName, bitfield);
}

LiteSDK::GameObject::GameObject(GenericObject* objPtr, LiteSDK* sdkPtr)
{
	ObjectPtr = objPtr;
	sdk = sdkPtr;

	if (objPtr == nullptr)
		sdk->nullCount++;
}

bool LiteSDK::GameObject::IsNotNull()
{
	return ObjectPtr != nullptr;
}