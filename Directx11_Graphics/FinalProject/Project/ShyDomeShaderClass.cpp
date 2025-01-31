﻿#include "ShyDomeShaderClass.h"
#include <fstream>

SkyDomeShaderClass::SkyDomeShaderClass()
{
}

SkyDomeShaderClass::SkyDomeShaderClass(const SkyDomeShaderClass &)
{
}

SkyDomeShaderClass::~SkyDomeShaderClass()
{
}

bool SkyDomeShaderClass::Initialize(ID3D11Device * device, HWND hwnd)
{
	return InitializeShader(device, hwnd, L"../Project/Skydome.vs", L"../Project/Skydome.ps");
}

void SkyDomeShaderClass::Shutdown()
{
	ShutdownShader();
}

bool SkyDomeShaderClass::Render(ID3D11DeviceContext *deviceContext, int indexCount
	, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix
	, D3DXVECTOR4 apexColor, D3DXVECTOR4 centerColor)
{
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, apexColor, centerColor))
	{
		return false;
	}

	// 준비한 버퍼를 셰이더로 렌더링 합니다.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool SkyDomeShaderClass::InitializeShader(ID3D11Device *device, HWND hwnd, const WCHAR * vsFilename, const WCHAR *psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// 버텍스 쉐이더 코드를 컴파일한다.
	ID3D10Blob* vertexShaderBuffer = nullptr;


	if (FAILED(D3DX11CompileFromFile(vsFilename, NULL, NULL, "SkyDomeVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage , NULL)))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 쉐이더 코드를 컴파일한다.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DX11CompileFromFile(psFilename, NULL, NULL, "SkyDomePixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,NULL, 0, &pixelShaderBuffer, &errorMessage,NULL)))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 버퍼로부터 정점 셰이더를 생성한다.
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))
	{
		return false;
	}

	// 버퍼에서 픽셀 쉐이더를 생성합니다.
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))
	{
		return false;
	}

	// 정점 입력 레이아웃 구조체를 설정합니다.
	// 이 설정은 ModelClass와 셰이더의 VertexType 구조와 일치해야합니다.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	// 레이아웃의 요소 수를 가져옵니다.
	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 만듭니다.
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	// 더 이상 사용되지 않는 정점 셰이더 퍼버와 픽셀 셰이더 버퍼를 해제합니다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 버텍스 쉐이더에있는 동적 행렬 상수 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 이 클래스 내에서 정점 셰이더 상수 버퍼에 액세스 할 수 있도록 상수 버퍼 포인터를 만듭니다.
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))
	{
		return false;
	}

	// 픽셀 쉐이더에있는 그라데이션 동적 상수 버퍼의 설명을 설정합니다.
	// D3D11_BIND_CONSTANT_BUFFER를 사용하면 ByteWidth가 항상 16의 배수 여야하며 그렇지 않으면 CreateBuffer가 실패합니다.
	D3D11_BUFFER_DESC gradientBufferDesc;
	gradientBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	gradientBufferDesc.ByteWidth = sizeof(GradientBufferType);
	gradientBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gradientBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gradientBufferDesc.MiscFlags = 0;
	gradientBufferDesc.StructureByteStride = 0;

	// 이 클래스 내에서 정점 셰이더 상수 버퍼에 액세스 할 수 있도록 상수 버퍼 포인터를 만듭니다.
	if (FAILED(device->CreateBuffer(&gradientBufferDesc, NULL, &m_gradientBuffer)))
	{
		return false;
	}

	return true;
}

void SkyDomeShaderClass::ShutdownShader()
{
	// 그라데이션 동적 상수 버퍼를 해제합니다.
	if (m_gradientBuffer)
	{
		m_gradientBuffer->Release();
		m_gradientBuffer = 0;
	}

	// 행렬 상수 버퍼를 해제합니다.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// 레이아웃을 해제합니다.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 쉐이더를 해제합니다.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 버텍스 쉐이더를 해제합니다.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void SkyDomeShaderClass::OutputShaderErrorMessage(ID3D10Blob *errorMessage, HWND hwnd, const WCHAR * shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;
	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());
	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();
	// Open a file to write the error message to.
	fout.open("shader-error.txt");
	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}
	// Close the file.
	fout.close();
	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;
	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename,
		MB_OK);
	return;
}

bool SkyDomeShaderClass::SetShaderParameters(ID3D11DeviceContext *deviceContext, D3DXMATRIX worldMatrix
	, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXVECTOR4 apexColor, D3DXVECTOR4 centerColor)
{
	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼의 잠금을 풉니다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 조명 상수 버퍼를 잠글 수 있도록 기록한다.
	if (FAILED(deviceContext->Map(m_gradientBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	GradientBufferType* dataPtr2 = (GradientBufferType*)mappedResource.pData;

	// 조명 변수를 상수 버퍼에 복사합니다.
	dataPtr2->apexColor = apexColor;
	dataPtr2->centerColor = centerColor;

	// 상수 버퍼의 잠금을 해제합니다.
	deviceContext->Unmap(m_gradientBuffer, 0);

	// 픽셀 쉐이더에서 광원 상수 버퍼의 위치를 ​​설정합니다.
	bufferNumber = 0;

	// 마지막으로 픽셀 쉐이더에 텍스처 정보 상수 버퍼를 업데이트 된 값으로 설정합니다.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_gradientBuffer);

	return true;
}

void SkyDomeShaderClass::RenderShader(ID3D11DeviceContext *deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정합니다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 삼각형을 그립니다.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
