#ifndef GTInterface_H
#define GTInterface_H

#include "../SDL/include/SDL.h"
#include "../MathGeoLib/MathGeoLib.h"
#include <string>
#include "../Color.h"

#include <map>

typedef unsigned int uint;

class GTI
{
	enum class UIElementType
	{
		Image,
		Button,
		Checkbox,
		Input
	};

	enum class TransparencyType
	{
		OPAQUE,
		ALPHA_TEST,
		BLEND
	};

#pragma region UI ELEMENTS

public:

	class UIElement
	{
	public:
		UIElement();
		UIElement(bool drag = false) : draggable(drag) {};

		virtual UIElementType GetType() = 0;
		virtual void UpdatePos() {};
		virtual void OnClick() {};

		bool draggable;

		//TO TEST
		TransparencyType blendsType = TransparencyType::ALPHA_TEST;
		float alpha = 0.8;
		uint buffTexture = 0;

		//END TO TEST
		UIElement* parent = nullptr;

		float3 GetGlobalPosition(); //TODO
		float3 GetGlobalScale();
		Quat GetGlobalRotation();
		float4x4 GetGlobalTransform(); //TODO

		float3 positionLocal = { 0.f,0.f,0.f };
		float3 scaleLocal = { 1.f,1.f,1.f };
		Quat rotationLocal = Quat::identity;
	};

	class Image : public UIElement
	{
		Image(bool drag = false) : UIElement(drag) {};
		static UIElementType GetType()
		{
			return UIElementType::Image;
		}

		std::string source;
		SDL_Surface surface;
		Color color;
	};

	class Button : public UIElement
	{
		Button(bool drag = false) : UIElement(drag) {};
		static UIElementType GetType()
		{
			return UIElementType::Button;
		}

		std::string source;
		SDL_Surface surface;
		// NEED A WAY TO SAVE FUNCTIONS
		//	Emitter<bool> emitter;

		// NEED A WAY TO CHANGE THE IMAGE BUFF
		// std::vector<de 0 a 3> donde 0 es click, 1 hover, 2 no click
	};

	class Checkbox : public UIElement
	{
		Checkbox(bool* ref, bool drag = false) : UIElement(drag),
			reference(ref) {};
		static UIElementType GetType()
		{
			return UIElementType::Checkbox;
		}

		void OnClick()
		{
			(*reference) = !(*reference);
		}

		bool* reference;
	};

	class Input : public UIElement
	{
		static UIElementType GetType()
		{
			return UIElementType::Input;
		}

		void OnClick()
		{
			// set as focused and listen for keyboard events
		}

		SDL_Surface background;
		SDL_Surface label;
		std::string text;
	};

#pragma endregion


#define FAR_PLANE_DISTANCE 100.0f
private:
	GTI();
public:
	~GTI();

	static GTI GTInterface; // Brujeria
	
	static void Init(uint screenWidth, uint screenHeight, float scale = 0.5f);

	
	static void Render();
	static void RenderUIElement(UIElement* element, bool paintBlend = false);

	//To Test
	uint LoadTexture(char* path);
	//End To Test

	static void GetEventSDL(SDL_Event &e) { GTInterface.ProcessEventSDL(e); };
	
	float4x4 GetCameraTransform() const;

private:
	std::vector<UIElement*> UIElements;
	std::multimap<float, UIElement*> blendElements;

	float scale;
	Frustum frustum;

	void ProcessEventSDL(SDL_Event &e);

	class DebugDraw
	{
	public:
		static void DrawLine(float3 &from, float3 &to, float3 color = float3::one);
		static void DrawFrustum(Frustum &frustum);
	};

	void GeneratePlane();

	uint vertexBuff = 0;
	uint normalBuff = 0;
	uint UVBuff = 0;

	uint textureBuff = 0;

	uint indexBuff = 0;
};

#endif