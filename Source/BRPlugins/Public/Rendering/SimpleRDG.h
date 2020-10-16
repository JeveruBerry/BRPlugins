#pragma once
//#include "Templates/RefCounting.h"
#include "RenderGraph.h"

//BlueprintLibraryHeader
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimpleRDG.generated.h"

class FRHICommandListImmediate;
struct IPooledRenderTarget;

namespace SimpleRGD
{
	BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FBRUniformBufferParameters, )
		SHADER_PARAMETER(FVector4, Color1)
		SHADER_PARAMETER(FVector4, Color2)
		SHADER_PARAMETER(FVector4, Color3)
		SHADER_PARAMETER(FVector4, Color4)
		SHADER_PARAMETER(uint32, ColorIndex)
	END_GLOBAL_SHADER_PARAMETER_STRUCT()

	typedef TUniformBufferRef<FBRUniformBufferParameters> FBRUniformBufferRef;

	void RDGCompute(FRHICommandListImmediate& RHIImmCmdList, FRenderTarget* RenderTarget);
}

UCLASS(MinimalAPI, meta=(ScriptName="SimplePixelShaderTest"))
class USimplePixelShaderBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "SimplePixelShader", meta = (WorldContext = "WorldContextObject"))
	static void UseComputeShader(const UObject* WorldContextObject, UTextureRenderTarget2D* OutputRenderTarget);
};
