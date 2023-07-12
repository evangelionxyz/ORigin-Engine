// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <xhash>

namespace origin
{
	class UUID
	{
	public:
		// UUID id = 0
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID& uuid) = default;

		operator uint64_t() const { return m_UUID; }

	private:
		uint64_t m_UUID;
	};
}

namespace std
{
	template<>
	struct hash<origin::UUID>
	{
		std::size_t operator() (const origin::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}