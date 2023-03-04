#pragma once

#include <vector>
#include <algorithm>
using namespace std;

#include "AbstractModel.h"

class CompositeModel : public AbstractModel
{
public:
	CompositeModel() {
		m_childs.clear();
	}

	void addChild(AbstractModel* child) {
		m_childs.push_back(child);
	}

	virtual void Render(CameraClass* camera) {
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			m_childs[i]->Render(camera);
		}
	}

	virtual void Shutdown() {
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			m_childs[i]->Shutdown();
		}
	}
private:
	std::vector<AbstractModel*> m_childs;
};