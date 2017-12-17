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

	cImage(GameObject* _gameObject, const char* path = nullptr, GTI::UIElement* parent = nullptr) : cUI(_gameObject)
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
	cLabel(GameObject* _gameObject, const char* text, GTI::UIElement* parent = nullptr) : cUI(_gameObject)
	{
		label = GTI::GTInterface.CreateLabel(parent, text);
		_gameObject->AddComponent(this);
	}

	GTI::UIElement* GetUI() const { return label; }
	void SetText(const char* t = nullptr) { label->SetText(t); }
	bool SetFont(const char* font, uint size) { return label->SetFont(font, size); }

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

	void DrawUI();

private:
	GTI::Button* button;
};

class cCheckbox : public cUI
{
public:
	cCheckbox(GameObject* _gameObject, bool state, GTI::UIElement* parent = nullptr) : cUI(_gameObject)
	{
		checkbox = GTI::GTInterface.CreateCheckbox(state, parent);
		_gameObject->AddComponent(this);
	}
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

	void SetText(const char* t = nullptr) { input->SetText(t); }
	bool SetFont(const char* font, uint size) { input->SetFont(font, size); }

	void DrawUI()
	{

	}

private:
	GTI::Input* input;
};

