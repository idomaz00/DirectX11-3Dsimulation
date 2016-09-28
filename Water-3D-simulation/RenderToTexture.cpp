#include "pch.h"
#include "RenderToTexture.h"
#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Pipeline Resources\Textures\Texture2D.h"

using namespace DirectX;
using namespace vxe;

RenderTextureClass::RenderTextureClass()
{
	m_renderTargetTexture = 0;
	m_renderTargetView = 0;
	m_shaderResourceView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilView = 0;
}

void RenderTextureClass::Initialize(ID3D11Device2* device, int textureWidth, int textureHeight)
{
	// RENDER TARGET TEXTURE
	auto textureDesc = createTexture2dDesc(textureWidth, textureHeight);
	device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);

	// RENDER TARGET VIEW
	auto renderTargetViewDesc = createRTVDesc(textureDesc.Format);
	device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);

	auto srvDesc = createSRVDesv(textureDesc.Format);
	device->CreateShaderResourceView(m_renderTargetTexture, &srvDesc, &m_shaderResourceView);

	// DEPTH BUFFER
	auto depthBufferDesc = createDepthBufferDesc(textureWidth, textureHeight);
	device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);

	// DEPTH STENCIL VIEW
	auto depthStencilViewDesc = createDSVDesc();
	device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);


	// VIEWPORT
	m_viewport.Width = (float)textureWidth;
	m_viewport.Height = (float)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
}


void RenderTextureClass::Reset()
{
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}

	return;
}


void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext2* deviceContext)
{
	ID3D11RenderTargetView *const targets[1] = { m_renderTargetView };
	deviceContext->OMSetRenderTargets(1, targets, m_depthStencilView);
	deviceContext->RSSetViewports(1, &m_viewport);
}


void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext2* deviceContext, float red, float green, float blue, float alpha)
{
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}
