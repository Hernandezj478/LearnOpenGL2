#include <iostream>

#include "Vertex.h"

int main()
{
	//Cube
	v3 cubeVerts[] =
	{
		{-0.5f, -0.5f, -0.5f},	//P0	//0
		{ 0.5f, -0.5f, -0.5f},	//P1	//1
		{ 0.5f,  0.5f, -0.5f},	//P2	//2
		{-0.5f,  0.5f, -0.5f},	//P3	//3
		{-0.5f, -0.5f,  0.5f},	//P4	//4
		{ 0.5f, -0.5f,  0.5f},	//P5	//5
		{ 0.5f,  0.5f,  0.5f},	//P6	//6
		{-0.5f,  0.5f,  0.5f}	//P7	//7
	};
	v2 cubeTexCoords[] =
	{
		{0.0f, 0.0f},	//T0
		{1.0f, 0.0f},	//T1
		{1.0f, 1.0f},	//T2
		{0.0f, 1.0f},	//T3
	};
	Vertex cubeVerticies[] =
	{
		{cubeVerts[0],cubeTexCoords[0]},	//P0 T0	//0
		{cubeVerts[1],cubeTexCoords[1]},	//P1 T1	//1
		{cubeVerts[2],cubeTexCoords[2]},	//P2 T2	//2
		{cubeVerts[3],cubeTexCoords[3]},	//P3 T3	//3
		{cubeVerts[4],cubeTexCoords[0]},	//P4 T0	//4
		{cubeVerts[5],cubeTexCoords[1]},	//P5 T1	//5
		{cubeVerts[6],cubeTexCoords[2]},	//P6 T2	//6	
		{cubeVerts[7],cubeTexCoords[3]},	//P7 T3	//7
		{cubeVerts[7],cubeTexCoords[1]},	//P7 T1	//8
		{cubeVerts[3],cubeTexCoords[2]},	//P3 T2	//9
		{cubeVerts[0],cubeTexCoords[3]},	//P0 T3	//10
		{cubeVerts[6],cubeTexCoords[1]},	//P6 T1	//11
		{cubeVerts[1],cubeTexCoords[3]},	//P1 T3	//12
		{cubeVerts[5],cubeTexCoords[0]},	//P5 T0	//13
		{cubeVerts[1],cubeTexCoords[2]},	//P1 T2	//14
		{cubeVerts[7],cubeTexCoords[0]}		//P7 T0	//15
	};

	unsigned int cubeIndices[] =
	{
		  0,  1,  2,  2,  3,  0,
		  4,  5,  6,  6,  7,  4,
		  8,  9, 10, 10,  4,  8,
		 11,  2, 12, 12, 13, 11,
		 10, 14,  5,  5,  4, 10,
		  3,  2, 11, 11, 15,  3
	};

	std::cout << sizeof(cubeVerticies) / sizeof(float) << std::endl;
	std::cout << sizeof(cubeIndices) / sizeof(unsigned int);

	return 0;
}
