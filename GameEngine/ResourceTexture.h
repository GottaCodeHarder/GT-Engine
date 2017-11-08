#ifndef __RESOURCETEXTURE__
#define __RESOURCETEXTURE__

#include "Resources.h"
#include "glew\include\glew.h"

class ResourceTexture : public Resources
{
public:
	componentType GetType()
	{
		return MATERIAL;
	}

	void Destroy()
	{
		glDeleteBuffers(1, &buffTexture);
	}

	uint buffTexture = 0;
	ImVec2 imageDimensions;
	std::string path;
	float3 color;

};

#endif // !__RESOURCES__
