#include "DsLoader.h"

bool DsLoader::load(char* filename, AbstractModel* model)
{
	ifstream fin;
	char input;
	int i, vertextCount;

	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail()) {
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> vertextCount;

	// Set the number of indices to be the same as the vertex count.
	model->setVertexCount(vertextCount);
	model->setIndexCount(vertextCount);

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':') {
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < vertextCount; i++) {
		fin >> model->m_model[i].x >> model->m_model[i].y >> model->m_model[i].z;
		fin >> model->m_model[i].tu >> model->m_model[i].tv;
		fin >> model->m_model[i].nx >> model->m_model[i].ny >> model->m_model[i].nz;
	}

	// Close the model file.
	fin.close();

    return true;
}