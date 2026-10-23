#include <iostream>
#include <filesystem>
#include <string>

#include <glm/glm.hpp>

int main()
{
	glm::mat4 matrix(1.0f);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << matrix[i][j] << (j < 3 ? ", " : "");
		}
		std::cout << "\n";
	}
	return 0;
}
