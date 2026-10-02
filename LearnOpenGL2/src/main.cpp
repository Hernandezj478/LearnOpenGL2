#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/Application.h"
#include "Scene/RegisterScene.h"
#include <iostream>
#include <vector>

int main()
{
	try
	{
		Application App;
		RegisterScenes(App.Registry());
		App.Run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}