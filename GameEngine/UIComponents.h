#include "Component.h"
#include "Random.h"
#include "GameObject.h"

#include "GTInterface\GTInterface.h"

class GameObject;

class cUI : public Component
{
public:
	cUI(GameObject* _gameObject) : Component(UI, _gameObject) { }
	virtual ~cUI(){}

	virtual void Save(JSON_Object &object) const {};
	virtual void Load(const JSON_Object &object) {};

	std::vector<std::string> GetFunctionsName();
	
	void SetFunctions(std::string target, GTI::UIElement* element);

	GTI::RectTransform* getTransform() const;

	virtual GTI::UIElement* GetUI() const { return nullptr; }
};

class cCanvas : public cUI
{
public:
	cCanvas(GameObject* _gameObject) : cUI(_gameObject)
	{
		canvas = (GTI::Canvas*)GTI::GTInterface.GetRoot();
		
		_gameObject->AddComponent(this);
		//if (!canvas) LOG(GTI::GetLastError().c_str());
	}
	~cCanvas() {};

	GTI::UIElement* GetUI() const { return canvas; }

private:
	GTI::Canvas* canvas;
};

class cImage : public cUI
{
public:
	int unique;

	cImage(GameObject* _gameObject, char* path = nullptr, GTI::UIElement* parent = nullptr) : cUI(_gameObject)
	{
		image = GTI::GTInterface.CreateImage(parent, path);

		_gameObject->AddComponent(this);
		// Load Default Texture & set Transform's width and height
		//_gameObject->SetRectTransform(1, 1);
	}
	~cImage()
	{
		delete image;
	}

	GTI::Image* GetUI() const { return image; }

	void DrawUI();

private:
	GTI::Image* image;
};


class cLabel: public cUI
{
public:
	cLabel(GameObject* _gameObject, const char* _text, const char* _font, uint size = 14, SDL_Color color = { 255, 255, 255 }, GTI::UIElement* parent = nullptr) : cUI(_gameObject)
	{
		std::string text = (_text == nullptr) ? "" : _text;
		std::string font = (_font == nullptr) ? "" : _font;
		label = GTI::GTInterface.CreateLabel(text, font, size, color, parent);

		_gameObject->AddComponent(this);
	}

	GTI::UIElement* GetUI() const { return label; }

	void DrawUI()
	{
		//Source
		//Color RGB
		//Alpha
	}

private:
	GTI::Label* label;
};


class cButton : public cUI
{
public:
	cButton(GameObject* _gameObject, GTI::UIElement* parent = nullptr) : cUI(_gameObject)
	{
		button = GTI::GTInterface.CreateButton(parent);
		_gameObject->AddComponent(this);
	}
	~cButton()
	{
		delete button;
	}
	GTI::UIElement* GetUI() const { return button; }
	GTI::Image* GetButtonImages() const { return imageA; }
	
	void SetButtonImage(GTI::Image* image) { imageA = image; }

	void DrawUI();

private:
	GTI::Button* button;

	GTI::Image* imageA;
};

class cCheckbox : public cUI
{
public:
	cCheckbox(GameObject* _gameObject, GTI::UIElement* parent = nullptr) : cUI(_gameObject) { _gameObject->AddComponent(this);  }
	~cCheckbox()
	{
		delete checkbox;
	}
	GTI::UIElement* GetUI() const { return checkbox; }

	void DrawUI();

private:
	GTI::Checkbox* checkbox;
};

class cInput : public cUI
{
public:
	cInput(GameObject* _gameObject) : cUI(_gameObject)
	{
		_gameObject->AddComponent(this);
		//GetUI()->stringEmitter.Register<GTI::Label>(GetUI(), &GTI::Label::SetText);
	}
	~cInput()
	{
		delete input;
	}
	GTI::UIElement* GetUI() const { return input; }

	void DrawUI()
	{

	}

private:
	GTI::Input* input;
};

