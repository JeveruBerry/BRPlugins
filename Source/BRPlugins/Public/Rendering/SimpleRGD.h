#pragma once

#include "RenderGraph.h"
#include "Templates/RefCounting.h"

class FRHICommandListImmediate;
class FViewInfo;
struct IPooledRenderTarget;

namespace SimpleRGD
{
	// Does the platform support the ShaderPrint system?
	bool IsSupported(FViewInfo const& View);
	// Have we enabled the ShaderPrint system?
	bool IsEnabled();

	// Allocate the debug print buffer associated with the view
	void BeginView(FRHICommandListImmediate& RHICmdList, FViewInfo& View);
	// Draw info from the debug print buffer to the given output target
	void DrawView(FRDGBuilder& GraphBuilder, const FViewInfo& View, FRDGTextureRef OutputTexture);
	// Release the debug print buffer associated with the view
	void EndView(FViewInfo& View);
}
