#ifndef GTInterface_H
#define GTInterface_H

#include "../SDL/include/SDL.h"
#include "../MathGeoLib/MathGeoLib.h"



typedef unsigned int uint;


class GTI
{

#define FAR_PLANE_DISTANCE 100.0f
private:
	GTI();
public:
	~GTI();

	static GTI GTInterface; // Brujeria

	static void Init(uint screenWidth, uint screenHeight, float scale = 0.5f);

	//
	static void Render();
	static void GetEventSDL(SDL_Event &e) { GTInterface.ProcessEventSDL(e); };

private:
	float scale;
	Frustum frustum;

	void ProcessEventSDL(SDL_Event &e);

	class DebugDraw
	{
	public:
		static void DrawLine(float3 &from, float3 &to, float3 color = float3::one);
		static void DrawFrustum(Frustum &frustum);
	};


	enum class UIElementType
	{
		Image,
		Button,
		Checkbox,
		Input
	};

	class UIElement
	{
	public:
		UIElement();

		virtual UIElementType GetType() = 0;

	};

	class Image : public UIElement
	{
		static UIElementType GetType()
		{
			return UIElementType::Image;
		}
	};

};


#endif