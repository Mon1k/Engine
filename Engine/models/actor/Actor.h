#pragma once

#include <vector>
#include <string>

#include "../Model.h"
#include "../loader/AbstractLoader.h"

class Actor: public Model
{
public:
	struct Weight
	{
		int joint;
		float bias;
		int index;
		D3DXVECTOR3 position;
		std::string name;

		int BoneIDs[4] = { 0 };
		float Weights[4] = { 0.0f };

		Weight()
		{
		}

		void AddBoneData(int BoneID, float Weight)
		{
			for (int i = 0; i < 4; i++) {
				if (Weights[i] == 0.0f) {
					BoneIDs[i] = BoneID;
					Weights[i] = Weight;
					return;
				}
			}
		}
	};

	struct KeyFrame
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 scaling;
		D3DXQUATERNION rotation;
		D3DXMATRIX transform;
		int numFrame;
		float time;

		KeyFrame() {
			time = 0;
		}
	};

	struct Joint
	{
		std::string name;
		int parentId;
		D3DXMATRIX inverse;
		std::vector<KeyFrame> animation;
		std::vector<KeyFrame> position;
		std::vector<KeyFrame> scaling;
		std::vector<KeyFrame> rotation;
	};

	struct Animation
	{
		float frameTime;
		float totalTime;
		float currentTime;
		float tick;
		std::string name;
		D3DXMATRIX globalInverseTransformation;

		std::vector<Joint> joints;
	};

	struct BoneInfo
	{
		D3DXMATRIX OffsetMatrix;
		D3DXMATRIX FinalTransformation;
		D3DXMATRIX transformation;
		D3DXMATRIX globalTansformation;
		std::string name;
		std::string parent;
		int boneId;
		
		BoneInfo() {
			boneId = -1;
			D3DXMatrixIdentity(&transformation);
			D3DXMatrixIdentity(&globalTansformation);
		}

		BoneInfo(D3DXMATRIX Offset)
		{
			OffsetMatrix = Offset;
			D3DXMatrixIdentity(&transformation);
			D3DXMatrixIdentity(&globalTansformation);
			D3DXMatrixIdentity(&FinalTransformation);
		}
	};

	

public:
	Actor();

	void addAnimation(Actor::Animation animation)
	{
		m_animations.push_back(animation);
	}

	void addWeights(std::vector<Actor::Weight> weights)
	{
		m_weights.clear();
		m_weights = weights;
	}

	virtual void frame(CameraClass*, float);
	D3DXMATRIX CalculateGlobalTransform(std::string boneName, D3DXMATRIX transform);

	std::vector<Actor::Animation> m_animations;
	std::vector<Actor::Weight> m_weights;
	std::vector<BoneInfo> m_BoneInfo;
protected:
	

	float m_counter;
	float m_counterTotal;
	int m_currentAnimation;
	int m_totalAnimation;

};