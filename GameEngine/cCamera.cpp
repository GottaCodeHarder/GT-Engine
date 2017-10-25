#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "cCamera.h"
#include "cTransform.h"

cCamera::cCamera(GameObject* _gameObject) : Component(CAMERA, _gameObject)
{
	transform = new cTransform(_gameObject);

	frustum.SetPos(vec(20, 20, 20));
	frustum.SetFront(vec(20, 20, 20));
	frustum.SetUp(vec(20, 20, 20));
	frustum.SetOrthographic(10, 10);

}

cCamera::~cCamera()
{

}

void cCamera::DrawUI()
{

}
