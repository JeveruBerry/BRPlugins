#pragma once

#include "CoreMinimal.h"
#include "ShaderParameterMacros.h"
#include "ShaderParameters.h"

class FArchive;
class FRHICommandListImmediate;
class FShaderParameterMap;
class FViewInfo;

namespace SimpleRGD
{

	// ShaderPrint uniform buffer layout
	BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FUniformBufferParameters, )
		SHADER_PARAMETER(FVector4, FontSize)
		SHADER_PARAMETER(int32, MaxValueCount)
		SHADER_PARAMETER(int32, MaxSymbolCount)
		END_GLOBAL_SHADER_PARAMETER_STRUCT()

		// ShaderPrint parameter struct declaration
		BEGIN_SHADER_PARAMETER_STRUCT(FShaderParameters, )
		SHADER_PARAMETER_STRUCT_REF(FUniformBufferParameters, UniformBufferParameters)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<ShaderPrintItem>, RWValuesBuffer)
		END_SHADER_PARAMETER_STRUCT()

		// Call this to fill the FShaderParameters
		void SetParameters(FViewInfo const& View, FShaderParameters& OutParameters);


	// Legacy parameter binding helper for an FShader that doesn't use FParameters style parameter declaration
	struct FShaderParametersLegacy
	{
		DECLARE_TYPE_LAYOUT(FShaderParametersLegacy, NonVirtual);
	public:
		// Call on shader construction to bind
		void Bind(FShaderParameterMap const& ParameterMap);

		// Call to set parameters
		void SetParameters(FRHICommandListImmediate& RHICmdList, FRHIVertexShader* ShaderRHI, FViewInfo const& View);
		void SetParameters(FRHICommandListImmediate& RHICmdList, FRHIPixelShader* ShaderRHI, FViewInfo const& View);
		void SetParameters(FRHICommandListImmediate& RHICmdList, FRHIComputeShader* ShaderRHI, FViewInfo const& View);

		// Call to unbind the UAV
		void UnsetUAV(FRHICommandListImmediate& RHICmdList, FRHIComputeShader* ShaderRHI);

		// Serializer
		friend FArchive& operator<<(FArchive& Ar, FShaderParametersLegacy& P);

		// Parameters
		LAYOUT_FIELD(FShaderUniformBufferParameter, UniformBufferParameter);
		LAYOUT_FIELD(FRWShaderParameter, ValuesBufferParameter);
	};
}
