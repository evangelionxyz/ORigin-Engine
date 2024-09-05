// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef UUID_H
#define UUID_H

#include "Types.h"

#include <functional>
#include <string>

namespace origin
{
	class UUID
	{
	public:
		UUID();
		UUID(u64 uuid);
		UUID(const UUID &uuid) = default;
		operator u64() const { return m_UUID; }
	private:
		u64 m_UUID;
	};
}

template<>
struct std::hash<origin::UUID>
{
	std::size_t operator() (const origin::UUID& uuid) const noexcept
	{
		return hash<u64>()(static_cast<u64>(uuid));
	}
};

#endif