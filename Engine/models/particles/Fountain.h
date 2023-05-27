#ifndef _FOUNTAIN_H_
#define _FOUNTAIN_H_

#include "AbstractParticle.h"
#include "../../tool/random.h"

class Fountain : public AbstractParticle
{
public:
	Fountain();
	Fountain(const Fountain&);
	~Fountain();

	bool Initialize(D3DClass*, std::string);

protected:
	bool InitializeParticleSystem();
	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();
};

#endif