#include "../glew/include/glew.h"

#include "GTInterface.h"

#include "../Devil/include/il.h"
#include "../Devil/include/ilu.h"
#include "../Devil/include/ilut.h"

#include "../SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glew/libx86/glew32.lib")

#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma	comment (lib, "Devil/libx86/ILU.lib")
#pragma	comment (lib, "Devil/libx86/ILUT.lib")

#pragma comment (lib, "GTInterface/SDL_ttf/lib/x86/SDL2_ttf.lib")

GTI GTI::GTInterface = GTI();

GTI::GTI()
{}

GTI::~GTI()
{
	for (auto fonts : availableFonts)
		TTF_CloseFont(fonts.second.second);
}

void GTI::Init(uint screenWidth, uint screenHeight, float scale)
{
	TTF_Init();
	ilInit();

	GTInterface.frustum.SetViewPlaneDistances(0.1f, FAR_PLANE_DISTANCE);
	GTInterface.frustum.SetWorldMatrix(float3x4::identity * float3x4::RotateY(3.14159265));

	GTInterface.frustum.SetPos(vec(0.0f, 0.0f, FAR_PLANE_DISTANCE / 2.0f));
	GTInterface.frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);

	GTInterface.frustum.SetOrthographic(screenWidth * scale, screenHeight * scale);

	GTInterface.GeneratePlane();

	GTInterface.root.transform->w = screenWidth;
	GTInterface.root.transform->h = screenHeight;
	GTInterface.scale = scale;
	GTInterface.lastError = GTIError::NONE;
	GTInterface.mouseLBDown = false;
	
	GTInterface.timer.Start();

	/*FunctionEmitter<bool> emit;

	emit.Register<UIElement>(element, &GTI::UIElement::SetActive);

	emitt.AddFunction<GTI::UIElement>("WOW", a, &GTI::UIElement::SetActive);*/
}

void GTI::CleanUp()
{}

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
		if (element->GetType() != UIElementType::Button)
		{
			if (element->IsActive())
				RenderUIElement(element, false);
		}
	}

	for (auto blendElement : GTInterface.blendElements)
	{
		if (blendElement.second->GetType() != UIElementType::Button)
		{
			if (blendElement.second->IsActive())
				RenderUIElement(blendElement.second, true);
		}
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
	float4x4 transform = element->GetGlobalTransform();
	glMultMatrixf(transform.Transposed().ptr());

	switch (element->blendType)
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
			glBlendFunc(GL_SRC_ALPHA, element->blend);

			element->UpdateFade();
		}
		break;
	}
	}
	if (!(!paintBlend && element->blendType == TransparencyType::BLEND))
	{
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
	}

	// CleanUp
	glColor4f(1.0, 1.0, 1.0, 1.0);

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

uint GTI::LoadTexture(const char * fullPath, RectTransform* transform)
{
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
		if (ImageInfo.Origin != IL_ORIGIN_UPPER_LEFT)
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
		
		if (transform != nullptr)
		{
			transform->w = ilGetInteger(IL_IMAGE_WIDTH);
			transform->h = ilGetInteger(IL_IMAGE_HEIGHT);
		}
	}
	else
	{
		if (transform != nullptr)
		{
			transform->w = 200;
			transform->h = 200;
		}

		error = ilGetError();
		return -1;
	}

	return textureID;
}

uint GTI::GenerateText(std::string text, std::string fontName, uint size, SDL_Color color, RectTransform * transform)
{
	uint ret = 0;
	TTF_Font* font = FindFont(fontName, size);

	if (font != nullptr)
	{
		GLuint textureID;
		SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

		if (transform != nullptr)
		{
			transform->w = surface->w;
			transform->h = surface->h;
		}

		SDL_FreeSurface(surface);
		ret = textureID;
	}

	return ret;
}

void GTI::UpdateText(uint texBuffer, std::string text, std::string fontName, uint size, SDL_Color color, RectTransform * transform)
{
	TTF_Font* font = FindFont(fontName, size);

	if (font != nullptr)
	{
		SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, texBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

		if (transform != nullptr)
		{
			transform->w = surface->w;
			transform->h = surface->h;
		}

		SDL_FreeSurface(surface);
	}
}

std::string GTI::LoadFont(const char * path, uint size)
{
	TTF_Font* font = nullptr;
	std::vector<std::string> split = Splitpath(path, std::set<char>({ '\\' }));
	std::string name = split.back();

	if (GTInterface.availableFonts.count(name) > 0)
	{
		// check for different size
		std::pair <std::multimap<std::string, std::pair< uint, TTF_Font*>>::iterator,
			std::multimap<std::string, std::pair< uint, TTF_Font*>>::iterator>
			range = GTInterface.availableFonts.equal_range(name);

		for (std::multimap<std::string, std::pair< uint, TTF_Font*>>::iterator
			it = range.first; it != range.second; ++it)
		{
			if (it->second.first == size)
				font = it->second.second;
		}

		if (font == nullptr)
		{
			font = TTF_OpenFont(path, size);
			GTInterface.availableFonts.insert({ name.c_str(),{ size, font } });
		}
	}
	else
	{
		font = TTF_OpenFont(path, size);

		if (font != nullptr)
			GTInterface.availableFonts.insert({ name.c_str(),{ size, font } });
	}

	if (font == nullptr)
		name = "";

	return name;
}

GTI::UIElement* GTI::GetRoot()
{
	return (UIElement*)&GTInterface.root;
}

GTI::UIElement* GTI::GetFocus()
{
	return ((Canvas*)GTInterface.GetRoot())->focus;
}

void GTI::SetFocus(UIElement* focus, float gPosZ)
{
	GTInterface.root.focus = focus;
	((Canvas*)GTInterface.GetRoot())->minZ = focus == nullptr ? -FAR_PLANE_DISTANCE : gPosZ;
}

GTI::Image* GTI::CreateImage(UIElement* parent, char* path)
{
	Image* image = new Image(parent, path);
	UIElements.push_back(image);
	return image;
}

GTI::Label* GTI::CreateLabel(std::string text, std::string font, uint size, SDL_Color color, UIElement* parent)
{
	Label* label = nullptr;

	if (FindFont(font, size) == nullptr)
	{
		if (!availableFonts.empty())
		{
			label = new Label(text, availableFonts.begin()->first, availableFonts.begin()->second.first, color, parent);
			UIElements.push_back(label);
		}
		else
		{
			// ERROR: font not found & no other fonts loaded
		}
	}
	else
	{
		label = new Label(text, font, size, color, parent);
		UIElements.push_back(label);
	}
	return label;
}

GTI::Button* GTI::CreateButton(UIElement* parent)
{
	Button* button = new Button(parent);
	UIElements.push_back(button);
	return button;
}

GTI::Checkbox* GTI::CreateCheckbox(bool &ref, UIElement* parent)
{
	Checkbox* cbox = new Checkbox(ref, parent);
	UIElements.push_back(cbox);
	return cbox;
}

GTI::Input* GTI::CreateInput(UIElement* parent)
{
	Input* input = new Input(parent);
	UIElements.push_back(input);
	return input;
}

std::string GTI::GetLastError() const
{
	std::string error;
	switch(lastError)
	{
	case GTIError::NONE: error = "GTInterface encountered no errors"; break;
	case GTIError::IL_LOAD_IMG: error = "Il couldn't load image"; break;
	case GTIError::IL_CONVERT: error = "Il couldn't convert image to RGB"; break;
	}
	return error;
}

void GTI::GetEventSDL(SDL_Event &e)
{
	GTInterface.root.HandleEvent(e);
}

void GTI::UpdateWindowSize(int w, int h)
{
	GTInterface.frustum.SetOrthographic(w*GTInterface.scale, h*GTInterface.scale);
}

bool GTI::GetMLBDown() const
{
	return mouseLBDown;
}

float4x4 GTI::GetCameraTransform() const
{
	return frustum.WorldMatrix();
}

void GTI::GeneratePlane()
{
	float3 vertex[4];
	vertex[0] = float3(-0.5f, -0.5f, 0.0f);
	vertex[1] = float3(0.5f, -0.5f, 0.0f);
	vertex[2] = float3(0.5f, 0.5f, 0.0f);
	vertex[3] = float3(-0.5f, 0.5f, 0.0f);

	float2 uvs[4]; // Warning
	uvs[0] = float2(0.0f, 1.0f);
	uvs[1] = float2(1.0f, 1.0f);
	uvs[2] = float2(1.0f, 0.0f);
	uvs[3] = float2(0.0f, 0.0f);

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


// Code author: Anders K.'s
// https://stackoverflow.com/questions/8520560/get-a-file-name-from-a-path
std::vector<std::string> GTI::Splitpath(const std::string& str, const std::set<char> delimiters)
{
	std::vector<std::string> result;

	char const* pch = str.c_str();
	char const* start = pch;
	for (; *pch; ++pch)
	{
		if (delimiters.find(*pch) != delimiters.end())
		{
			if (start != pch)
			{
				std::string str(start, pch);
				result.push_back(str);
			}
			else
			{
				result.push_back("");
			}
			start = pch + 1;
		}
	}
	result.push_back(start);

	return result;
}

TTF_Font* GTI::FindFont(std::string fontName, uint size)
{
	if (GTInterface.availableFonts.count(fontName) > 0)
	{
		std::pair <std::multimap<std::string, std::pair< uint, TTF_Font*>>::iterator,
			std::multimap<std::string, std::pair< uint, TTF_Font*>>::iterator>
			range = GTInterface.availableFonts.equal_range(fontName);

		for (std::multimap<std::string, std::pair< uint, TTF_Font*>>::iterator
			it = range.first; it != range.second; ++it)
		{
			if (it->second.first == size)
			{
				return it->second.second;
			}
		}
	}
	return nullptr;
}




GTI::RectTransform::RectTransform()
{
	Reset();
}
void GTI::RectTransform::Reset()
{
	w = 0;
	h = 0;
	positionLocal = float3(0.0f);
	scaleLocal = float3(1.0f);
	rotationLocal = Quat::identity;
	anchorMin = { 0.5f, 0.5f };
	anchorMax = { 0.5f, 0.5f };
	pivot = { 0.5f, 0.5f };
}

GTI::UIElement::UIElement(UIElementType t, UIElement* _parent)
{
	type = t;
	parent = _parent;

	blendType = TransparencyType::ALPHA_TEST;
	buffTexture = 0;
	alpha = 0.8f;
	blend = GL_ONE_MINUS_SRC_ALPHA;

	if (_parent != nullptr)
		parent = _parent;
	else if (t > UIElementType::Canvas)
		parent = &GTI::GTInterface.root;
	else
		parent = nullptr;

	if (parent != nullptr)
		parent->AddSon(this);

	transform = new RectTransform();
}

GTI::UIElement::~UIElement()
{
	delete transform;
}


GTI::UIElementType GTI::UIElement::GetType() const
{
	return type;
}

void GTI::UIElement::AddSon(UIElement* son)
{
	if (son != nullptr)
	{
		son->parent = this;
		sons.push_back(son);
	}
}

void GTI::UIElement::OnClick()
{}

void GTI::UIElement::CheckMouseClick(SDL_MouseButtonEvent &e)
{
	if (Contains(e.x, e.y))
	{
		float3 gPos = GetGlobalPosition();

		for (std::vector<UIElement*>::iterator it = sons.begin(); it != sons.end(); ++it)
			(*it)->CheckMouseClick(e);

		if (GTInterface.GetFocus() == nullptr)
		{
			GTInterface.SetFocus(this, gPos.z);
		}
		else
		{
			Canvas* root = (Canvas*)GTInterface.GetRoot();
			if (gPos.z >= root->minZ)
				GTInterface.SetFocus(this, gPos.z);
		}
	}
}

void GTI::UIElement::CheckMouseMove(SDL_MouseMotionEvent &e)
{
	if (GTInterface.GetFocus() == this
		&& GTInterface.GetMLBDown()
		&& draggable)
	{
		mouseHover = true;
		// drag rectTransform
	}
	else if (Contains(e.x, e.y))
	{
		mouseHover = true;

		/*		x	- X coordinate, relative to window
		y	- Y coordinate, relative to window
		xrel	- relative motion in the X direction
		yrel	- relative motion in the Y direction*/

		for (std::vector<UIElement*>::iterator it = sons.begin(); it != sons.end(); ++it)
			(*it)->CheckMouseMove(e);
	}
	else
	{
		MouseHoverLeave();
	}
}

void GTI::UIElement::MouseHoverLeave()
{
	mouseHover = false;

	for (std::vector<UIElement*>::iterator it = sons.begin(); it != sons.end(); ++it)
		(*it)->MouseHoverLeave();
}

bool GTI::UIElement::Contains(int x, int y) const
{
	return (x >= MinX() && x <= MaxX() && y >= MinY() && y <= MaxY());
}

int GTI::UIElement::MinX() const
{
	int ret = 0;
	float3 pos = GetGlobalPosition();
	float3 scale = GetGlobalScale();

	ret = (GTInterface.GetRoot()->transform->w / 2) + pos.x; // center
	ret -= transform->w * transform->pivot.x * scale.x; // displace
														// TODO?? Apply rotation??

	return ret;
}

int GTI::UIElement::MinY() const
{
	int ret = 0;
	float3 pos = GetGlobalPosition();
	float3 scale = GetGlobalScale();

	ret = (GTInterface.GetRoot()->transform->h / 2) + pos.y; // center
	ret -= transform->h * transform->pivot.y * scale.y; // displace
														// TODO?? Apply rotation??

	return ret;
}

int GTI::UIElement::MaxX() const
{
	int ret = 0;
	float3 pos = GetGlobalPosition();
	float3 scale = GetGlobalScale();

	ret = (GTInterface.GetRoot()->transform->w / 2) + pos.x; // center
	ret += transform->w * transform->pivot.x * scale.x; // displace
														// TODO?? Apply rotation??

	return ret;
}

int GTI::UIElement::MaxY() const
{
	int ret = 0;
	float3 pos = GetGlobalPosition();
	float3 scale = GetGlobalScale();

	ret = (GTInterface.GetRoot()->transform->h / 2) + pos.y; // center
	ret += transform->h * transform->pivot.y * scale.y; // displace
														// TODO?? Apply rotation??

	return ret;
}

float3 GTI::UIElement::GetGlobalPosition() const
{
	float3 ret = transform->positionLocal;
	if (parent != nullptr)
	{
		ret += parent->GetGlobalPosition();
	}
	return ret;
}

float3 GTI::UIElement::GetGlobalScale() const
{
	float3 ret = transform->scaleLocal;
	if (parent != nullptr)
	{
		float3 parentGScale = parent->GetGlobalScale();
		ret = ret.Mul(parentGScale);
	}
	return ret;
}

Quat GTI::UIElement::GetGlobalRotation() const
{
	Quat ret = transform->rotationLocal;
	if (parent != nullptr)
	{
		ret = ret * (parent->GetGlobalRotation());
	}
	return ret;
}

float4x4 GTI::UIElement::GetGlobalTransform() const
{
	float4x4 ret;
	ret = ret.FromTRS(GetGlobalPosition(), GetGlobalRotation(), GetGlobalScale());
	return ret;
}



void GTI::UIElement::StartFade(float msDuration)
{
	fadeDuration = msDuration;
	fadeStart = GTInterface.timer.Read() + msDuration;
}

void GTI::UIElement::UpdateFade()
{
	float time = GTInterface.timer.Read();

	if (time <= fadeStart && fadeDuration != 0.0f)
	{
		for (; fadeAlpha > 0.0f; fadeAlpha = (1.0f - (time - fadeStart) / fadeDuration))
		{
			if (fadeAlpha < 0.0f)
			{
				fadeAlpha = 0.0f;
			}
			glColor4f(1.0f, 1.0f, 1.0f, fadeAlpha);
		}
	}
	else
	{
		fadeDuration = 0.0f;
	}
}

bool GTI::UIElement::IsActive() const
{
	return active;
}

void GTI::UIElement::SetActive(bool set)
{
	active = set;
}



GTI::Canvas::Canvas() : GTI::UIElement(UIElementType::Canvas)
{
	frustum = nullptr;
	interactable = true;
	focus = nullptr;
	minZ = -FAR_PLANE_DISTANCE;
}

void GTI::Canvas::HandleEvent(SDL_Event & e)
{
	if (!interactable)
		return;

	switch (e.type)
	{
	case SDL_MOUSEBUTTONDOWN:
	{
		if (e.button.button == SDL_BUTTON_LEFT)
		{
			focus = nullptr;

			for (std::vector<UIElement*>::iterator it = sons.begin(); it != sons.end(); ++it)
				(*it)->CheckMouseClick(e.button);

			if (focus != nullptr)
				focus->OnClick();
		}
		break;
	}
	case SDL_MOUSEBUTTONUP:
	{
		if (focus != nullptr && focus->GetType() != UIElementType::Input)
			focus = nullptr;
		break;
	}
	case SDL_MOUSEMOTION:
	{
		// Button Hover and Image Drag
		for (std::vector<UIElement*>::iterator it = sons.begin(); it != sons.end(); ++it)
			(*it)->CheckMouseMove(e.motion);

		break;
	}
	case SDL_TEXTINPUT:
	{
		if (focus != nullptr && focus->GetType() == UIElementType::Input)
		{
			Input* input = (Input*)focus;
			input->Write(e.text.text);
		}

		break;
	}
	case SDL_WINDOWEVENT:
	{
		if (e.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			// 1. Change transform w and h
			// 2. ResizeSons(...)

			// new width  = e.window.data1
			// new height = e.window.data2
		}
	}
	}
}

GTI::Image::Image(UIElement* _parent, char* path) : UIElement(UIElementType::Image, _parent)
{
	SetImage(path);
}

void GTI::Image::SetImage(char* path)
{
	buffTexture = LoadTexture(path, transform); 
}

void GTI::Image::OnClick()
{
	transform->scaleLocal.x *= 1.25f;
}

GTI::Label::Label(std::string _text, std::string _font, uint _size, SDL_Color _color, UIElement* _parent) : UIElement(UIElementType::Button, _parent)
{
	text = _text;
	font = _font;
	size = _size;
	color = _color;
	buffTexture = GenerateText(text, font, size, color, transform);
}

void GTI::Label::SetText(char* t)
{
	if (t == nullptr)
	{
		if (text.compare("") != 0)
		{
			text = "";
			UpdateText(buffTexture, text, font, size, color, transform);
		}
	}
	else if (text.compare(t) != 0)
	{
		text = t;
		UpdateText(buffTexture, text, font, size, color, transform);
	}
}

bool GTI::Label::SetFont(std::string _font, uint _size)
{
	bool ret;

	if (FindFont(_font, _size) != nullptr)
	{
		font = _font;
		size = _size;
		UpdateText(buffTexture, text, font, size, color, transform);
		ret = true;
	}

	return ret;
}

void GTI::Label::OnClick()
{
	// TODO SAMU
	//stringEmitter.CallFunction(text);
	SetText("I tried");
}

GTI::Button::Button(UIElement* _parent) : UIElement(UIElementType::Button, _parent)
{

}

void GTI::Button::OnClick()
{
	boolEmitter.CallFunction(valueBool);
	floatEmitter.CallFunction(valueFloat);
}

GTI::Checkbox::Checkbox(bool &ref, UIElement* _parent) : UIElement(UIElementType::Input, _parent)
{
	value = &ref;
}

GTI::Input::Input(UIElement* _parent) : UIElement(UIElementType::Checkbox, _parent)
{

}

void GTI::Input::Write(char* key)
{
	// if we had a cursor, we could write at specific position
	text += key;
}