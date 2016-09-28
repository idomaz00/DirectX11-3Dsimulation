#pragma once

#include "pch.h"
#include "RenderToTexture.h"
//#include "d3dclass.h"

#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Third Party\DirectX Tool Kit\VertexTypes.h"
#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Models\Mesh Base.h"

#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Pipeline Stages\Shaders\Vertex Shader.h"
#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Pipeline Stages\Shaders\Pixel Shader.h"

#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Scene\Transforms\World Transforms.h"
#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Scene\Transforms\View Transform.h"
#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Scene\Transforms\Projection Transform.h"
#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Scene\Transforms\Position.h"

#include "C:\Users\Izabela\Documents\Projekti\3DSimulacije\Engine\Pipeline Resources\Textures\Texture2D.h"

namespace vxe {
	class Renderer3D : public RendererBase3D {
	public:
		Renderer3D() {
			DebugPrint(std::string("Renderer3D Constructor called..."));
			nullSrv = nullptr;
			nullUav = nullptr;
		}

		virtual void CreateDeviceDependentResources() override;
		virtual void CreateWindowSizeDependentResources() override;
		virtual void ReleaseDeviceDependentResources()override;
		virtual void Render()override;

		void SetTexturing();
		void SetReflectionTexture();
		void ReflectionRender();
		void Blending();		

		std::shared_ptr<VertexShader<DirectX::VertexPositionNormalTexture>> _vertexshader;
		std::shared_ptr<PixelShader> _pixelshader;

		std::shared_ptr<VertexShader<DirectX::VertexPositionNormalTexture>> _reflectionvertexshader;
		std::shared_ptr<PixelShader> _reflectionpixelshader;

		std::shared_ptr<MeshBase<DirectX::VertexPositionNormalTexture, unsigned short >> _watermodel;
		std::shared_ptr<MeshBase<DirectX::VertexPositionNormalTexture, unsigned short >> _cubemodel;
		std::shared_ptr<MeshBase<DirectX::VertexPositionNormalTexture, unsigned short >> _rightcubemodel;

		std::shared_ptr<RenderTextureClass> renderToTexture;

		ID3D11ShaderResourceView* nullSrv;
		ID3D11UnorderedAccessView* nullUav;	
		ID3D11BlendState1* g_pBlendState = NULL;

		std::shared_ptr<WorldTransforms> _world;
		std::shared_ptr<WorldTransforms> _cubeworld;
		std::shared_ptr<WorldTransforms> _rightcubeworld;
		std::shared_ptr<WorldTransforms> _refcubeworld;
		std::shared_ptr<WorldTransforms> _leftcubeworld;
		std::shared_ptr<WorldTransforms> _leftrefcubeworld;

		std::shared_ptr<ViewTransform> _view;
		std::shared_ptr<ProjectionTransform> _projection;
		std::shared_ptr<ViewTransform> _reflectionview;

		std::shared_ptr<Texture2D> _texture;
		std::shared_ptr<Texture2D> _reflectiontexture;
		std::shared_ptr<Texture2D> _watertexture;
		std::shared_ptr<Texture2D> _cubetexture;
		std::shared_ptr<Texture2D> _rightcubetexture;
		std::shared_ptr<Texture2D> _leftcubetexture;

	private:

		void SetCamera()
		{
			auto context = m_deviceResources->GetD3DDeviceContext();

			_view->GetConstantBuffer()->Bind(context,
				ProgrammableStage::VertexShaderStage, 1);
			_projection->GetConstantBuffer()->Bind(context,
				ProgrammableStage::VertexShaderStage, 2);
		}

		template <typename T, typename U>
		void Draw(std::shared_ptr<MeshBase<T, U>>& mesh,
			std::shared_ptr<WorldTransforms>& world,
			bool indexed = true)
		{
			auto context = m_deviceResources->GetD3DDeviceContext();

			world->Update(context);
			world->GetConstantBuffer()->Bind(context);

			mesh->BindVertexBuffer(context);
			mesh->BindIndexBuffer(context);

			if (indexed) mesh->DrawIndexed(context);
			else mesh->Draw(context);
		}
	};
}
