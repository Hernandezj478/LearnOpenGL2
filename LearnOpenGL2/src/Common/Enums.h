#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

enum DTYPE
{
	BYTE	= GL_BYTE,
	UBYTE	= GL_UNSIGNED_BYTE,
	INT		= GL_INT,
	UINT	= GL_UNSIGNED_INT,
	FLOAT	= GL_FLOAT
};

enum FileType
{
	UNSUPPORTED = -1,
	JPG = 0,
	PNG = 1
};

enum WrapType
{
	REPEAT = GL_REPEAT,
	MREPEAT = GL_MIRRORED_REPEAT,
	ECLAMP = GL_CLAMP_TO_EDGE,
	BCLAMP = GL_CLAMP_TO_BORDER
};

enum TextureType
{
	DIFFUSE = 0,
	SPECULAR = 1,
	NORMAL = 2,
	EMISSIVE = 3,
	HEIGHT = 4,
	AMBIENT = 5,
	AO = 6,
	ROUGHNESS = 7,
	UNKNOWN = -1
};

enum class TextureTarget
{
	Texture2D = GL_TEXTURE_2D,
	Texture2DMS = GL_TEXTURE_2D_MULTISAMPLE
};