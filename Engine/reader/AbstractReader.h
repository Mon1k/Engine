#pragma once

#include <string>

#include "AbstractEntity.h"

class AbstractReader
{
public:
	virtual AbstractEntity* read(std::string path) = 0;
};
