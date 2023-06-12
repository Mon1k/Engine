#pragma once

#include <string>

#include "AbstractEntity.h"

class AbstractWriter
{
public:
	virtual void save(std::string, AbstractEntity*) = 0;
};