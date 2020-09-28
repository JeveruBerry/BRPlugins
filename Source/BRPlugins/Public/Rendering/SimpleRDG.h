#pragma once

#include "RenderGraph.h"
#include "Templates/RefCounting.h"

//BlueprintLibraryHeader
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimpleRDG.generated.h"

class FRHICommandListImmediate;
struct IPooledRenderTarget;

namespace SimpleRGD
{
	// Draw info from the debug print buffer to the given output target
	void DrawView(FRDGBuilder& GraphBuilder, const FViewInfo& View, FRDGTextureRef OutputTexture);

	void Render(FRHICommandListImmediate& RHIImmCmdList, FRDGTextureRef InTexture);
}

UCLASS(MinimalAPI, meta=(ScriptName="SimplePixelShaderTest"))
class USimplePixelShaderBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "SimplePixelShader", meta = (WorldContext = "WorldContextObject"))
	static void UseComputeShader(const UObject* WorldContextObject, UTextureRenderTarget2D* OutputRenderTarget);
};
