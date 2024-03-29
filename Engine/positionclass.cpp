#include "positionclass.h"

PositionClass::PositionClass()
{
	m_frameTime = 0.0f;
	
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_forwardSpeed = 0.0f;
	m_backwardSpeed = 0.0f;
	m_strafeLeftSpeed = 0.0f;
	m_strafeRightSpeed = 0.0f;
	m_upwardSpeed = 0.0f;
	m_downwardSpeed = 0.0f;
	m_leftTurnSpeed = 0.0f;
	m_rightTurnSpeed = 0.0f;
	m_lookUpSpeed = 0.0f;
	m_lookDownSpeed = 0.0f;

	m_sensivity = 1.0f;
	m_isFreeLook = true;
}


PositionClass::~PositionClass()
{
}

void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
}

void PositionClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}


void PositionClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

void PositionClass::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
}


void PositionClass::GetRotation(float& x, float& y, float& z)
{
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;
}

void PositionClass::MoveForward(bool keydown)
{
	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown) {
		m_forwardSpeed += m_frameTime * 0.001f;

		if (m_forwardSpeed > (m_frameTime * 0.03f)) {
			m_forwardSpeed = m_frameTime * 0.03f;
		}
	}
	else {
		m_forwardSpeed -= m_frameTime * 0.0007f;

		if (m_forwardSpeed < 0.0f) {
			m_forwardSpeed = 0.0f;
		}
	}

	if (m_forwardSpeed != 0.0f) {
		float yawRadian = m_rotationY * 0.0174532925f;

		if (m_isFreeLook) {
			float pitchRadian = m_rotationX * 0.0174532925f;

			m_positionX += sinf(yawRadian) * cosf(pitchRadian) * m_forwardSpeed * m_sensivity;
			m_positionY -= sinf(pitchRadian) * m_forwardSpeed * m_sensivity;
			m_positionZ += cosf(yawRadian) * cosf(pitchRadian) * m_forwardSpeed * m_sensivity;
		}
		else {
			m_positionX += sinf(yawRadian) * m_forwardSpeed * m_sensivity;
			m_positionZ += cosf(yawRadian) * m_forwardSpeed * m_sensivity;
		}
	}
}


void PositionClass::MoveLeft(bool keydown)
{
	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown) {
		m_strafeLeftSpeed += m_frameTime * 0.001f;

		if (m_strafeLeftSpeed > (m_frameTime * 0.03f)) {
			m_strafeLeftSpeed = m_frameTime * 0.03f;
		}
	}
	else {
		m_strafeLeftSpeed -= m_frameTime * 0.0007f;

		if (m_strafeLeftSpeed < 0.0f) {
			m_strafeLeftSpeed = 0.0f;
		}
	}

	if (m_strafeLeftSpeed != 0.0f) {
		// Convert degrees to radians.
		float radians = (m_rotationY - 90) * 0.0174532925f;

		// Update the position.
		m_positionX += sinf(radians) * m_strafeLeftSpeed * m_sensivity;
		m_positionZ += cosf(radians) * m_strafeLeftSpeed * m_sensivity;
	}
}

void PositionClass::MoveRight(bool keydown)
{
	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown) {
		m_strafeRightSpeed += m_frameTime * 0.001f;

		if (m_strafeRightSpeed > (m_frameTime * 0.03f)) {
			m_strafeRightSpeed = m_frameTime * 0.03f;
		}
	}
	else {
		m_strafeRightSpeed -= m_frameTime * 0.0007f;

		if (m_strafeRightSpeed < 0.0f) {
			m_strafeRightSpeed = 0.0f;
		}
	}

	if (m_strafeRightSpeed != 0.0f) {
		// Convert degrees to radians.
		float radians = (m_rotationY + 90) * 0.0174532925f;

		// Update the position.
		m_positionX += sinf(radians) * m_strafeRightSpeed * m_sensivity;
		m_positionZ += cosf(radians) * m_strafeRightSpeed * m_sensivity;
	}
}

void PositionClass::MoveBackward(bool keydown)
{
	// Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown) {
		m_backwardSpeed += m_frameTime * 0.001f;

		if (m_backwardSpeed > (m_frameTime * 0.03f)) {
			m_backwardSpeed = m_frameTime * 0.03f;
		}
	}
	else {
		m_backwardSpeed -= m_frameTime * 0.0007f;

		if (m_backwardSpeed < 0.0f) {
			m_backwardSpeed = 0.0f;
		}
	}

	if (m_backwardSpeed != 0.0f) {
		float yawRadian = m_rotationY * 0.0174532925f;

		if (m_isFreeLook) {
			float pitchRadian = m_rotationX * 0.0174532925f;
		
			m_positionX -= sinf(yawRadian) * cosf(pitchRadian) * m_backwardSpeed * m_sensivity;
			m_positionY += sinf(pitchRadian) * m_backwardSpeed * m_sensivity;
			m_positionZ -= cosf(yawRadian) * cosf(pitchRadian) * m_backwardSpeed * m_sensivity;
		}
		else {
			m_positionX -= sinf(yawRadian) * m_backwardSpeed * m_sensivity;
			m_positionZ -= cosf(yawRadian) * m_backwardSpeed * m_sensivity;
		}
	}
}

void PositionClass::MoveUpward(bool keydown)
{
	// Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown) {
		m_upwardSpeed += m_frameTime * 0.003f;

		if (m_upwardSpeed > (m_frameTime * 0.03f)) {
			m_upwardSpeed = m_frameTime * 0.03f;
		}
	}
	else {
		m_upwardSpeed -= m_frameTime * 0.002f;

		if (m_upwardSpeed < 0.0f) {
			m_upwardSpeed = 0.0f;
		}
	}

	// Update the height position.
	m_positionY += m_upwardSpeed * m_sensivity;
}

void PositionClass::MoveDownward(bool keydown)
{
	// Update the downward speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown) {
		m_downwardSpeed += m_frameTime * 0.003f;

		if (m_downwardSpeed > (m_frameTime * 0.03f)) {
			m_downwardSpeed = m_frameTime * 0.03f;
		}
	}
	else {
		m_downwardSpeed -= m_frameTime * 0.002f;

		if (m_downwardSpeed < 0.0f) {
			m_downwardSpeed = 0.0f;
		}
	}

	// Update the height position.
	m_positionY -= m_downwardSpeed * m_sensivity;
}


void PositionClass::TurnLeft(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns left.  If not slow down the turn speed.
	if (keydown) {
		m_leftTurnSpeed += m_frameTime * 0.01f * m_sensivity;

		if (m_leftTurnSpeed > (m_frameTime * 0.15f)) {
			m_leftTurnSpeed = m_frameTime * 0.15f;
		}
	} else {
		m_leftTurnSpeed -= m_frameTime * 0.03f;

		if (m_leftTurnSpeed < 0.0f) {
			m_leftTurnSpeed = 0.0f;
		}
	}

	// Update the rotation using the turning speed.
	m_rotationY -= m_leftTurnSpeed;
	if (m_rotationY < 0.0f) {
		m_rotationY += 360.0f;
	}
}


void PositionClass::TurnRight(bool keydown)
{
	// If the key is pressed increase the speed at which the camera turns right.  If not slow down the turn speed.
	if (keydown) {
		m_rightTurnSpeed += m_frameTime * 0.01f * m_sensivity;

		if (m_rightTurnSpeed > (m_frameTime * 0.15f)) {
			m_rightTurnSpeed = m_frameTime * 0.15f;
		}
	} else {
		m_rightTurnSpeed -= m_frameTime * 0.03f;

		if (m_rightTurnSpeed < 0.0f) {
			m_rightTurnSpeed = 0.0f;
		}
	}

	// Update the rotation using the turning speed.
	m_rotationY += m_rightTurnSpeed;
	if (m_rotationY > 360.0f) {
		m_rotationY -= 360.0f;
	}
}

void PositionClass::LookUpward(bool keydown)
{
	// Update the upward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown) {
		m_lookUpSpeed += m_frameTime * 0.01f * m_sensivity;

		if (m_lookUpSpeed > (m_frameTime * 0.15f)) {
			m_lookUpSpeed = m_frameTime * 0.15f;
		}
	}
	else {
		m_lookUpSpeed -= m_frameTime * 0.03f;

		if (m_lookUpSpeed < 0.0f) {
			m_lookUpSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationX -= m_lookUpSpeed;

	// Keep the rotation maximum 90 degrees.
	if (m_rotationX > 90.0f) {
		m_rotationX = 90.0f;
	}
}

void PositionClass::LookDownward(bool keydown)
{
	// Update the downward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if (keydown) {
		m_lookDownSpeed += m_frameTime * 0.01f * m_sensivity;

		if (m_lookDownSpeed > (m_frameTime * 0.15f)) {
			m_lookDownSpeed = m_frameTime * 0.15f;
		}
	}
	else {
		m_lookDownSpeed -= m_frameTime * 0.03f;

		if (m_lookDownSpeed < 0.0f) {
			m_lookDownSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationX += m_lookDownSpeed;

	// Keep the rotation maximum 90 degrees.
	if (m_rotationX < -90.0f) {
		m_rotationX = -90.0f;
	}
}