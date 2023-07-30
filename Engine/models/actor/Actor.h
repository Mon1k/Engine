#pragma once

#include <vector>
#include <string>

#include "../Model.h"

class Actor: public Model
{
public:
	struct Weight
	{
		int joint;
		float bias;
		D3DXVECTOR3 position;
	};

	struct KeyFrame
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 scaling;
		D3DXMATRIX transform;
		int numFrame;
	};

	struct Joint
	{
		std::string name;
		int parentId;
		D3DXMATRIX inverse;
		std::vector<KeyFrame> animation;
	};

	struct Animation
	{
		float frameTime;
		float totalTime;
		float currentTime;
		std::string name;

		std::vector<Joint> joints;
	};

public:
	Actor();

	void addAnimation(Actor::Animation animation)
	{
		m_animations.push_back(animation);
	}

	void addWeight(Actor::Weight weight)
	{
		m_weights.push_back(weight);
	}

	virtual void frame(CameraClass*, float);

protected:
	std::vector<Actor::Animation> m_animations;
	std::vector<Actor::Weight> m_weights;

	float m_counter;
	int m_currentAnimation;
};