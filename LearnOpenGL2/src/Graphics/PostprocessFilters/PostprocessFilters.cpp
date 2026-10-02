#include "PostprocessFilters.h"

PostProcessFilters::PostProcessFilters()
{
	CurrentSelection = KernelType::Sharpen;
	UpdateKernel(CurrentSelection);
}

void PostProcessFilters::UpdateTextureOffset(float offset)
{
	textureOffset = offset;
}

void PostProcessFilters::UpdateBlurStrength(float strength)
{
	blurStrength = strength;
	UpdateKernel(KernelType::Blur);
}

void PostProcessFilters::UpdateKernel(KernelType Kernel)
{
	switch (Kernel)
	{
	case KernelType::Sharpen:
		kernel.clear();
		kernel = 
		{
			-1, -1, -1,
			-1,  9, -1,
			-1, -1, -1
		};
		CurrentSelection = KernelType::Sharpen;
		break;
	case KernelType::Blur:
		kernel.clear();
		kernel =
		{
			1.0f / blurStrength, 2.0f / blurStrength, 1.0f / blurStrength,
			2.0f / blurStrength, 4.0f / blurStrength, 2.0f / blurStrength,
			1.0f / blurStrength, 2.0f / blurStrength, 1.0f / blurStrength
		};
		CurrentSelection = KernelType::Blur;
		break;
	case KernelType::Edge:
		kernel.clear();
		kernel =
		{
			1,  1,  1,
			1, -8,  1,
			1,  1,  1
		};
		CurrentSelection = KernelType::Edge;
		break;
	}
}

float PostProcessFilters::GetTextureOffset()
{
	return textureOffset;
}

float PostProcessFilters::GetBlurStrength()
{
	return blurStrength;
}

KernelType PostProcessFilters::GetKernelSelection()
{
	return CurrentSelection;
}

std::vector<float> PostProcessFilters::GetKernel()
{
	return kernel;
}

float PostProcessFilters::GetKernelAt(int index)
{
	return kernel[index];
}
