#include "SkydomeClass.h"

#include <fstream>
using namespace std;


SkyDomeClass::SkyDomeClass()
{
	time = 720;
}


SkyDomeClass::SkyDomeClass(const SkyDomeClass& other)
{
}


SkyDomeClass::~SkyDomeClass()
{
}


bool SkyDomeClass::Initialize(ID3D11Device* device)
{
	// 스카이 돔 모델 정보를 읽어옵니다.
	if (!LoadSkyDomeModel("../Project/data/skydome.txt"))
	{
		return false;
	}

	// 스카이 돔을 정점에 로드하고 렌더링을 위해 인덱스 버퍼를 로드합니다.
	if (!InitializeBuffers(device))
	{
		return false;
	}
	sunsetApexColor = D3DXVECTOR4(0.780f, 0.153f, 0.153f, 1.0f);
	sunsetCenterColor = D3DXVECTOR4(0.992f, 0.839f, 0.511f, 1.0f);
	nightApexColor = D3DXVECTOR4(0.192f, 0.203f, 0.352f, 1.0f);
	nightCenterColor = D3DXVECTOR4(0.283f, 0.363f, 0.394f, 0.7f);
	dayApexColor= D3DXVECTOR4(0.53f, 0.81f, 0.92f, 1.0f);
	dayCenterColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);


	dayToSunsetApex = (sunsetApexColor - dayApexColor) / 60;
	dayToSunsetCenter = (sunsetCenterColor - dayCenterColor) / 60;
	sunsetToDayApex = (dayApexColor - sunsetApexColor) / 60;
	sunsetToDayCenter = (dayCenterColor - sunsetCenterColor) / 60;

	nightToSunsetApex = (sunsetApexColor - nightApexColor)/60;
	nightToSunsetCenter = (sunsetCenterColor - nightCenterColor)/60;
	sunsetToNightApex = (nightApexColor - sunsetApexColor) / 60;
	sunsetToNightCenter = (nightCenterColor - sunsetCenterColor) / 60;

	time = 720;
	//==== 낮
	// 스카이 돔 꼭대기에 색상을 설정합니다.
	m_apexColor = dayApexColor;

	// 스카이 돔의 중심에 색상을 설정합니다.
	m_centerColor = dayCenterColor;

	return true;
}


void SkyDomeClass::Shutdown()
{
	// 스카이 돔 렌더링에 사용된 정점 및 인덱스 버퍼를 해제합니다.
	ReleaseBuffers();

	// 스카이 돔 모델을 해제합니다.
	ReleaseSkyDomeModel();
}


void SkyDomeClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 스카이 돔을 렌더링 합니다.
	RenderBuffers(deviceContext);
}

void SkyDomeClass::Frame()
{
	time++;

	if (time > 1440)
		time = 0;

	SkyChange();
}

void SkyDomeClass::SkyChange()
{
	if (time < 300) // 5시 전까지 밤
	{
		m_apexColor = nightApexColor;
		m_centerColor = nightCenterColor;
	}
	else if (time >= 300 && time < 360) // 5~6시까지 밤-> 노을로 바뀜
	{
		m_apexColor += nightToSunsetApex;
		m_centerColor += nightToSunsetCenter;
	}
	else if (time >= 360 && time < 420) //6시 ~7시까지 노을
	{
		m_apexColor = sunsetApexColor;
		m_centerColor = sunsetCenterColor;
	}
	else if (time >= 420 && time < 480) // 7시 ~ 8시까지 노을 -> 아침으로바뀜
	{
		m_apexColor += sunsetToDayApex;
		m_centerColor += sunsetToDayCenter;
	}
	else if (time >= 480 && time < 1020) // 8시 ~ 17시까지 밝은 하늘
	{
		m_apexColor = dayApexColor;
		m_centerColor = dayCenterColor;
	}
	else if (time >= 1020 && time < 1080) // 17시 ~ 18시까지 밝은 하늘 -> 노을
	{
		m_apexColor += dayToSunsetApex;
		m_centerColor += dayToSunsetCenter;
	}
	else if (time >= 1080 && time < 1140) // 18시 ~ 19시까지 노을 -> 밤
	{
		m_apexColor += sunsetToNightApex;
		m_centerColor += sunsetToNightCenter;
	}
	else if (time >= 1140)
	{
		m_apexColor = nightApexColor;
		m_centerColor = nightCenterColor;
	}
}

bool SkyDomeClass::IsSpecularLite()
{
	//if (time < 300) // 5시 전까지 밤
	//{

	//}
	//else if (time >= 300 && time < 360) // 5~6시까지 밤-> 노을로 바뀜
	//{

	//}
	//else if (time >= 360 && time < 420) //6시 ~7시까지 노을
	//{

	//}
	//else if (time >= 420 && time < 480) // 7시 ~ 8시까지 노을 -> 아침으로바뀜
	//{

	//}
	//else if (time >= 480 && time < 1020) // 8시 ~ 17시까지 밝은 하늘
	//{

	//}
	//else if (time >= 1020 && time < 1080) // 17시 ~ 18시까지 밝은 하늘 -> 노을
	//{

	//}
	//else if (time >= 1080 && time < 1140) // 18시 ~ 19시까지 노을 -> 밤
	//{

	//}
	//else if (time >= 1140)
	//{
	//}
	return false;
}

D3DXVECTOR4 SkyDomeClass::GetLightColor()
{
	//if (time < 300) // 5시 전까지 밤
	//{

	//}
	//else if (time >= 300 && time < 360) // 5~6시까지 밤-> 노을로 바뀜
	//{

	//}
	//else if (time >= 360 && time < 420) //6시 ~7시까지 노을
	//{

	//}
	//else if (time >= 420 && time < 480) // 7시 ~ 8시까지 노을 -> 아침으로바뀜
	//{

	//}
	//else if (time >= 480 && time < 1020) // 8시 ~ 17시까지 밝은 하늘
	//{

	//}
	//else if (time >= 1020 && time < 1080) // 17시 ~ 18시까지 밝은 하늘 -> 노을
	//{

	//}
	//else if (time >= 1080 && time < 1140) // 18시 ~ 19시까지 노을 -> 밤
	//{

	//}
	//else if (time >= 1140)
	//{
	//}
	return m_apexColor;
}


int SkyDomeClass::GetIndexCount()
{
	return m_indexCount;
}


D3DXVECTOR4 SkyDomeClass::GetApexColor()
{
	return m_apexColor;
}


D3DXVECTOR4 SkyDomeClass::GetCenterColor()
{
	return m_centerColor;
}

int SkyDomeClass::GetTime()
{
	return time;
}


bool SkyDomeClass::LoadSkyDomeModel(const char* filename)
{
	// 모델 파일을 엽니다.
	ifstream fin;
	fin.open(filename);

	// 파일을 열 수 없으면 종료합니다.
	if (fin.fail())
	{
		return false;
	}

	// 버텍스 카운트의 값까지 읽는다.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// 버텍스 카운트를 읽는다.
	fin >> m_vertexCount;

	// 인덱스의 수를 정점 수와 같게 설정합니다.
	m_indexCount = m_vertexCount;

	// 읽어 들인 정점 개수를 사용하여 모델을 만듭니다.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// 데이터의 시작 부분까지 읽는다.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// 버텍스 데이터를 읽습니다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// 모델 파일을 닫는다.
	fin.close();

	return true;
}


void SkyDomeClass::ReleaseSkyDomeModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
}


bool SkyDomeClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 정점 배열과 인덱스 배열을 데이터로 로드합니다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y-0.3f, m_model[i].z);
		indices[i] = i;
	}

	// 정점 버퍼의 구조체를 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 마침내 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 이제 버텍스와 인덱스 버퍼가 생성되고로드 된 배열을 해제하십시오.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void SkyDomeClass::ReleaseBuffers()
{
	// 인덱스 버퍼를 해제합니다.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 버텍스 버퍼를 해제한다.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void SkyDomeClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링되어야하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

