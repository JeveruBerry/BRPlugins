#include "Rendering/SimpleRGD.h"
#include "Rendering/SimpleRGDParameters.h"

#include "CommonRenderResources.h"
#include "Containers/DynamicRHIResourceArray.h"
#include "Engine/Engine.h"
#include "GlobalShader.h"
#include "PipelineStateCache.h"

#include "Renderer/Private/SceneRendering.h"
//#include "Renderer/Private/SystemTextures.h"

namespace SimpleRGD {

	// Structure used by shader buffers to store values and symbols
	// 该结构被用于创建Shader中存储数值变量的缓存
	struct ShaderPrintItem
	{
		FVector2D ScreenPos;
		int32 Value;
		int32 Type;
	};

	// Supported platforms
	bool IsSupported(EShaderPlatform InShaderPlatform)
	{
		return RHISupportsComputeShaders(InShaderPlatform) && !IsHlslccShaderPlatform(InShaderPlatform);
	}

	bool IsSupported(FViewInfo const& View)
	{
		return IsSupported(View.GetShaderPlatform());
	}

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
		//	SHADER_PARAMETER_SRV(StructuredBuffer<ShaderPrintItem>, ValuesBuffer)
		//	SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<ShaderPrintItem>, RWSymbolsBuffer)
		//	SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<uint>, RWIndirectDispatchArgsBuffer)
		END_SHADER_PARAMETER_STRUCT()

		static bool ShouldCompilePermutation(FGlobalShaderPermutationParameters const& Parameters)
		{
			return IsSupported(Parameters.Platform);
		}
	};

	IMPLEMENT_GLOBAL_SHADER(FRDGComputerShader, "/Engine/Private/SimpleComputeShader.usf", "MainCS", SF_Compute);

	// Shader for draw pass to render each symbol
	class FRDGGlobalShader : public FGlobalShader
	{
	public:
		SHADER_USE_PARAMETER_STRUCT(FRDGGlobalShader, FGlobalShader);

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		//	RENDER_TARGET_BINDING_SLOTS()
		//	SHADER_PARAMETER_STRUCT_REF(FUniformBufferParameters, UniformBufferParameters)
		//	SHADER_PARAMETER_TEXTURE(Texture2D, MiniFontTexture)
		//	SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<ShaderPrintItem>, SymbolsBuffer)
		//	SHADER_PARAMETER_RDG_BUFFER(StructuredBuffer<uint>, IndirectDrawArgsBuffer)
		END_SHADER_PARAMETER_STRUCT()

		static bool ShouldCompilePermutation(FGlobalShaderPermutationParameters const& Parameters)
		{
			return IsSupported(Parameters.Platform);
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

	IMPLEMENT_GLOBAL_SHADER(FRDGVertexShader, "/Engine/Private/SimplePixelShader.usf", "MainVS", SF_Vertex);

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

	IMPLEMENT_GLOBAL_SHADER(FRDGPixelShader, "/Engine/Private/SimplePixelShader.usf", "MainPS", SF_Pixel);
}