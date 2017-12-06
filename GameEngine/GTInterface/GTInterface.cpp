#include "GTInterface.h"

#include "../glew/include/glew.h"
#include "../SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>


#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glew/libx86/glew32.lib")


GTI GTI::GTInterface = GTI();

GTI::GTI()
	{

	}

GTI::~GTI()
	{

	}

	void GTI::Init(uint screenWidth, uint screenHeight, float scale)
	{
		GTInterface.frustum.SetViewPlaneDistances(0.01f, FAR_PLANE_DISTANCE);
		GTInterface.frustum.SetWorldMatrix(float3x4::identity);
		GTInterface.frustum.SetPos(vec(0.0f, 0.0f, -FAR_PLANE_DISTANCE/2.0f));
		GTInterface.frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);

		GTInterface.frustum.SetOrthographic(screenWidth * scale, screenHeight * scale);
	}

	void GTI::Render()
	{
		DebugDraw::DrawFrustum(GTInterface.frustum);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadMatrixf(GTInterface.frustum.ProjectionMatrix().Transposed().ptr());

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Todo lo que se pinte aqui dentro es UI
		

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

	void GTI::ProcessEventSDL(SDL_Event & e)
	{
		/*switch (e.type)
		{
		case SDL_MOUSEWHEEL:
		{
			mouseZ = e.wheel.y;
			break;
		}

		case SDL_MOUSEMOTION:
		{
			mouseX = e.motion.x / SCREEN_SIZE;
			mouseY = e.motion.y / SCREEN_SIZE;

			mouseXMotion = e.motion.xrel / SCREEN_SIZE;
			mouseYMotion = e.motion.yrel / SCREEN_SIZE;
			break;
		}

		case SDL_WINDOWEVENT:
		{
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				App->window->UpdateWindowSize();
				App->renderer3D->OnResize(e.window.data1, e.window.data2);
			}
		}
		}*/
	}

	void GTI::DebugDraw::DrawLine(float3 &from, float3 &to, float3 color)
	{
		glDisable(GL_LIGHTING);
		glLineWidth(2.0f);

		glBegin(GL_LINES);
		glColor4f(color.x, color.y, color.z, 1.0f);

		glVertex3f(from.x, from.y, from.z); glVertex3f(to.x, to.y, to.z);

		glEnd();
		glLineWidth(1.0f);
		glEnable(GL_LIGHTING);
	}

	void GTI::DebugDraw::DrawFrustum(Frustum &frustum)
	{
		vec vector[8];
		frustum.GetCornerPoints(vector);

		float3 nearPlaneColor = float3(0, 1, 1);
		float3 frustumColor = float3(1, 0, 1);

		DrawLine(float3(vector[0].x, vector[0].y, vector[0].z), float3(vector[1].x, vector[1].y, vector[1].z), nearPlaneColor);
		DrawLine(float3(vector[2].x, vector[2].y, vector[2].z), float3(vector[3].x, vector[3].y, vector[3].z), nearPlaneColor);
		DrawLine(float3(vector[0].x, vector[0].y, vector[0].z), float3(vector[2].x, vector[2].y, vector[2].z), nearPlaneColor);
		DrawLine(float3(vector[1].x, vector[1].y, vector[1].z), float3(vector[3].x, vector[3].y, vector[3].z), nearPlaneColor);

		DrawLine(float3(vector[4].x, vector[4].y, vector[4].z), float3(vector[5].x, vector[5].y, vector[5].z), frustumColor);
		DrawLine(float3(vector[6].x, vector[6].y, vector[6].z), float3(vector[7].x, vector[7].y, vector[7].z), frustumColor);
		DrawLine(float3(vector[4].x, vector[4].y, vector[4].z), float3(vector[6].x, vector[6].y, vector[6].z), frustumColor);
		DrawLine(float3(vector[5].x, vector[5].y, vector[5].z), float3(vector[7].x, vector[7].y, vector[7].z), frustumColor);

		DrawLine(float3(vector[0].x, vector[0].y, vector[0].z), float3(vector[4].x, vector[4].y, vector[4].z), frustumColor);
		DrawLine(float3(vector[1].x, vector[1].y, vector[1].z), float3(vector[5].x, vector[5].y, vector[5].z), frustumColor);
		DrawLine(float3(vector[2].x, vector[2].y, vector[2].z), float3(vector[6].x, vector[6].y, vector[6].z), frustumColor);
		DrawLine(float3(vector[3].x, vector[3].y, vector[3].z), float3(vector[7].x, vector[7].y, vector[7].z), frustumColor);
	}




	GTI::UIElement::UIElement()
	{

	}
