#pragma once

#include <vector>
#include <algorithm>

#include "AbstractModel.h"

class CompositeModel: public AbstractModel
{
public:
	CompositeModel()
	{
		m_childs.clear();
	}

	void addChild(AbstractModel* child)
	{
		m_childs.push_back(child);
		this->CalcMinMax();
	}

	void removeChild(int id)
	{
		size_t size = m_childs.size();
		for (int i = 0; i < size; i++) {
			m_childs.erase(m_childs.begin() + i);
		}
	}

	void removeChilds()
	{
		m_childs.clear();
	}

	virtual void Render(CameraClass* camera)
	{
		size_t size = m_childs.size();
		for (int i = 0; i < size; i++) {
			m_childs[i]->Render(camera);
		}
	}

	virtual void Render()
	{
		size_t size = m_childs.size();
		for (int i = 0; i < size; i++) {
			m_childs[i]->Render();
		}
	}

	virtual void Shutdown()
	{
		size_t size = m_childs.size();
		for (int i = 0; i < size; i++) {
			m_childs[i]->Shutdown();
		}
	}

	virtual void SetPosition(D3DXVECTOR3 _position)
	{
		D3DXVECTOR3 delta;
		for (size_t i = 0; i < m_childs.size(); i++) {
			D3DXVECTOR3 newPosition = m_childs[i]->GetPosition();
			newPosition.x += _position.x - newPosition.x;
			newPosition.y += _position.y - newPosition.y;
			newPosition.z += _position.z - newPosition.z;

			m_childs[i]->SetPosition(newPosition);
		}

		position = _position;
		this->CalcMinMax();
	}

	virtual void SetScale(D3DXVECTOR3 _scale)
	{
		for (size_t i = 0; i < m_childs.size(); i++) {
			m_childs[i]->SetScale(_scale);
		}

		scale = _scale;
		this->CalcMinMax();
	}

	virtual void SetRotation(D3DXVECTOR3 _rotation)
	{
		for (size_t i = 0; i < m_childs.size(); i++) {
			m_childs[i]->SetRotation(_rotation);
		}

		m_rotation = _rotation;
	}

	virtual void setAlpha(bool alpha)
	{
		m_isAlpha = alpha;

		for (size_t i = 0; i < m_childs.size(); i++) {
			m_childs[i]->setAlpha(alpha);
		}
	}

	virtual void setShadow(bool shadow)
	{
		m_isShadow = shadow;

		for (size_t i = 0; i < m_childs.size(); i++) {
			m_childs[i]->setShadow(shadow);
		}
	}

	virtual void CalcMinMax()
	{
		D3DXVECTOR3 minP, maxP;

		m_Min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
		m_Max = D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN);

		for (size_t i = 0; i < m_childs.size(); i++) {
			minP = m_childs[i]->getMinPosition();
			maxP = m_childs[i]->getMaxPosition();

			m_Min.x = min(m_Min.x, minP.x);
			m_Min.y = min(m_Min.y, minP.y);
			m_Min.z = min(m_Min.z, minP.z);

			m_Max.x = max(m_Max.x, maxP.x);
			m_Max.y = max(m_Max.y, maxP.y);
			m_Max.z = max(m_Max.z, maxP.z);
		}
	}

	std::vector<AbstractModel*> getChilds()
	{
		return m_childs;
	}

	AbstractModel* getByIndex(int index)
	{
		return m_childs[index];
	}

	int GetIndexCount()
	{
		int count = 0;
		for (size_t i = 0; i < m_childs.size(); i++) {
			count += m_childs[i]->GetIndexCount();
		}

		return count;
	}
private:
	std::vector<AbstractModel*> m_childs;
};