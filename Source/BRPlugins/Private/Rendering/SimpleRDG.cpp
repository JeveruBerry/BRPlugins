#include "Rendering/SimpleRDG.h"
#include "Engine/TextureRenderTarget2D.h"

#include "CommonRenderResources.h"
#include "Containers/DynamicRHIResourceArray.h"
#include "Engine/Engine.h"
#include "PipelineStateCache.h"

#include "GlobalShader.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "RHIStaticStates.h"
#include "ShaderParameterUtils.h"

namespace SimpleRGD {

	IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FBRUniformBufferParameters, "BRUniform");
	/**
	 * GlobalShader
	 */
	class FRDGComputerShader : public FGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(FRDGComputerShader);
		SHADER_USE_PARAMETER_STRUCT(FRDGComputerShader, FGlobalShader);

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		//	SHADER_PARAMETER_STRUCT_REF(FUniformBufferParameters, UniformBufferParameters)
			SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutTexture)
			//RENDER_TARGET_BINDING_SLOTS()
		END_SHADER_PARAMETER_STRUCT()

		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
		{
			return RHISupportsComputeShaders(Parameters.Platform);
		}
	};

	IMPLEMENT_GLOBAL_SHADER(FRDGComputerShader, "/BRPlugins/Private/SimpleComputeShader.usf", "MainCS", SF_Compute);

	// Shader for draw pass to render each symbol
	class FRDGGlobalShader : public FGlobalShader
	{
	public:
		SHADER_USE_PARAMETER_STRUCT(FRDGGlobalShader, FGlobalShader);

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
			SHADER_PARAMETER_STRUCT_REF(FBRUniformBufferParameters, UniformBufferParameters)
			SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, TextureUAV)
			RENDER_TARGET_BINDING_SLOTS()
		END_SHADER_PARAMETER_STRUCT()

		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
		{
			return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
		}
	};

	class FRDGVertexShader : public FRDGGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(FRDGVertexShader);

		FRDGVertexShader()
		{}

		FRDGVertexShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
			: FRDGGlobalShader(Initializer)
		{}
	};

	IMPLEMENT_GLOBAL_SHADER(FRDGVertexShader, "/BRPlugins/Private/SimplePixelShader.usf", "MainVS", SF_Vertex);

	class FRDGPixelShader : public FRDGGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(FRDGPixelShader);

		FRDGPixelShader()
		{}

		FRDGPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
			: FRDGGlobalShader(Initializer)
		{}
	};

	IMPLEMENT_GLOBAL_SHADER(FRDGPixelShader, "/BRPlugins/Private/SimplePixelShader.usf", "MainPS", SF_Pixel);
	/*
	void DrawView(FRDGBuilder& GraphBuilder, const FViewInfo& View, FRDGTextureRef OutputTexture)
	{
		check(OutputTexture);
		RDG_EVENT_SCOPE(GraphBuilder, "SimpleRDGDraw");

		// Initialize graph managed resources
		// Symbols buffer contains Count + 1 elements. The first element is only used as a counter.
		//FRDGBufferRef IndirectDrawArgsBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateIndirectDesc(0), TEXT("ShaderPrintIndirectDrawArgs"));
		//FRDGBufferRef MacroGroupAABBBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateBufferDesc(4, 6 * MacroGroupCount), TEXT("HairMacroGroupAABBBuffer"));

		FRDGTextureDesc TextureDesc = FRDGTextureDesc::Create2DDesc(
			FIntPoint(1024,1024),
			PF_FloatRGBA,
			FClearValueBinding::Black,
			TexCreate_None,
			TexCreate_ShaderResource | TexCreate_RenderTargetable | TexCreate_UAV,
			false
		);
		
		FRDGTexture* ColorTexture = GraphBuilder.CreateTexture(TextureDesc, TEXT("TextureUAV"));

		// Non graph managed resources

		const ERHIFeatureLevel::Type FeatureLevel = GMaxRHIFeatureLevel;	//ERHIFeatureLevel::SM5
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
		
		// SimpleRDGDraw
		{
			TShaderMapRef< FRDGVertexShader > VertexShader(GlobalShaderMap);
			TShaderMapRef< FRDGPixelShader > PixelShader(GlobalShaderMap);

			FRDGGlobalShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FRDGGlobalShader::FParameters>();
			PassParameters->RenderTargets[0] = FRenderTargetBinding(OutputTexture, ERenderTargetLoadAction::ENoAction);
			PassParameters->TextureUAV = GraphBuilder.CreateUAV(ColorTexture);
			
			//PassParameters->UniformBufferParameters.Color1 = FVector4(1.0f, 0.0f, 0.0f, 1.0f);
			//PassParameters->UniformBufferParameters.Color2 = FVector4(0.0f, 1.0f, 0.0f, 1.0f);
			//PassParameters->UniformBufferParameters.Color3 = FVector4(1.0f, 0.0f, 1.0f, 1.0f);
			//PassParameters->UniformBufferParameters.Color4 = FVector4(1.0f, 0.0f, 1.0f, 1.0f);
			//PassParameters->UniformBufferParameters.ColorIndex = 1;

			GraphBuilder.AddPass(
				RDG_EVENT_NAME("SimpleRDGDraw"),
				PassParameters,
				ERDGPassFlags::Raster,
				[VertexShader, PixelShader, PassParameters](FRHICommandListImmediate& RHICmdListImmediate)
				{
					FGraphicsPipelineStateInitializer GraphicsPSOInit;
					RHICmdListImmediate.ApplyCachedRenderTargets(GraphicsPSOInit);
					GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
					GraphicsPSOInit.BlendState = TStaticBlendState<CW_RGBA, BO_Add, BF_One, BF_InverseSourceAlpha, BO_Add, BF_Zero, BF_One>::GetRHI();
					GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
					GraphicsPSOInit.PrimitiveType = PT_TriangleList;
					GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GetVertexDeclarationFVector4();
					GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
					GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
					SetGraphicsPipelineState(RHICmdListImmediate, GraphicsPSOInit);

					SetShaderParameters(RHICmdListImmediate, VertexShader, VertexShader.GetVertexShader(), *PassParameters);
					SetShaderParameters(RHICmdListImmediate, PixelShader, PixelShader.GetPixelShader(), *PassParameters);

					//FPixelShaderUtils::DrawFullscreenPixelShader(RHICmdList, View.ShaderMap, *PixelShader, *PassParameters, FIntRect(0, 0, TexBufferSize.X, TexBufferSize.Y));

					/*
					const uint32 NumPrimitives = GRHISupportsRectTopology ? 1 : 2;
					const uint32 NumInstances = Parameters->VS.NumRects;
					RHICmdList.DrawPrimitive(0, NumPrimitives, NumInstances);
					
					
					DrawRectangle(
						RHICmdList,
						0, 0,
						Viewport.Width(), Viewport.Height(),
						Viewport.Min.X, Viewport.Min.Y,
						Viewport.Width(), Viewport.Height(),
						Viewport.Size(),
						Resolution,
						VertexShader,
						EDRF_UseTriangleOptimization);
					

					//RHICmdListImmediate.DrawIndexedPrimitiveIndirect(GTwoTrianglesIndexBuffer.IndexBufferRHI, PassParameters->IndirectDrawArgsBuffer->GetIndirectRHICallBuffer(), 0);

				});
		}

	}
	*/
	void RDGCompute(FRHICommandListImmediate& RHIImmCmdList, FRenderTarget* RenderTarget)
	{
		check(IsInRenderingThread());

		FTexture2DRHIRef RenderTargetRHI = RenderTarget->GetRenderTargetTexture();

		FSceneRenderTargetItem RenderTargetItem;
		RenderTargetItem.TargetableTexture = RenderTargetRHI;
		RenderTargetItem.ShaderResourceTexture = RenderTargetRHI;
		FPooledRenderTargetDesc RenderTargetDesc = FPooledRenderTargetDesc::Create2DDesc(RenderTarget->GetSizeXY(),
			RenderTargetRHI->GetFormat(), FClearValueBinding::Black, TexCreate_None, TexCreate_RenderTargetable |
			TexCreate_ShaderResource | TexCreate_UAV, false);
		TRefCountPtr<IPooledRenderTarget> PooledRenderTarget;
		//GRenderTargetPool.CreateUntrackedElement(RenderTargetDesc, PooledRenderTarget, RenderTargetItem);
		GRenderTargetPool.FindFreeElement(RHIImmCmdList, RenderTargetDesc, PooledRenderTarget, TEXT("DisplacementRenderTarget"));


		//RenderGraph部分开始
		FRDGBuilder GraphBuilder(RHIImmCmdList);
		FRDGTextureRef DisplacementRenderTargetRDG = GraphBuilder.RegisterExternalTexture(PooledRenderTarget, TEXT("DisplacementRenderTarget"));

		//设置变量与资源
		FRDGComputerShader::FParameters* Parameters = GraphBuilder.AllocParameters<FRDGComputerShader::FParameters>();

		FRDGTextureUAVDesc UAVDesc(DisplacementRenderTargetRDG);
		Parameters->OutTexture = GraphBuilder.CreateUAV(UAVDesc);
		//Parameters->RenderTargets[0] = FRenderTargetBinding(OutputTexture, ERenderTargetLoadAction::ENoAction);


		//从GlobalShaderMap中取得ComputeShader
		const ERHIFeatureLevel::Type FeatureLevel = GMaxRHIFeatureLevel;	//ERHIFeatureLevel::SM5
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
		TShaderMapRef<FRDGComputerShader> ComputeShader(GlobalShaderMap);
		//确定ComputerShader线程数
		FIntVector ThreadGroupCount(
			RenderTarget->GetSizeXY().X / 16,
			RenderTarget->GetSizeXY().Y / 16,
			1);
		
		//如果有多个AddPass需要调用这个函数
		//ClearUnusedGraphResources(ComputeShader, Parameters);

		GraphBuilder.AddPass(
			RDG_EVENT_NAME("SimpleRDG ComputerShader Dispatch"),
			Parameters,
			ERDGPassFlags::Compute | ERDGPassFlags::Compute,
			[Parameters, ComputeShader,ThreadGroupCount](FRHICommandList& RHICmdList)
			{
				FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *Parameters, ThreadGroupCount);
			});

		GraphBuilder.QueueTextureExtraction(DisplacementRenderTargetRDG, &PooledRenderTarget);
		GraphBuilder.Execute();

		//FRHICopyTextureInfo CopyInfo;
		//CopyInfo.NumMips = 1;
		//RHIImmCmdList.CopyTexture(
		//	DisplacementRenderTargetRDG->GetRHI(),
		//	RenderTargetRHI->GetTexture2D(),
		//	CopyInfo);
		
		//RHICmdList.CopyToResolveTarget(DisplacementRenderTargetRDG, DebugRenderTarget, FResolveParams());
		
	}
}


void USimplePixelShaderBlueprintLibrary::UseComputeShader(const UObject* WorldContextObject, UTextureRenderTarget2D* OutputRenderTarget)
{
	check(IsInGameThread());

	FRenderTarget* RenderTargetResource = OutputRenderTarget->GameThread_GetRenderTargetResource();
	const UWorld* World = WorldContextObject->GetWorld();
	//ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();

	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[RenderTargetResource](FRHICommandListImmediate& RHICmdList)
		{
			SimpleRGD::RDGCompute(RHICmdList,RenderTargetResource);
		});
}