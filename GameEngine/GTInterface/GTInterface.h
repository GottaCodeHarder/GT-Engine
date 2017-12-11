#ifndef GTInterface_H
#define GTInterface_H

#include "../SDL/include/SDL.h"
#include "../MathGeoLib/MathGeoLib.h"
#include "../Color.h"

#include <map>
#include <string>
#include <functional>

typedef unsigned int uint;

class GTI
{
	enum class UIElementType
	{
		Unidentified,
		Image,
		Button,
		Checkbox,
		Input
	};

	enum class TransparencyType
	{
		NONE,
		ALPHA_TEST,
		BLEND
	};

#pragma region UI ELEMENTS

public:

	class UIElement
	{
	public:
		UIElement(bool drag = false);

		virtual UIElementType GetType() { return UIElementType::Unidentified; }
		virtual void UpdatePos() {};
		virtual void OnClick() {};

		bool draggable;

		//TO TEST
		TransparencyType blendsType = TransparencyType::ALPHA_TEST;
		float alpha = 0.8;
		uint buffTexture = 0;

		//END TO TEST
		UIElement* parent = nullptr;

		float3 GetGlobalPosition();
		float3 GetGlobalScale();
		Quat GetGlobalRotation();
		float4x4 GetGlobalTransform();

		float3 positionLocal = { 0.f,0.f,0.f };
		float3 scaleLocal = { 1.f,1.f,1.f };
		Quat rotationLocal = Quat::identity;
	};

	class Image : public UIElement
	{
	public:
		Image(bool drag = false) : UIElement(drag) {};
		static UIElementType GetType() { return UIElementType::Image; }

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
		// std::vector<de 0 a 3> donde 0 es normal, 1 hover, 2 click
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

#pragma region EMITTER

	template <typename in>

	class Emitter
	{
	public:
		void Register(std::function<void(in)> func)
		{
			funcList.push_back(func);
		}

		template <typename C>
		void Register(C* object, void(C::* memFunc)(in))
		{
			Register([object = object, memFunc = memFunc](in arg) { (object->*(memFunc))(arg); });
		}

		void CallFunction(in param)
		{
			for (auto func : funcList)
			{
				func(param);
			}
		}

	private:
		std::vector<std::function<void(in)>> funcList;
	};


#pragma endregion

#define FAR_PLANE_DISTANCE 100.0f
private:
	GTI();
public:
	~GTI();

	static GTI GTInterface; // Brujeria

	static void Init(uint screenWidth, uint screenHeight, float scale = 0.5f);
	static void CleanUp();

	static void Render();
	static void RenderUIElement(UIElement* element, bool paintBlend = false);

	//To Test
	uint LoadTexture(char* path);
	//End To Test

	Image* AddImage();

	static void GetEventSDL(SDL_Event &e) { GTInterface.ProcessEventSDL(e); };
	static void UpdateWindowSize(int w, int h);

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
		static void DrawPlane(float3 planeVertex[]);
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