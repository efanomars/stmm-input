/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   capability.h
 */

#ifndef STMI_CAPABILITY_H
#define STMI_CAPABILITY_H

#include "private-namedtypes.h"

#include <typeinfo>
#include <type_traits>
#include <string>
#include <memory>
#include <atomic>
#include <iostream>
#include <cassert>

namespace stmi
{

using std::shared_ptr;

class Device;
class DeviceManager;

class DeviceManagerCapability;

/** Base capability class. */
class Capability
{
public:
	virtual ~Capability() = default;

	/** The capability id.
	 * @return The unique id of the capability.
	 */
	int32_t getId() const { return m_nCapabilityId; }

	/** Returns the device owning this capability, if any.
	 * If it's a device manager capability this might return null.
	 * It also can be null if the device was deleted.
	 */
	virtual shared_ptr<Device> getDevice() const = 0;

	/** The representation of a registered capability class.
	 */
	class Class final
	{
	public:
		/** Constructs an empty capability class instance.
		 */
		Class()
		: m_p0CapabilityType(nullptr)
		, m_bIsDeviceManagerCapability(false)
		{
		}
		/** Constructs an capability class instance.
		 * If the passed c++ typeid is not of a registered class an empty
		 * class is constructed.
		 * @param oCapabilityType underlying c++ typeid of the class.
		 */
		Class(const std::type_info& oCapabilityType)
		: Class(oCapabilityType, false)
		{
		}
		/** Compares a registered capability class instance with a c++ typeid.
		 * If the class instance is empty `false` is returned.
		 * @param oOtherCapabilityType The c++ typeid.
		 * @return The result of the comparison.
		 */
		inline bool operator==(const std::type_info& oOtherCapabilityType) const
		{
			if (m_p0CapabilityType == nullptr) {
				return false;
			}
			return ((*m_p0CapabilityType) == oOtherCapabilityType);
		}
		/**
		 * @see operator==(const std::type_info&)
		 */
		inline bool operator!=(const std::type_info& oOtherCapabilityType) const
		{
			return !operator==(oOtherCapabilityType);
		}
		/** Compares a registered capability class to another.
		 * If both are empty `true` is returned.
		 * @param oOther The other class instance. Might be empty.
		 * @return  The result of the comparison.
		 */
		inline bool operator==(const Class& oOther) const
		{
			if (m_p0CapabilityType == nullptr) {
				return (oOther.m_p0CapabilityType == nullptr);
			}
			return ((*m_p0CapabilityType) == (*oOther.m_p0CapabilityType));
		}
		/**
		 * @see operator==(const Class&)
		 */
		inline bool operator!=(const Class& oOther) const
		{
			return !operator==(oOther);
		}
		/** Tells whether class is registered.
		 * @return Whether class is not empty.
		 */
		inline operator bool() const
		{
			return (m_p0CapabilityType != nullptr);
		}
		/** The underlying c++ typeid of the class.
		 * @return The c++ type or if class is empty an invalid value.
		 * @see operator bool() const
		 */
		inline const std::type_info& getTypeInfo() const
		{
			return *m_p0CapabilityType;
		}
		/** The class id.
		 * If the class is empty returns empty string.
		 * @return The class id.
		 */
		inline std::string getId() const
		{
			if (m_p0CapabilityType == nullptr) {
				return "";
			}
			return getCapabilityTypeClassId(*m_p0CapabilityType);
		}
		/** Tells whether you can statically cast an instance of this Class to DeviceManagerCapability.
		 * @see class DeviceManagerCapability
		 * @return Whether this registered class is subclass of DeviceManagerCapability.
		 */
		inline bool isDeviceManagerCapability() const
		{
			return m_bIsDeviceManagerCapability;
		}
	private:
		Class(const std::type_info& oCapabilityType, bool bIsDeviceManagerCapability)
		: m_p0CapabilityType((nullptr != getNamedTypes().getDataFromType(oCapabilityType, bIsDeviceManagerCapability)) ? &oCapabilityType : nullptr)
		, m_bIsDeviceManagerCapability(bIsDeviceManagerCapability)
		{
			if (m_p0CapabilityType == nullptr) {
				std::cout << "Capability::Class(oCapabilityType) oCapabilityType '" << oCapabilityType.name() << "' not registered!" << std::endl;
			}
		}
	private:
		const std::type_info* m_p0CapabilityType;
		bool m_bIsDeviceManagerCapability;
	};
	/** Get the registered class of the capability instance.
	 * @return The registered class. Cannot be empty.
	 */
	inline const Class& getCapabilityClass() const
	{
		return m_oClass;
	}
	/** Tells whether the given capability class id is registered.
	 * @param sCapabilityClassId The class id.
	 * @return Whether registered.
	 */
	static bool isCapabilityClassIdRegistered(const std::string& sCapabilityClassId);
	/** Gets the class with given class id.
	 * @param sCapabilityClassId The registered class id.
	 * @return The class. Might be empty if sCapabilityClassId is not registered.
	 */
	static Class getCapabilityClassIdClass(const std::string& sCapabilityClassId);

protected:
	/** Constructor to be called from subclasses.
	 * The registered type has to be the actual class of the instance being constructed
	 * or a superclass of it.
	 * @param oClass The registered class of the capability. Cannot be empty.
	 */
	Capability(const Class& oClass);
	/** Template used to register capability classes.
	 * In a subclass of Capability you want to register define (i.e. KeysCapability):
	 *
	 *     class KeysCapability : public Capability {
	 *     public:
	 *       ...
	 *       static const char* const s_sClassId;
	 *       static const Capability::Class& getClass() { return s_oInstall.getCapabilityClass(); }
	 *     private:
	 *       static RegisterClass<KeysCapability> s_oInstall;
	 *       ...
	 *     }
	 *
	 * with the instantiation
	 *
	 *     const char* const KeysCapability::s_sClassId = "KeysCapability";
	 *     // this instance registers the class!
	 *     Capability::RegisterClass<KeysCapability> KeysCapability::s_oInstall(s_sClassId);
	 *
	 * Beware! No registered class should be subclass of another registered class!
	 */
	template<class T>
	class RegisterClass final
	{
	public:
		/** Class registration constructor.
		 * The class id must be a statically defined unique ascii string with length > 0.
		 * @param p0CapabilityClassId The class id. Can't be nullptr.
		 */
		RegisterClass(const char* const &p0CapabilityClassId)
		{
			static_assert(std::is_base_of<Capability,T>::value, "Wrong type.");
			static_assert(!std::is_same<Capability,T>::value, "Wrong type.");
			static_assert(!std::is_same<DeviceManagerCapability,T>::value, "Wrong type.");
			assert(p0CapabilityClassId != nullptr);
			assert((*p0CapabilityClassId) != static_cast<char>(0));
			//assert(!getNamedTypes().hasType(typeid(T)));
			if (!getNamedTypes().hasType(typeid(T))) {
				getNamedTypes().addType(typeid(T), p0CapabilityClassId, std::is_base_of<DeviceManagerCapability,T>::value);
			}
			m_oFirstInstanceClass = Class(typeid(T));
		}
		~RegisterClass()
		{
		}
		/** Get the class of the registered type.
		 * @return A non-empty class.
		 */
		inline const Class& getCapabilityClass()
		{
			return m_oFirstInstanceClass;
		}
	private:
		Capability::Class m_oFirstInstanceClass;
	private:
		RegisterClass() = delete;
	};
	/** Tells whether a capability type was registered.
	 * @param oCapabilityType The c++ typeid of a class.
	 * @return Whether the class was registered.
	 */
	static bool isCapabilityTypeRegistered(const std::type_info& oCapabilityType);
	/** Gets the type of the registered capability subclass associated with class id.
	 * @param sCapabilityClassId The type string.
	 * @param bRegistered Whether the return value is valid.
	 * @return The registered type. If bRegistered is `false` the result is undefined.
	 */
	static const std::type_info& getCapabilityClassIdType(const std::string& sCapabilityClassId, bool& bRegistered);
	/** Gets the class id of the given type.
	 * @param oType The type to be tested.
	 * @return The class id or an empty string if the type is not registered.
	 */
	static std::string getCapabilityTypeClassId(const std::type_info& oType);
private:
	// Deferred unique instance creation (Scott Meyers singleton)
	static Private::NamedTypes<bool>& getNamedTypes()
	{
		static Private::NamedTypes<bool> s_oNamedTypes;
		return s_oNamedTypes;
	}
private:
	static int32_t getNewCapabilityId();
private:
	const Capability::Class m_oClass;
private:
	int32_t m_nCapabilityId;
	static std::atomic<int32_t> s_nNewIdCounter;
private:
	Capability() = delete;
};

/** DeviceManager capabilities should derive from this class.
 */
class DeviceManagerCapability : public Capability
{
public:
	/** Returns null. */
	shared_ptr<Device> getDevice() const override { return shared_ptr<Device>(); }
	/** Returns the device manager owning this capability, if any. */
	virtual shared_ptr<DeviceManager> getDeviceManager() const = 0;

protected:
	/** @see Capability::Capability(const Class&) */
	DeviceManagerCapability(const Class& oClass);
private:
	DeviceManagerCapability() = delete;
};

} // namespace stmi

namespace std {
	template <>
	struct hash<stmi::Capability::Class>
	{
		std::size_t operator()(const stmi::Capability::Class& oClass) const
		{
			if (!oClass) {
				return 0;
			}
			return oClass.getTypeInfo().hash_code();
		}
	};
} // namespace std

#endif /* STMI_CAPABILITY_H */

