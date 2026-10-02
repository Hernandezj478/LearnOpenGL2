#pragma once
#include <vector>

enum class KernelType
{
	Sharpen,
	Blur,
	Edge
};

class PostProcessFilters
{
public:
	PostProcessFilters();

	void UpdateTextureOffset(float offset);
	void UpdateBlurStrength(float strength);
	void UpdateKernel(KernelType Kernel);
	float GetTextureOffset();
	float GetBlurStrength();
	KernelType GetKernelSelection();
	std::vector<float> GetKernel();
	float GetKernelAt(int index);
private:
	KernelType CurrentSelection;
	float textureOffset = 1.0f / 300.0f;
	float blurStrength = 16.0f;
	std::vector<float> kernel;
};