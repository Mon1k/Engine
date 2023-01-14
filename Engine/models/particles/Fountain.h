#ifndef _FOUNTAIN_H_
#define _FOUNTAIN_H_

#include <d3d11.h>
#include <d3dx10math.h>

#include "../../modelclass.h"
#include "../../textures/particleshaderclass.h"

class Fountain : public ModelClass
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
	Fountain();
	Fountain(const Fountain&);
	~Fountain();

	bool Initialize(D3DClass*, std::wstring);
	void Shutdown();
	bool Frame(float);
	virtual void RenderBuffers(ID3D11DeviceContext*);
	virtual void Render(CameraClass*);

	void setDeviation(D3DXVECTOR3 deviation) {
		m_particleDeviationX = deviation.x;
		m_particleDeviationY = deviation.y;
		m_particleDeviationZ = deviation.z;
	}
	void setColor(D3DXVECTOR3 color) {
		m_color = color;
	};
	void setMaxParticles(int);

	void setParticlesPerSecond(float timeout) {
		m_particlesPerSecond = timeout;
	};

protected:
	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	bool InitializeBuffers();
	void CalcMinMax();

	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();

	bool UpdateBuffers();

private:
	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;
	D3DXVECTOR3 m_color;

	int m_currentParticleCount;
	float m_accumulatedTime;

	ParticleType* m_particleList;
	VertexType* m_vertices;
};

#endif