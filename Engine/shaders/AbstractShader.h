#ifndef _ABSTRACTSHADER_H_
#define _ABSTRACTSHADER_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

class AbstractShader
{
public:
	virtual bool Initialize(ID3D11Device*) = 0;
	virtual void Shutdown()
	{
		// Release the pixel shader.
		if (m_pixelShader) {
			m_pixelShader->Release();
			m_pixelShader = 0;
		}

		// Release the vertex shader.
		if (m_vertexShader) {
			m_vertexShader->Release();
			m_vertexShader = 0;
		}
	};

	virtual bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3)
	{
		return true;
	};

	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename)
	{
		char* compileErrors;
		unsigned long bufferSize, i;
		ofstream fout;


		// Get a pointer to the error message text buffer.
		compileErrors = (char*)(errorMessage->GetBufferPointer());

		// Get the length of the message.
		bufferSize = errorMessage->GetBufferSize();

		// Open a file to write the error message to.
		fout.open("shader-error.log");

		// Write out the error message.
		for (i = 0; i < bufferSize; i++) {
			fout << compileErrors[i];
		}

		// Close the file.
		fout.close();

		// Release the error message.
		errorMessage->Release();
		errorMessage = 0;

		// Pop a message up on the screen to notify the user to check the text file for compile errors.
		MessageBox(NULL, L"Error compiling shader.  Check shader-error.log for message.", shaderFilename, MB_OK);
	}

protected:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
};

#endif