#ifndef GTInterface_H
#define GTInterface_H

#include "GTITimer.h"

#include "../SDL/include/SDL.h"
#include "../MathGeoLib/MathGeoLib.h"
#include "SDL_ttf/include/SDL_ttf.h"

#include <map>
#include <vector>
#include <list>
#include <string>
#include <functional>
#include <set>

typedef unsigned int uint;

template <typename in>
class FunctionEmitter
{
public:
	FunctionEmitter() {};

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

template <typename in>
class FunctionStorage
{
public:
	FunctionStorage() {};

	// Add to Map
	void RegisterFunction(std::string givenName, std::function<void(in)> func)
	{
		FunctionsMap.insert(std::pair<std::string, std::function<void(in)>>(givenName, func));
	}

	template <typename C>
	void AddFunction(std::string name, C* object, void(C::* memFunc)(in))
	{
		RegisterFunction(name, [object = object, memFunc = memFunc](in arg) { (object->*(memFunc))(arg); });
	}
	
	std::map<std::string, std::function<void(in)>> FunctionsMap;
};

class GTI
{
public:
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
		NONE = 0,
		ALPHA_TEST = 1,
		BLEND = 2
	};
private:

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
		//void ResizeSons(...);

		// RectTransform
		bool Contains(int x, int y) const;
		virtual float3 GetGlobalPosition() const;
		virtual float3 GetGlobalScale() const;
		virtual Quat GetGlobalRotation() const;
		virtual float4x4 GetGlobalTransform() const;

		// Fade
		void StartFade(float msDuration);
		void UpdateFade(float dt);

		// Active
		bool IsActive() const;
		void SetActive(bool set);

		// Save
		void Load();
		void Save();

		FunctionEmitter<bool> boolEmitter;
		FunctionEmitter<float> floatEmitter;
		FunctionEmitter<std::string> stringEmitter;

		float valueFloat = 0.0f;
		bool valueBool = false;

		float fadeAlpha = 1.0f;

	private:
		int MinX() const;
		int MinY() const;
		int MaxX() const;
		int MaxY() const;

		bool active = true;
		bool activeSaved;

		float fadeAlphaSaved;

		float3 positionLocalSaved;
		float3 scaleLocalSaved;

	public:
		UIElement* parent;
		std::vector<UIElement*> sons;
		RectTransform* transform;

		uint buffTexture;
		TransparencyType blendType;
		float alpha;
		uint blend;

		bool draggable;
		bool preserveAspect;

	private:
		UIElementType type;

		bool mouseHover;

		float fadeDuration;
		float fadeSubstracted;
		
		float fadeDurationSaved;
	};

	class Canvas : public UIElement
	{
	public:
		Canvas();
		void HandleEvent(SDL_Event &e);
		float3 GetGlobalPosition() const;
		float3 GetGlobalScale() const;
		Quat GetGlobalRotation() const;
	public:
		Frustum* frustum;
		bool interactable;
		UIElement* focus;
		float minZ;
	};

	class Image : public UIElement
	{
	public:
		Image(UIElement* _parent = nullptr, const char* path = nullptr);
		void SetImage(const char* path = nullptr);
		
		void OnClick();
	public:
		std::string source;
	};

	class Label : public UIElement
	{
	public:
		Label(UIElement* parent = nullptr, const char* text = nullptr, const char* font = nullptr,  uint size = 14, SDL_Color color = { 255, 255, 255 });
		void SetText(const char* t = nullptr);
		bool SetFont(const char* f, uint size);
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
		void SetImage(char* path = nullptr);

		void OnClick();
	};

	class Checkbox : public UIElement
	{
	public:
		Checkbox(bool state, UIElement* _parent = nullptr);
		void OnClick();

	public:
		bool value = false;
	};

	class Input : public UIElement
	{
	public:
		Input(UIElement* _parent = nullptr);
		void Write(char* key);
		void SetText(const char* t = nullptr);
		bool SetFont(const char* f, uint size);
	public:
		std::string text;
		std::string font;
		uint size;
		SDL_Color color;
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

	static void Render(float dt);
	static void RenderUIElement(UIElement* element, bool paintBlend = false, float dt = 0.0f);

	static uint LoadTexture(const char* path, RectTransform* transform);
	static uint GenerateText(const char* text = nullptr, const char* font = nullptr, uint size = 24, SDL_Color color = { 255, 255, 255 }, RectTransform* transform = nullptr);
	static void UpdateText(uint texBuffer, const char* text, const char* fontName, uint size, SDL_Color color, RectTransform * transform = nullptr);
	static const char* LoadFont(const char* path, uint size);

	static UIElement* GetRoot();
	static float GetScale();
	static UIElement* GetFocus();
	void SetFocus(UIElement* focus, float gPosZ);

	Image* CreateImage(UIElement* parent = nullptr, const char* path = nullptr);
	Label* CreateLabel(UIElement* parent = nullptr, const char* text = nullptr, const char* font = nullptr, uint size = 24, SDL_Color color = { 255, 255, 255 });
	Button* CreateButton(UIElement* parent = nullptr);
	Checkbox* CreateCheckbox(bool &ref, UIElement* parent = nullptr);
	Input* CreateInput(UIElement* parent = nullptr);

	FunctionStorage<bool> boolFunctions;
	FunctionStorage<float> floatFunctions;
	FunctionStorage<std::string> stringFunctions;

	std::string GetLastError() const;

	static void GetEventSDL(SDL_Event &e);
	static void UpdateWindowSize(int w, int h);
	bool GetMLBDown() const;

	float4x4 GetCameraTransform() const;
		
	static void Load();
	static void Save();

	GTITimer timer;

private:
	static std::vector<std::string> Splitpath(const std::string& str, const std::set<char> delimiters);
	static TTF_Font* FindFont(const char* font, uint size);

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