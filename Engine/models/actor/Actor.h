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
		unsigned int BoneIDs[4] = { 0 };
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
		int numFrame;
		float time;

		KeyFrame() {
			time = 0;
			numFrame = 0;
			position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			scaling = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			rotation = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 0.0f);
		}
	};

	struct AnimationNode
	{
		std::string name;
		std::vector<KeyFrame> frames;

		D3DXMATRIX transform;

		AnimationNode() {
			D3DXMatrixIdentity(&transform);
		}
	};

	struct Animation
	{
		float frameTime;
		float totalTime;
		float currentTime;
		float maxTime;
		float tick;
		std::string name;
		std::vector<AnimationNode> nodes;
	};

	struct NodeInfo
	{
		D3DXMATRIX localTransformation;
		D3DXMATRIX globalTransformation;
		std::string name = "none";

		NodeInfo* parent;
		int depth = 0;

		NodeInfo() {
			parent = nullptr;
			D3DXMatrixIdentity(&localTransformation);
			D3DXMatrixIdentity(&globalTransformation);
		}

		void setParent(NodeInfo* parentLink) {
			parent = parentLink;
			if (parent) {
				globalTransformation = localTransformation * parent->globalTransformation;
			}
		}
	};

	struct BoneInfo
	{
		std::string name = "none";
		D3DXMATRIX OffsetMatrix;
		NodeInfo* node;

		D3DXMATRIX t;	

		BoneInfo() {
			D3DXMatrixIdentity(&OffsetMatrix);
		}
	};

	struct HierarchyMesh
	{
		std::string name;
		int baseVertex;

		NodeInfo* node;
		std::vector<BoneInfo> bones;
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

	std::vector<Animation> m_animations;
	std::vector<Weight> m_weights;
	std::vector<NodeInfo*> m_NodeInfo;
	std::vector<HierarchyMesh*> m_Mesh;

protected:
	float m_counter;
	float m_counterTotal;
	int m_currentAnimation;
	int m_totalAnimation;
};