#pragma once

#include "Core/Defines.h"

namespace VolcaniCore {

class Serializer {
public:
	template<typename TData>
	Serializer& Write(const std::string& name, const TData& value);

	template<typename TData>
	Serializer& Read(const std::string& name, TData& value);

	virtual void Finalize(const std::string& path) = 0;
};

}