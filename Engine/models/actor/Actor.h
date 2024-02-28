#pragma once

#include <vector>
#include <map>
#include <string>

#include "../Model.h"
#include "../../tool/String.h"

class Actor: public Model
{
public:
	// wight per vertex
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

	// frame per anim node with matrix transformation
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

	// anim node
	struct AnimationNode
	{
		std::string name;
		std::vector<KeyFrame> frames;

		D3DXMATRIX transform;

		AnimationNode() {
			D3DXMatrixIdentity(&transform);
		}
	};

	// animation
	struct Animation
	{
		float totalTime;
		float currentTime;
		float maxTime;
		float tick;

		std::string name;
		std::vector<AnimationNode> nodes;
	};

	// node
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

	// bone
	struct BoneInfo
	{
		std::string name = "none";
		D3DXMATRIX OffsetMatrix;
		NodeInfo* node;

		D3DXMATRIX transformation;

		BoneInfo() {
			D3DXMatrixIdentity(&OffsetMatrix);
		}
	};

	// mesh
	struct MeshInfo
	{
		std::string name;
		int baseVertex;
		int count;

		NodeInfo* node;
		std::vector<BoneInfo> bones;
	};

	enum AnimationType {
		AnimationWalk = 0,
		AnimationRun = 1,
		AnimationIdle = 2
	};
	const std::string AnimationTypeWalk = "walk";
	const std::string AnimationTypeRun = "run";
	const std::string AnimationTypeIdle = "idle";

public:
	Actor();

	void addAnimation(Actor::Animation animation)
	{
		m_animations.push_back(animation);
	}

	void fillAnimationMap(Actor::Animation animation)
	{
		std::vector<int> values;
		values.push_back(m_animations.size() - 1);

		if (String::search(animation.name, AnimationTypeWalk) != -1) {
			if (m_animationsMap.count(AnimationTypeWalk) > 0) {
				m_animationsMap.at(Actor::AnimationTypeWalk).push_back(m_animations.size() - 1);
			}
			else {
				m_animationsMap.insert(std::pair<std::string, std::vector<int>>(Actor::AnimationTypeWalk, values
				));
			}
		} else if (String::search(animation.name, AnimationTypeRun) != -1) {
			if (m_animationsMap.count(AnimationTypeRun) > 0) {
				m_animationsMap.at(Actor::AnimationTypeRun).push_back(m_animations.size() - 1);
			}
			else {
				m_animationsMap.insert(std::pair<std::string, std::vector<int>>(Actor::AnimationTypeRun, values
				));
			}
		} else if (String::search(animation.name, AnimationTypeIdle) != -1) {
			if (m_animationsMap.count(AnimationTypeIdle) > 0) {
				m_animationsMap.at(Actor::AnimationTypeIdle).push_back(m_animations.size() - 1);
			}
			else {
				m_animationsMap.insert(std::pair<std::string, std::vector<int>>(Actor::AnimationTypeIdle, values
			));
			}
		}
	}

	void setWeights(std::vector<Actor::Weight> weights)
	{
		m_weights = weights;
	}

	void addNode(Actor::NodeInfo* node)
	{
		m_NodeInfo.push_back(node);
	}

	std::vector<Actor::NodeInfo*> getNodes()
	{
		return m_NodeInfo;
	}

	void addMesh(Actor::MeshInfo* mesh)
	{
		m_Mesh.push_back(mesh);
	}

	Actor::MeshInfo* getMesh(int index)
	{
		return m_Mesh[index];
	}

	virtual void frame(CameraClass*, float);

	int getCurrentAnimation()
	{
		return m_currentAnimation;
	}

	void setCurrentAnimation(int animation)
	{
		if (animation >= 0 && animation < m_animations.size()) {
			m_currentAnimation = animation;
		}
		else {
			m_currentAnimation = 0;
		}
	}

protected:
	float m_counter;
	float m_counterTotal;

	int m_currentAnimation;
	int m_totalAnimation;

	std::vector<Animation> m_animations;
	std::map<std::string, std::vector<int>> m_animationsMap;


	std::vector<Weight> m_weights;
	std::vector<NodeInfo*> m_NodeInfo;
	std::vector<MeshInfo*> m_Mesh;
};