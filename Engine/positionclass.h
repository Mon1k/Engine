#ifndef _POSITIONCLASS_H_
#define _POSITIONCLASS_H_

#include <math.h>
#include <d3dx10math.h>

class PositionClass
{
public:
	PositionClass();
	PositionClass(const PositionClass&);
	~PositionClass();

	void SetFrameTime(float);
	
	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void GetPosition(float&, float&, float&);
	D3DXVECTOR3 getPosition() {
		return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
	};
	void GetRotation(float&, float&, float&);
	D3DXVECTOR3 getRotation() {
		return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
	};

	void MoveForward(bool);
	void MoveBackward(bool);
	void MoveLeft(bool);
	void MoveRight(bool);
	void MoveUpward(bool);
	void MoveDownward(bool);
	void TurnLeft(bool);
	void TurnRight(bool);
	void LookUpward(bool);
	void LookDownward(bool);

	float getSensivity() {
		return m_sensivity;
	}
	void setSensivity(float sensivity) {
		m_sensivity = sensivity;
	}
private:
	float m_frameTime;

	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	float m_forwardSpeed, m_backwardSpeed;
	float m_strafeLeftSpeed, m_strafeRightSpeed;
	float m_upwardSpeed, m_downwardSpeed;
	float m_leftTurnSpeed, m_rightTurnSpeed;
	float m_lookUpSpeed, m_lookDownSpeed;

	float m_sensivity;
};

#endif