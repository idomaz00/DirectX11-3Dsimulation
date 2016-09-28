#pragma once

#include "pch.h"
#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Pipeline Resources\Textures\Texture2D.h"

namespace vxe
{
	class RenderTextureClass
	{
	public:
		RenderTextureClass();

		void Initialize(ID3D11Device2*, int, int);
		void Reset();

		void SetRenderTarget(ID3D11DeviceContext2*);
		void ClearRenderTarget(ID3D11DeviceContext2*, float, float, float, float);
		ID3D11ShaderResourceView* GetShaderResourceView();

	private:
		ID3D11Texture2D* m_renderTargetTexture;
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11ShaderResourceView* m_shaderResourceView;
		ID3D11Texture2D* m_depthStencilBuffer;
		ID3D11DepthStencilView* m_depthStencilView;
		D3D11_VIEWPORT m_viewport;

		std::shared_ptr<Texture2D> _p1Texture;

		CD3D11_TEXTURE2D_DESC createTexture2dDesc(int width, int height)
		{
			CD3D11_TEXTURE2D_DESC textureDesc;
			ZeroMemory(&textureDesc, sizeof(textureDesc));

			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;

			return textureDesc;
		}
		CD3D11_RENDER_TARGET_VIEW_DESC createRTVDesc(DXGI_FORMAT format)
		{
			CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

			renderTargetViewDesc.Format = format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;

			return renderTargetViewDesc;
		}
		CD3D11_SHADER_RESOURCE_VIEW_DESC createSRVDesv(DXGI_FORMAT format)
		{
			CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

			shaderResourceViewDesc.Format = format;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			return shaderResourceViewDesc;
		}
		CD3D11_TEXTURE2D_DESC createDepthBufferDesc(int width, int height)
		{
			CD3D11_TEXTURE2D_DESC depthBufferDesc;
			ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

			depthBufferDesc.Width = width;
			depthBufferDesc.Height = height;
			depthBufferDesc.MipLevels = 1;
			depthBufferDesc.ArraySize = 1;
			depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthBufferDesc.SampleDesc.Count = 1;
			depthBufferDesc.SampleDesc.Quality = 0;
			depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthBufferDesc.CPUAccessFlags = 0;
			depthBufferDesc.MiscFlags = 0;

			return depthBufferDesc;
		}
		CD3D11_DEPTH_STENCIL_VIEW_DESC createDSVDesc() {
			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
			return depthStencilViewDesc;
		}
	};
}