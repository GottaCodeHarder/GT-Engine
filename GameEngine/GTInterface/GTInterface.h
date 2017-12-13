#ifndef GTInterface_H
#define GTInterface_H

#include "../SDL/include/SDL.h"
#include "../MathGeoLib/MathGeoLib.h"

#include <map>
#include <string>
#include <functional>

typedef unsigned int uint;

class GTI
{
	enum class UIElementType
	{
		Unidentified = -1,
		Canvas,
		Image,
		Label,
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

	struct RectTransform
	{
		RectTransform();
		void Reset();

		uint w;
		uint h;
		float3 positionLocal;
		float3 scaleLocal;
		Quat rotationLocal;

		// Unity also sets pivot & anchor variables here
	};

	class UIElement
	{
	public:
		UIElement(UIElementType t, UIElement* _parent = nullptr, const bool drag = false);
		~UIElement();

		UIElementType GetType() const { return type; }
		virtual void HandleEvent(SDL_Event & e) {};

		float3 GetGlobalPosition();
		float3 GetGlobalScale();
		Quat GetGlobalRotation();
		float4x4 GetGlobalTransform();

	public:
		bool draggable;
		TransparencyType blendsType;
		float alpha;
		uint buffTexture;
		UIElement* parent;

		RectTransform* transform;

	private:
		UIElementType type;
	};

	class Canvas : public UIElement
	{
	public:
		Canvas();
		void HandleEvent(SDL_Event & e) {};
		Frustum* frustum;
	};

	class Image : public UIElement
	{
	public:
		Image(UIElement* _parent = nullptr, char* path = nullptr, bool drag = false);
		static UIElementType GetType() { return UIElementType::Image; }
		void HandleEvent(SDL_Event & e) {};
		void SetImage(char* path = nullptr);
		std::string source;
	};

	class Label : public UIElement
	{
	public:
		Label(UIElement* _parent = nullptr, bool drag = false);
		void HandleEvent(SDL_Event & e) {};
		void SetText(const char* t = nullptr);
		std::string text;
	private:
		void UpdateBuffer();
	};

	class Button : public UIElement
	{
	public:
		Button(UIElement* _parent = nullptr, bool drag = false);
		void HandleEvent(SDL_Event & e) {};
		// NEED A WAY TO SAVE FUNCTIONS
		//	Emitter<bool> emitter;

		// NEED A WAY TO CHANGE THE IMAGE BUFF
		// std::vector<de 0 a 3> donde 0 es normal, 1 hover, 2 click
	};

	class Checkbox : public UIElement
	{
	public:
		Checkbox(bool* ref, UIElement* _parent = nullptr, bool drag = false);
		void HandleEvent(SDL_Event & e) {};
		bool* reference;
	};

	class Input : public UIElement
	{
	public:
		Input(UIElement* _parent = nullptr, bool drag = false);
		void HandleEvent(SDL_Event & e) {};
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

	static uint LoadTexture(const char* path, RectTransform* transform);

	UIElement* GetRoot() const;

	Image* CreateImage(UIElement* parent = nullptr, char* path = nullptr);
	Label* CreateLabel(UIElement* parent = nullptr);
	Button* CreateButton(UIElement* parent = nullptr);
	Checkbox* CreateCheckbox(bool* ref = nullptr, UIElement* parent = nullptr);
	Input* CreateInput(UIElement* parent = nullptr);

	std::string GetPath(const char* filename) const;
	std::string GetLastError() const;

	static void GetEventSDL(SDL_Event &e) { GTInterface.ProcessEventSDL(e); };
	static void UpdateWindowSize(int w, int h);

	float4x4 GetCameraTransform() const;

private:
	std::vector<UIElement*> UIElements;
	std::multimap<float, UIElement*> blendElements;

	float scale;
	Frustum frustum;
	std::string currentDir;

	Canvas root;

	// Error
	enum class GTIError
	{
		NONE = 0,
		IL_LOAD_IMG,
		IL_CONVERT
	} lastError;



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