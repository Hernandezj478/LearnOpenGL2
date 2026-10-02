#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "sstream"
#include "iomanip"

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoord;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	Vertex() : Position(), TexCoord(), Normal(), Tangent(), Bitangent() {}
	Vertex(glm::vec3 pos, glm::vec2 tex) : Position(pos), TexCoord(tex), Normal(), Tangent(), Bitangent() {}
	Vertex(glm::vec3 pos, glm::vec2 tex, glm::vec3 norm) : Position(pos), TexCoord(tex), Normal(norm), Tangent(), Bitangent() {}
	Vertex(glm::vec3 pos, glm::vec2 tex, glm::vec3 norm, glm::vec3 tan, glm::vec3 bitan) :
		Position(pos), TexCoord(tex), Normal(norm), Tangent(tan), Bitangent(bitan) {}

	std::string GetDataAsString()
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(1) <<
			"Position	{" << Position.x	<< ", " << Position.y	<< ", " << Position.z	<< "}" << std::endl <<
			"TexCoord	{" << TexCoord.x	<< ", " << TexCoord.y	<< "}"	<< std::endl	<<
			"Normal		{" << Normal.x		<< ", " << Normal.y		<< ", " << Normal.z		<< "}" << std::endl <<
			"Tangent	{" << Tangent.x		<< ", " << Tangent.y	<< ", " << Tangent.z	<< "}" << std::endl <<
			"Bitangent	{" << Bitangent.x	<< ", " << Bitangent.y	<< ", " << Bitangent.z	<< "}" << std::endl;
		return stream.str();
	}
	bool operator==(const Vertex& other) const
	{
		return  Position == other.Position &&
			TexCoord == other.TexCoord &&
			Normal == other.Normal &&
			Tangent == other.Tangent &&
			Bitangent == other.Bitangent;
	}
};

struct Vertex2D
{
	glm::vec2 Position;
	glm::vec2 TexCoord;


	Vertex2D() : Position(), TexCoord() {}
	Vertex2D(glm::vec2 position, glm::vec2 tex) : Position(position), TexCoord(tex) {}


	std::string Print()
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(1) <<
			"Position {" << Position.x << ", " << Position.y << "}" << std::endl <<
			"TexCoord {" << TexCoord.x << ", " << TexCoord.y << "}" << std::endl;
		return stream.str();
	}
	bool operator==(const Vertex2D& other) const
	{
		return Position == other.Position &&
			TexCoord == other.TexCoord;
	}

};

namespace std {
	template<> struct hash<glm::vec2> {
		size_t operator()(const glm::vec2& v) const {
			return hash<float>()(v.x) ^ (hash<float>()(v.y) << 1);
		}
	};

	template<> struct hash<glm::vec3> {
		size_t operator()(const glm::vec3& v) const {
			return hash<float>()(v.x) ^ (hash<float>()(v.y) << 1) ^ (hash<float>()(v.z) << 2);
		}
	};

	// Vertex hash
	template<> struct hash<Vertex> {
		size_t operator()(const Vertex& v) const {
			return hash<glm::vec3>()(v.Position) ^ hash<glm::vec2>()(v.TexCoord) ^ hash<glm::vec3>()(v.Normal) ^
				hash<glm::vec3>()(v.Tangent) ^ hash<glm::vec3>()(v.Bitangent);
		}
	};
}

struct Line
{
	glm::vec3 p1;
	glm::vec3 p2;
	std::string Print()
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(1) << 
			"{" << p1.x << ", " << p1.y << ", " << p1.z << "}" << std::endl <<
			"{" << p2.x << ", " << p2.y << ", " << p2.z << "}" << std::endl;
		return stream.str();
	}
};