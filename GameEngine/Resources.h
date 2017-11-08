#ifndef __RESOURCES__
#define __RESOURCES__
#include "Globals.h"
#include "Component.h"

class Resources
{
public:
	uint nReferences = 0;

	virtual componentType GetType() = 0;

	template <typename T>
	T* Read() { return (T*)this; }

	std::string name;
};

#endif // !__RESOURCES__
