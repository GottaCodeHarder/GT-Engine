#ifndef __RESOURCETEXTURE__
#define __RESOURCETEXTURE__

#include "Resources.h"

class ResourceTexture : public Resources
{
	componentType GetType()
	{
		return MATERIAL;
	}

};

#endif // !__RESOURCES__
