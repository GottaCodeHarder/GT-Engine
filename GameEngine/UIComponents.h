#include "Component.h"
#include "GTInterface\GTInterface.h"
class GameObject;

class cUI : public Component
{
public:
	cUI(GameObject* _gameObject) : Component(UI, _gameObject) {}
	virtual ~cUI(){}

	virtual void Save(JSON_Object &object) const {};
	virtual void Load(const JSON_Object &object) {};
	virtual uint Serialize(char* buffer) { return 0; };

	GTI::RectTransform* getTransform() const
	{
		return GetUI() != nullptr ? GetUI()->transform : nullptr;
	}
	virtual GTI::UIElement* GetUI() const { return nullptr; }
};

class cCanvas : public cUI
{
public:
	cCanvas(GameObject* _gameObject) : cUI(_gameObject)
	{
		canvas = (GTI::Canvas*)GTI::GTInterface.GetRoot();
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
	cImage(GameObject* _gameObject, char* path = nullptr, GTI::UIElement* parent = nullptr) : cUI(_gameObject)
	{
		image = GTI::GTInterface.CreateImage(parent, path);

		// Load Default Texture & set Transform's width and height
		//_gameObject->SetRectTransform(1, 1);
	}
	~cImage()
	{
		delete image;
	}

	GTI::UIElement* GetUI() const { return image; }

	void DrawUI()
	{
		//Source
		//Color RGB
		//Alpha
	}

private:
	GTI::Image* image;
};

class cButton : public cUI
{
public:
	cButton(GameObject* _gameObject, GTI::UIElement* parent = nullptr) : cUI(_gameObject) {}
	~cButton()
	{
		delete button;
	}
	GTI::UIElement* GetUI() const { return button; }

	void DrawUI()
	{
		//Source
	}

private:
	GTI::Button* button;
};

class cCheckbox : public cUI
{
public:
	cCheckbox(GameObject* _gameObject, GTI::UIElement* parent = nullptr) : cUI(_gameObject) {}
	~cCheckbox()
	{
		delete checkbox;
	}
	GTI::UIElement* GetUI() const { return checkbox; }

	void DrawUI()
	{
		//Source
	}

private:
	GTI::Checkbox* checkbox;
};

class cInput : public cUI
{
public:
	cInput(GameObject* _gameObject) : cUI(_gameObject) {}
	~cInput()
	{
		delete input;
	}
	GTI::UIElement* GetUI() const { return input; }

	void DrawUI()
	{
		//Source
	}

private:
	GTI::Input* input;
};

