#ifndef GTInterface_H
#define GTInterface_H

#include "GTITimer.h"

#include "../SDL/include/SDL.h"
#include "../MathGeoLib/MathGeoLib.h"
#include "SDL_ttf/include/SDL_ttf.h"

#include <map>
#include <list>
#include <string>
#include <functional>
#include <set>

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
		std::list<std::function<void(in)>> funcList;
	};

#pragma endregion

#pragma region UI ELEMENTS

public:

	struct RectTransform
	{
		RectTransform();
		void Reset();
		
		float3 positionLocal;
		float3 scaleLocal;
		Quat rotationLocal;

		uint w;
		uint h;
		float2 anchorMin;
		float2 anchorMax;
		float2 pivot;
	};

	class UIElement
	{
	public:
		UIElement(UIElementType t, UIElement* _parent = nullptr);
		~UIElement();

		UIElementType GetType() const;
		void AddSon(UIElement* son);

		// Input
		virtual void OnClick();
		void CheckMouseClick(SDL_MouseButtonEvent &e);
		void CheckMouseMove(SDL_MouseMotionEvent &e);
		void MouseHoverLeave();
		//void ResizeSons(...);

		// RectTransform
		bool Contains(int x, int y) const;
		float3 GetGlobalPosition() const;
		float3 GetGlobalScale() const;
		Quat GetGlobalRotation() const;
		float4x4 GetGlobalTransform() const;

		// Fade
		void StartFade(float msDuration);
		void UpdateFade();

	private:
		int MinX() const;
		int MinY() const;
		int MaxX() const;
		int MaxY() const;


	public:
		UIElement* parent;
		std::vector<UIElement*> sons;
		RectTransform* transform;

		uint buffTexture;
		TransparencyType blendsType;
		float alpha;

		bool draggable;
		bool mouseHover;
		bool preserveAspect;

		Emitter<bool> emitter;

	private:
		UIElementType type;

		float fadeDuration = 0; // With the End in ms of the Fade
		float fadeStart = 0;
		float fadeAlpha = 0;
	};

	class Canvas : public UIElement
	{
	public:
		Canvas();
		void HandleEvent(SDL_Event &e);
	public:
		Frustum* frustum;
		bool interactable;
		UIElement* focus;
		float minZ;
	};

	class Image : public UIElement
	{
	public:
		Image(UIElement* _parent = nullptr, char* path = nullptr);
		void SetImage(char* path = nullptr);
		void OnClick();
	public:
		std::string source;
	};

	class Label : public UIElement
	{
	public:
		Label(std::string text, std::string font, uint size = 14, SDL_Color color = { 255, 255, 255 }, UIElement* parent = nullptr);
		void SetText(const char* t = nullptr);
		bool SetFont(std::string font, uint size);
		void OnClick();
	public:
		std::string text;
		std::string font;
		uint size;
		SDL_Color color;
	};

	class Button : public UIElement
	{
	public:
		Button(UIElement* _parent = nullptr);
		// NEED A WAY TO CHANGE THE IMAGE BUFF
		// std::vector<de 0 a 3> donde 0 es normal, 1 hover, 2 click
	};

	class Checkbox : public UIElement
	{
	public:
		Checkbox(bool* ref, UIElement* _parent = nullptr);
		bool* reference;
	};

	class Input : public UIElement
	{
	public:
		Input(UIElement* _parent = nullptr);
		void Write(char* key);
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
	static void CleanUp();

	static void Render();
	static void RenderUIElement(UIElement* element, bool paintBlend = false);

	static uint LoadTexture(const char* path, RectTransform* transform);
	static uint GenerateText(std::string text, std::string font, uint size, SDL_Color color, RectTransform* transform = nullptr);
	static void UpdateText(uint texBuffer, std::string text, std::string fontName, uint size, SDL_Color color, RectTransform * transform = nullptr);
	static std::string LoadFont(const char* path, uint size);

	static UIElement* GetRoot();
	static UIElement* GetFocus();
	void SetFocus(UIElement* focus, float gPosZ);

	Image* CreateImage(UIElement* parent = nullptr, char* path = nullptr);
	Label* CreateLabel(std::string text, std::string font, uint size = 14, SDL_Color color = { 255, 255, 255 }, UIElement* parent = nullptr);
	Button* CreateButton(UIElement* parent = nullptr);
	Checkbox* CreateCheckbox(bool* ref = nullptr, UIElement* parent = nullptr);
	Input* CreateInput(UIElement* parent = nullptr);

	std::string GetLastError() const;

	static void GetEventSDL(SDL_Event &e);
	static void UpdateWindowSize(int w, int h);
	bool GetMLBDown() const;

	float4x4 GetCameraTransform() const;

	std::map<std::string, std::function<void(bool)>> boolFunctionsMap;
	std::map<std::string, std::function<void(char)>> charFunctionsMap;
	std::map<std::string, std::function<void(int)>> intFunctionsMap;

	void Register(std::string givenName, std::function<void(bool)> func)
	{
		boolFunctionsMap.insert(std::pair<std::string, std::function<void(bool)>>(givenName, func));
	};

	void Register(std::string givenName, std::function<void(char)> func)
	{
		charFunctionsMap.insert(std::pair<std::string, std::function<void(char)>>(givenName, func));
	};

	void Register(std::string givenName, std::function<void(int)> func)
	{
		intFunctionsMap.insert(std::pair<std::string, std::function<void(int)>>(givenName, func));
	};

	void Func1(int value)
	{
		std::cout << "Func 1 pinging a " << value << std::endl;
	}

	GTITimer timer;

private:
	static std::vector<std::string> Splitpath(const std::string& str, const std::set<char> delimiters);
	static TTF_Font* FindFont(std::string fontName, uint size);

private:
	std::vector<UIElement*> UIElements;
	std::multimap<float, UIElement*> blendElements;
	std::multimap<std::string, std::pair< uint, TTF_Font*>> availableFonts;

	float scale;
	Frustum frustum;
	bool mouseLBDown;
	Canvas root;

	// Error
	enum class GTIError
	{
		NONE = 0,
		IL_LOAD_IMG,
		IL_CONVERT
	} lastError;

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