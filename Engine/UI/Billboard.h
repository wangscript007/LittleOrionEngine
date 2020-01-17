#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include "Texture.h"

#include <string>
#include <MathGeoLib.h>
#include <GL/glew.h>

class Billboard
{
public:
	Billboard(const std::string& texture_path, float width, float height);
	~Billboard();

	void Render(const float3& position) const;

private:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;

	float width = 5.f;
	float height = 5.f;
	Texture* billboard_texture = nullptr;

};

#endif //_BILLBOARD_H_