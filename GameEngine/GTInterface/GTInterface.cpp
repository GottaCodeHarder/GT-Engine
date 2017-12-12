#include "../glew/include/glew.h"

#include "GTInterface.h"

#include "../Devil/include/il.h"
#include "../Devil/include/ilu.h"
#include "../Devil/include/ilut.h"

#include "../SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glew/libx86/glew32.lib")

#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma	comment (lib, "Devil/libx86/ILU.lib")
#pragma	comment (lib, "Devil/libx86/ILUT.lib")

GTI GTI::GTInterface = GTI();

GTI::GTI()
	{

	}

GTI::~GTI()
	{

	}

	void GTI::Init(uint screenWidth, uint screenHeight, float scale)
	{
		GTInterface.frustum.SetViewPlaneDistances(0.1f, FAR_PLANE_DISTANCE);
		GTInterface.frustum.SetWorldMatrix(float3x4::identity * float3x4::RotateY(3.14159265));
		
		GTInterface.frustum.SetPos(vec(0.0f, 0.0f, FAR_PLANE_DISTANCE/2.0f));
		GTInterface.frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);

		GTInterface.frustum.SetOrthographic(screenWidth * scale, screenHeight * scale);

		GTInterface.scale = scale;

		GTInterface.GeneratePlane();
	}

	void GTI::CleanUp()
	{
		
	}

	void GTI::Render()
	{
		DebugDraw::DrawFrustum(GTInterface.frustum);

		glDisable(GL_LIGHTING);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadMatrixf(GTInterface.frustum.ProjectionMatrix().Transposed().ptr());

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadMatrixf(float4x4(GTInterface.frustum.ViewMatrix()).Transposed().ptr());

		// Todo lo que se pinte aqui dentro es UI
		
		for (UIElement* element : GTInterface.UIElements)
		{
			RenderUIElement(element, false);
		}

		for (auto blendElement : GTInterface.blendElements)
		{
			RenderUIElement(blendElement.second, true);
		}

		GTInterface.blendElements.clear();

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glEnable(GL_LIGHTING);
	}

	void GTI::RenderUIElement(UIElement * element, bool paintBlend)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glPushMatrix();
		glMultMatrixf(element->GetGlobalTransform().Transposed().ptr());

		switch (element->blendsType)
		{
		case (TransparencyType::ALPHA_TEST):
		{
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, element->alpha);
			break;
		}
		case (TransparencyType::BLEND):
		{
			if (!paintBlend)
			{
				float3 tmp = element->GetGlobalPosition() - GTInterface.GetCameraTransform().TranslatePart();

				GTInterface.blendElements.insert(std::pair<float, UIElement*>(tmp.LengthSq(), element));
			}
			else if (paintBlend)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, 0); // 0 blendType modifiable
			}
			break;
		}
		}
		if ((element->buffTexture) > 0)
		{
			glEnableClientState(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, element->buffTexture);
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, GTInterface.vertexBuff);
		glVertexPointer(3, GL_FLOAT, 0, NULL);


		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, GTInterface.normalBuff);
		glNormalPointer(GL_FLOAT, 0, NULL);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, GTInterface.UVBuff);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GTInterface.indexBuff);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);


		// CleanUp
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_TEXTURE_2D);

		glPopMatrix();
	}

	uint GTI::LoadTexture(char * fullPath)
	{
		ilInit();

		ILuint imageID = 0;
		GLuint textureID;

		// Safe
		ILboolean success;
		ILenum error;

		ilGenImages(1, &imageID);
		ilBindImage(imageID);

		success = ilLoadImage(fullPath);
		if (success)
		{
			ILinfo ImageInfo;
			iluGetImageInfo(&ImageInfo);
			if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			{
				iluFlipImage();
			}

			success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

			if (!success)
			{
				error = ilGetError();
				return -1;
			}

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		}
		else
		{
			error = ilGetError();
			return -1;
		}
		return textureID;
	}

	GTI::Image* GTI::AddImage()
	{
		Image* ret = new Image();
		UIElements.push_back(ret);
		return ret;
	}

	void GTI::UpdateWindowSize(int w, int h)
	{
		GTInterface.frustum.SetOrthographic(w*GTInterface.scale, h*GTInterface.scale);
	}

	float4x4 GTI::GetCameraTransform() const
	{
		return frustum.WorldMatrix();
	}

	void GTI::ProcessEventSDL(SDL_Event & e)
	{
		switch (e.type)
		{
		case SDL_MOUSEWHEEL:
		{	
			break;
		}

		case SDL_MOUSEMOTION:
		{
			//mouseX = e.motion.x / SCREEN_SIZE;
			//mouseY = e.motion.y / SCREEN_SIZE;

			//mouseXMotion = e.motion.xrel / SCREEN_SIZE;
			//mouseYMotion = e.motion.yrel / SCREEN_SIZE;
			break;
		}

		case SDL_WINDOWEVENT:
		{
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				UpdateWindowSize(e.window.data1, e.window.data2);
			}
		}
		}
	}

	void GTI::GeneratePlane()
	{
		float3 vertex[4];
		vertex[0] = float3(-0.5f, -0.5f, 0.0f);
		vertex[1] = float3(0.5f, -0.5f, 0.0f);
		vertex[2] = float3(0.5f, 0.5f, 0.0f);
		vertex[3] = float3(-0.5f, 0.5f, 0.0f);

		float2 uvs[4]; // Warning
		uvs[0] = float2(0.0f, 0.0f);
		uvs[1] = float2(1.0f, 0.0f);
		uvs[2] = float2(1.0f, 1.0f);
		uvs[3] = float2(0.0f, 1.0f);
		
		float3 normals[4];
		normals[0] = float3(0.0f, 0.0f, -1.0f);
		normals[1] = float3(0.0f, 0.0f, -1.0f);
		normals[2] = float3(0.0f, 0.0f, -1.0f);
		normals[3] = float3(0.0f, 0.0f, -1.0f);

		uint index[6];
		index[0] = 0;
		index[1] = 1;
		index[2] = 3;
		index[3] = 1;
		index[4] = 2;
		index[5] = 3;

		glGenBuffers(1, (GLuint*) &(GTInterface.vertexBuff));
		glBindBuffer(GL_ARRAY_BUFFER, GTInterface.vertexBuff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * 4, vertex, GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*) &(GTInterface.normalBuff));
		glBindBuffer(GL_ARRAY_BUFFER, GTInterface.normalBuff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * 4, normals, GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*) &(GTInterface.UVBuff));
		glBindBuffer(GL_ARRAY_BUFFER, GTInterface.UVBuff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * 4, uvs, GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*) &(GTInterface.indexBuff));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GTInterface.indexBuff);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 6, index, GL_STATIC_DRAW);
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

	void GTI::DebugDraw::DrawPlane(float3 planeVertex[])
	{
		float3 planeColor = float3(0, 1, 1);
		DrawLine(float3(planeVertex[0].x, planeVertex[0].y, planeVertex[0].z), float3(planeVertex[1].x, planeVertex[1].y, planeVertex[1].z), planeColor);
		DrawLine(float3(planeVertex[2].x, planeVertex[2].y, planeVertex[2].z), float3(planeVertex[3].x, planeVertex[3].y, planeVertex[3].z), planeColor);
		DrawLine(float3(planeVertex[0].x, planeVertex[0].y, planeVertex[0].z), float3(planeVertex[3].x, planeVertex[3].y, planeVertex[3].z), planeColor);
		DrawLine(float3(planeVertex[1].x, planeVertex[1].y, planeVertex[1].z), float3(planeVertex[2].x, planeVertex[2].y, planeVertex[2].z), planeColor);
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




	GTI::UIElement::UIElement(bool drag) : draggable(drag)
	{

	}

	float3 GTI::UIElement::GetGlobalPosition()
	{
		float3 ret = positionLocal;
		if (parent != nullptr)
		{
			ret += parent->GetGlobalPosition();
		}
		return ret;
	}

	float3 GTI::UIElement::GetGlobalScale()
	{
		float3 ret = scaleLocal;
		if (parent != nullptr)
		{
			float3 parentGScale = parent->GetGlobalScale();
			ret = ret.Mul(parentGScale);
		}
		return ret;
	}

	Quat GTI::UIElement::GetGlobalRotation()
	{
		Quat ret = rotationLocal;
		if (parent != nullptr)
		{
			ret = ret * (parent->GetGlobalRotation());
		}
		return ret;
	}

	float4x4 GTI::UIElement::GetGlobalTransform()
	{
		float4x4 ret;
		ret = ret.FromTRS(GetGlobalPosition(), GetGlobalRotation(), GetGlobalScale());
		return ret;
	}
