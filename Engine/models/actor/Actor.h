#pragma once

#include <vector>
#include <string>

#include "../Model.h"

class Actor: public Model
{
public:
	struct Joint
	{
		std::string name;
		D3DXVECTOR3 position;
		D3DXVECTOR4 scaling;
	};

	struct Weight
	{
		int joint;
		float bias;
		D3DXVECTOR3 position;
	};

	struct Animation
	{
		int countAnimations;
		int countJoints;

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

protected:
	std::vector<Actor::Animation> m_animations;
	std::vector<Actor::Weight> m_weights;
};