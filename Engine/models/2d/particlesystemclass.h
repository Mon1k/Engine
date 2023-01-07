#ifndef _PARTICLESYSTEMCLASS_H_
#define _PARTICLESYSTEMCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>

#include "../../modelclass.h"
#include "../../textures/particleshaderclass.h"

class ParticleSystemClass : public ModelClass
{
private:	
	struct ParticleType
	{
		float positionX, positionY, positionZ;
		float red, green, blue;
		float velocity;
		bool active;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR4 color;
	};

public:
	ParticleSystemClass();
	ParticleSystemClass(const ParticleSystemClass&);
	~ParticleSystemClass();

	bool Initialize(D3DClass*, std::wstring);
	void Shutdown();
	bool Frame(float);
	virtual void RenderBuffers(ID3D11DeviceContext*);
	virtual void Render(CameraClass*);

private:
	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	bool InitializeBuffers();

	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();

	bool UpdateBuffers();

private:
	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;

	int m_currentParticleCount;
	float m_accumulatedTime;

	ParticleType* m_particleList;
	VertexType* m_vertices;
};

#endif