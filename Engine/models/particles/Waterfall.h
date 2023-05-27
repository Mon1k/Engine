#ifndef _WATERFALL_H_
#define _WATERFALL_H_

#include "AbstractParticle.h"

class Waterfall : public AbstractParticle
{
public:
	Waterfall();
	Waterfall(const Waterfall&);
	~Waterfall();

	bool Initialize(D3DClass*, std::string);

protected:
	bool InitializeParticleSystem();
	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();
};

#endif