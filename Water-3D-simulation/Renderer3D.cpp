#include "pch.h"

#include "Renderer3D.h"
#include "RenderToTexture.h"

#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Models\Basic Shapes\Cubes.h"
#include "Water.h"


using namespace vxe;
using namespace std;
using namespace concurrency;

using namespace DirectX;
using namespace Windows::Foundation;

void Renderer3D::CreateDeviceDependentResources() {

	DebugPrint(string("\t Renderer3D::CreateDeviceDependentResources() ... \n"));

	auto device = m_deviceResources->GetD3DDevice();

	vector<task<void>> tasks;

	_vertexshader = make_shared<VertexShader<VertexPositionNormalTexture>>();
	tasks.push_back(_vertexshader->CreateAsync(device, L"VertexShader.cso"));

	_pixelshader = make_shared<PixelShader>();
	tasks.push_back(_pixelshader->CreateAsync(device, L"PixelShader.cso"));

	_reflectionvertexshader = make_shared<VertexShader<VertexPositionNormalTexture>>();
	tasks.push_back(_reflectionvertexshader->CreateAsync(device, L"ReflectionVertexShader.cso"));

	_reflectionpixelshader = make_shared<PixelShader>();
	tasks.push_back(_reflectionpixelshader->CreateAsync(device, L"ReflectionPixelShader.cso"));

	_watermodel = make_shared<Water<VertexPositionNormalTexture, unsigned short>>();
	tasks.push_back(_watermodel->CreateAsync(device));

	_cubemodel = make_shared<Cube<VertexPositionNormalTexture, unsigned short>>();
	tasks.push_back(_cubemodel->CreateAsync(device));

	_rightcubemodel = make_shared<Cube<VertexPositionNormalTexture, unsigned short>>();
	tasks.push_back(_rightcubemodel->CreateAsync(device));

	_texture = make_shared<Texture2D>(device, DXGI_FORMAT_R8G8B8A8_UNORM);
	_reflectiontexture = make_shared<Texture2D>(device, DXGI_FORMAT_R8G8B8A8_UNORM);

	_watertexture = make_shared<Texture2D>(device, DXGI_FORMAT_R8G8B8A8_UNORM);
	tasks.push_back(_watertexture->CreateDDSAsync(m_deviceResources->GetD3DDeviceContext(), L"Assets\\blue01.dds"));

	_cubetexture = make_shared<Texture2D>(device, DXGI_FORMAT_R8G8B8A8_UNORM);
	tasks.push_back(_cubetexture->CreateDDSAsync(m_deviceResources->GetD3DDeviceContext(), L"Assets\\tiles.dds"));

	_rightcubetexture = make_shared<Texture2D>(device, DXGI_FORMAT_R8G8B8A8_UNORM);
	tasks.push_back(_rightcubetexture->CreateDDSAsync(m_deviceResources->GetD3DDeviceContext(), L"Assets\\cardboard.dds"));
	_leftcubetexture = make_shared<Texture2D>(device, DXGI_FORMAT_R8G8B8A8_UNORM);
	tasks.push_back(_leftcubetexture->CreateDDSAsync(m_deviceResources->GetD3DDeviceContext(), L"Assets\\green.dds"));
	
	when_all(tasks.begin(), tasks.end()).then([this]() {
		m_loadingComplete = true;
		DebugPrint(string("\t -- A lambda: Loading is complete! \n"));

	});

	_world = make_shared<WorldTransforms>(device);

	_cubeworld = make_shared<WorldTransforms>(device);
	_cubeworld->Translate(0.0f, -4.0f, 0.0f);

	_rightcubeworld = make_shared<WorldTransforms>(device);
	_rightcubeworld->ScaleRotateXAndTranslate(0.5f, 0.5f, 0.5f, -60.0f, 4.0f, 1.0f, -1.0f);

	_refcubeworld = make_shared<WorldTransforms>(device);
	_refcubeworld->ScaleRotateXAndTranslate(0.5f, 0.5f, 0.5f, 0.0f, 4.5f, -4.0f, -1.0f);

	_leftcubeworld = make_shared<WorldTransforms>(device);
	_leftcubeworld->ScaleRotateXAndTranslate(1.0f, 1.5f, 0.5f, -30.0f, -5.0f, 2.0f, -1.0f);

	_leftrefcubeworld = make_shared<WorldTransforms>(device);
	_leftrefcubeworld->ScaleRotateXAndTranslate(1.0f, 1.5f, 0.5f, 30.0f, -5.0f, -5.0f, 0.0f);

	//_refcubeworld->RotateX(45.0f);
	
}

void Renderer3D::CreateWindowSizeDependentResources() {

	DebugPrint(string("\t Renderer3D::CreateWindowSizeDependentResources() ...\n"));

	auto device = m_deviceResources->GetD3DDevice();
	auto context = m_deviceResources->GetD3DDeviceContext();

	_view = make_shared<ViewTransform>(device, Handedness::RightHanded);

	static const XMVECTORF32 eye = { 0.0f, 2.0f, 7.0f, 0.0f }; //bilo je y = 7.0f, z = -10.0f, x = 0.0f
	static const XMVECTORF32 at = { 0.0f, 0.0f, 1.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	_view->SetView(eye, at, up);
	_view->Update(context);

	Size outputSize = m_deviceResources->GetOutputSize();
	DebugPrint(string("\t\t Width:") + to_string(outputSize.Width) + string("\n"));
	DebugPrint(string("\t\t Height:") + to_string(outputSize.Height) + string("\n"));

	float r = outputSize.Width / outputSize.Height;
	float fov = 70.0f * XM_PI / 180.0f;
	float n = 0.01f;
	float f = 1000.0f;

	if (r < 1.0f) { fov *= 2.0f; }

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();
	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	_projection = make_shared<ProjectionTransform>(device, Handedness::RightHanded);
	_projection->SetProjection(orientationMatrix, fov, r, n, f);

	_projection->Update(context);

}

void Renderer3D::Blending() {

	auto device = m_deviceResources->GetD3DDevice();
	auto context = m_deviceResources->GetD3DDeviceContext();
	CD3D11_BLEND_DESC1 BlendState;
	ZeroMemory(&BlendState, sizeof(CD3D11_BLEND_DESC1));

	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	const D3D11_BLEND_DESC1* blendState = &BlendState;
	device->CreateBlendState1(blendState, &g_pBlendState);

	context->OMSetBlendState(g_pBlendState, 0, 0xffffffff);
}

void Renderer3D::Render() {

	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete) { return; }

	auto context = m_deviceResources->GetD3DDeviceContext();

	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	_vertexshader->Bind(context);
	_pixelshader->Bind(context);

	_texture = _watertexture;
	SetTexturing();
	Draw(_watermodel, _world);
	_texture = _cubetexture;
	SetTexturing();
	Draw(_cubemodel, _world);
	_texture = _rightcubetexture;
	SetTexturing();
	Draw(_rightcubemodel, _rightcubeworld);
	_texture = _leftcubetexture;
	SetTexturing();
	Draw(_rightcubemodel, _leftcubeworld);

	_reflectiontexture = _cubetexture;
	SetReflectionTexture();	

	ReflectionRender();

	SetCamera();

	m_deviceResources->SetRasterizerState();

}

void Renderer3D::ReflectionRender()
{
	if (!m_loadingComplete) { return; }

	auto context = m_deviceResources->GetD3DDeviceContext();

	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };

	_reflectionvertexshader->Bind(context);
	_reflectionpixelshader->Bind(context);
	
	Blending();
	
	_texture = _cubetexture;
	SetTexturing();
	Draw(_cubemodel, _cubeworld);

	_texture = _rightcubetexture;
	SetTexturing();
	Draw(_rightcubemodel, _refcubeworld);

	_texture = _leftcubetexture;
	SetTexturing();
	Draw(_rightcubemodel, _leftrefcubeworld);
}

void Renderer3D::SetTexturing() {

	DebugPrint(string("\t Renderer3D::SetTexturing() ...\n"));

	auto context = m_deviceResources->GetD3DDeviceContext();

	_texture->CreateSamplerState();
	_texture->BindSamplerState(context);

	_texture->CreateShaderResourceView();
	_texture->BindShaderResourceView(context);

}

void Renderer3D::SetReflectionTexture() {

	auto context = m_deviceResources->GetD3DDeviceContext();

	DebugPrint(string("\t Renderer3D::SetReflectionTexture() ...\n"));

	_reflectiontexture->CreateShaderResourceView();
	_reflectiontexture->BindShaderResourceView(context, ProgrammableStage::PixelShaderStage, 1);

}

void Renderer3D::ReleaseDeviceDependentResources() {

	DebugPrint(string("\t Renderer3D::ReleaseDeviceDependentResources() ... \n"));

	m_loadingComplete = false;

	_vertexshader->Reset();
	_pixelshader->Reset();

	_reflectionvertexshader->Reset();
	_reflectionpixelshader->Reset();

	_watermodel->Reset();
	_cubemodel->Reset();
	_rightcubemodel->Reset();

	_world->Reset();
	_cubeworld->Reset();
	_rightcubeworld->Reset();
	_leftcubeworld->Reset();
	_refcubeworld->Reset();
	_view->Reset();
	_projection->Reset();
	_reflectionview->Reset();
}
