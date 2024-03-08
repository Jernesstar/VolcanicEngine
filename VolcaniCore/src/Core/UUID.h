#pragma once

#include "Core/Random.h"

namespace VolcaniCore {

class UUID {
public:
    UUID() : m_ID(Random::RandInt()) { }
    UUID(uint64_t uuid) : m_ID(uuid) { }

    operator uint64_t() const { return m_ID; }

private:
    uint64_t m_ID;
};

}

namespace std {

template<>
struct hash<VolcaniCore::UUID> {
    std::size_t operator()(const VolcaniCore::UUID& uuid) const
    {
        return (uint64_t)uuid;
    }
};

}