#pragma once
#include "Unreal.h"
#include <string>
#include <vector>
#include <type_traits>

#define INVALID_NAME_INDEX -1

class LiteSDK
{
private:
	int BaseAddress;
	TArray* GNames;
	TArray* GObjects;

public:
	class GameObject; 
	class ObjectProperty;

	LiteSDK();

	/** Searches game memory for an explicitly-named object.
	* This is best used to find non-instance objects such as functions.
	 \param name the full name of the object (including outer objects)

	 \returns    A GameObject with a pointer to a GenericObject in memory. If an object was not found, the ObjectPtr will be nullptr
	*/
	GameObject GetBaseObject(std::string name);

	/** Finds all instances of a certain class
	 \param className the name of the class you are looking for

	 \returns    A list of all GameObjects for which the class is the specified name
	*/
	std::vector<GameObject> GetInstances(std::string className);

	/** Finds the first instance of a certain class
	 \param className the name of the class you are looking for

	 \returns    A GameObject with a pointer to the first GenericObject of type 'className' in memory. If an object was not found, the ObjectPtr will be nullptr
	*/
	GameObject GetFirstInstance(std::string className);

	/** Finds a specific instance of a certain class. Best used to find level-specific objects such as meshes.
	 \param name the name of the instance you are trying to locate
	 \param className the name of the class you are looking for

	 \returns    A GameObject with a pointer to the requested GenericObject of type 'className' in memory. If an object was not found, the ObjectPtr will be nullptr
	*/
	GameObject GetNamedInstance(std::string name, std::string className);

	_ProcessEvent ProcessEvent;

	/** Finds all name entries
	 \returns    A list of strings used by the game
	*/
	std::vector<std::string> GetNames();

	/** Finds a string from GNames of a given index
	 \param index the string index in GNames

	 \returns    A string. If one is not found at the specified index, an empty string is returned
	*/
	std::string GetNameFromIndex(int index);

	/** Finds the index of a string from GNames
	 \param name the string to look for

	 \returns    The index of the specified string. If the string is not found in GNames, INVALID_NAME_INDEX is returned
	*/
	int GetIndexOfName(std::string name);

	/** Checks whether any GameObjects have been created with null GenericObjects since the last time this was called
	 \returns    True if all objects since the last call have valid GenericObject pointers, false if not
	*/
	bool ValidateObjects();

	/** Class that holds a pointer to a GenericObject in memory
	 * 
	 */
	class GameObject
	{
	private:
		LiteSDK* sdk;
		ObjectProperty* GetProperty(std::string propertyName);

	public:
		GameObject(GenericObject* objPtr, LiteSDK* sdkPtr);
		GenericObject* ObjectPtr;


		/** Executes a function as this object
		 \param pFunction a pointer to the Unreal function object you want to call
		 \param pParams a pointer to a structure containing the parameters needed by the function
		 \param pReturn a pointer to where the function will return a value
		*/
		void ProcessEvent(void* pFunction, void* pParams, void* pReturn);

		/** Executes a function as this object
		 \param functionObj a GameObject instance of the Unreal function object you want to call
		 \param pParams a pointer to a structure containing the parameters needed by the function
		 \param pReturn a pointer to where the function will return a value
		*/
		void ProcessEvent(GameObject functionObj, void* pParams, void* pReturn);

		/** Gets the simple name of this object
		\returns	the name of the object with no outer objects included
		*/
		std::string GetName();

		/** Gets the full name of this object
		\returns	the name of the object with all outer objects included
		*/
		std::string GetFullName();

		/** Gets the value of a named property on an object
		\returns	the value as type T
		*/
		template <typename T>
		T GetPropertyValue(std::string propertyName)
		{
			auto propertyObj = GetProperty(propertyName);
			T* ptr = (T*)((int)ObjectPtr + propertyObj->Offset);
			return *(T*)ptr;
		}

		/** Sets the value of a named property on an object
		*/
		template <typename T>
		void SetPropertyValue(std::string propertyName, T value)
		{
			auto propertyObj = GetProperty(propertyName);
			T* ptr = (T*)((int)ObjectPtr + propertyObj->Offset);
			memcpy(ptr, &value, sizeof(T));
		}

		/** Gets the value at an offset from an object
		\returns	the value as type T
		*/
		template <typename T>
		T GetValueAt(int offset)
		{
			T* ptr = (T*)((int)ObjectPtr + offset);
			return *(T*)ptr;
		}

		/** Gets the value of a named BoolProperty on an object
		\returns	the value as a bool
		*/
		bool GetBool(std::string boolPropertyName);

		/** Sets the value of a named BoolProperty on an object
		*/
		void SetBool(std::string boolPropertyName, bool value);

		/** Checks for a null pointer on the GenericObject
		\returns	whether the internal GenericObject is a non-null value
		*/
		bool IsNotNull();

	};

	class ObjectProperty : public GenericProperty
	{

	};

private:
	std::string ResolveObjectName(GenericObject* object, bool recursive = true);
	GameObject GetGameObject(std::string name, bool useFullName = true);

	int nullCount = 0;
};