#include "Fountain.h"

Fountain::Fountain()
{
	m_particleList = 0;
	m_vertices = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


Fountain::Fountain(const Fountain& other)
{
}


Fountain::~Fountain()
{
}

bool Fountain::Initialize(D3DClass* d3dClass, std::wstring textureFilename)
{
	m_D3D = d3dClass;

	if (!LoadTextures(m_D3D->GetDevice(), textureFilename)) {
		return false;
	}

	// Initialize the particle system.
	if (!InitializeParticleSystem()) {
		return false;
	}

	// Create the buffers that will be used to render the particles with.
	if (!InitializeBuffers()) {
		return false;
	}

	return true;
}

bool Fountain::InitializeParticleSystem()
{
	// Set the random deviation of where the particles can be located when emitted.
	m_particleDeviationX = 0.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 2.0f;

	// Set the speed and speed variation of particles.
	m_particleVelocity = D3DXVECTOR3(0.2f, 2.0f, 0.2f);
	m_particleVelocityVariation = 0.2f;

	// Set the physical size of the particles.
	m_particleSize = 0.2f;

	// Set the number of particles to emit per second.
	m_particlesPerSecond = 250.0f;

	// Set the maximum number of particles allowed in the particle system.
	setMaxParticles(5000);

	// Initialize the current particle count to zero since none are emitted yet.
	m_currentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.0f;

	m_maxPosition = D3DXVECTOR3(0.0f, 10.0f, 0.0f);
	m_minPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_color = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	return true;
}

void Fountain::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, red, green, blue;
	D3DXVECTOR3 velocity, direction;
	int index, i, j;


	// Increment the frame time.
	m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond)) {
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1))) {
		m_currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX = position.x + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationX;
		positionY = position.y + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationY;
		positionZ = position.z + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationZ;

		direction = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		if (Random::randDouble(-1.0f, 1.0f) < 0.0f) {
			direction.x *= -1;
		}
		if (Random::randDouble(-1.0f, 1.0f) < 0.0f) {
			direction.z *= -1;
		}

		velocity.x = m_particleVelocity.x + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;
		velocity.y = m_particleVelocity.y + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;
		velocity.z = m_particleVelocity.z + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

		red = m_color.x ? m_color.x : (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = m_color.y ? m_color.y : (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = m_color.z ? m_color.z : (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while (!found) {
			if ((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ)) {
				found = true;
			} else {
				index++;
			}
		}

		// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
		i = m_currentParticleCount;
		j = i - 1;

		while (i != index) {
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].red = m_particleList[j].red;
			m_particleList[i].green = m_particleList[j].green;
			m_particleList[i].blue = m_particleList[j].blue;
			m_particleList[i].velocity = m_particleList[j].velocity;
			m_particleList[i].direction = m_particleList[j].direction;
			m_particleList[i].active = m_particleList[j].active;
			i--;
			j--;
		}

		// Now insert it into the particle array in the correct depth order.
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].red = red;
		m_particleList[index].green = green;
		m_particleList[index].blue = blue;
		m_particleList[index].velocity = velocity;
		m_particleList[index].direction = direction;
		m_particleList[index].active = true;
	}

	CalcMinMax();
}

void Fountain::UpdateParticles(float frameTime)
{
	int i;

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for (i = 0; i < m_currentParticleCount; i++) {
		if (m_particleList[i].positionY > m_maxPosition.y) {
			m_particleList[i].direction.y *= -1;
		}
		m_particleList[i].positionX += (m_particleList[i].direction.x * m_particleList[i].velocity.x) * frameTime * 0.001f;
		m_particleList[i].positionY += (m_particleList[i].direction.y * m_particleList[i].velocity.y) * frameTime * 0.001f;
		m_particleList[i].positionZ += (m_particleList[i].direction.z * m_particleList[i].velocity.z) * frameTime * 0.001f;
	}
}

void Fountain::KillParticles()
{
	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for (i = 0; i < m_maxParticles; i++) {
		if ((m_particleList[i].active == true) && (m_particleList[i].positionY < m_minPosition.y)) {
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for (j = i; j < m_maxParticles - 1; j++) {
				m_particleList[j].positionX = m_particleList[j + 1].positionX;
				m_particleList[j].positionY = m_particleList[j + 1].positionY;
				m_particleList[j].positionZ = m_particleList[j + 1].positionZ;
				m_particleList[j].red = m_particleList[j + 1].red;
				m_particleList[j].green = m_particleList[j + 1].green;
				m_particleList[j].blue = m_particleList[j + 1].blue;
				m_particleList[j].velocity = m_particleList[j + 1].velocity;
				m_particleList[j].direction = m_particleList[j + 1].direction;
				m_particleList[j].active = m_particleList[j + 1].active;
			}
		}
	}
}