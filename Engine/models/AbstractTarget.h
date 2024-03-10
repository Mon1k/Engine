#pragma once

#include "AbstractModel.h"

#include <iostream>
#include <fstream>
#include <vector>

class AbstractTarget
{
public:
	void clearTargets()
	{
		m_modelsTarget.clear();
	}
	
	void addTarget(AbstractModel* target)
	{
		m_modelsTarget.push_back(target);
	}

	void addTargets(std::vector <AbstractModel*> targets) {
		for (int i = 0; i < targets.size(); i++) {
			m_modelsTarget.push_back(targets[i]);
		}
	}

protected:
	std::vector<AbstractModel*> m_modelsTarget;
};