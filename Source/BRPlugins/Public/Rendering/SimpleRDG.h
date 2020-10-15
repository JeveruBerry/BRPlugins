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
	void RDGCompute(FRHICommandListImmediate& RHIImmCmdList, FRenderTarget* RenderTarget);
}

UCLASS(MinimalAPI, meta=(ScriptName="SimplePixelShaderTest"))
class USimplePixelShaderBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "SimplePixelShader", meta = (WorldContext = "WorldContextObject"))
	static void UseComputeShader(const UObject* WorldContextObject, UTextureRenderTarget2D* OutputRenderTarget);
};
