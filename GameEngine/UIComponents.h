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

	virtual GTI::RectTransform* getTransform() const { return nullptr; }
};

class cCanvas : public cUI
{
public:
	cCanvas(GameObject* _gameObject) : cUI(_gameObject)
	{
		canvas = new GTI::Canvas();
	}
	~cCanvas() {};

	GTI::RectTransform* getTransform() const { return canvas->transform; }

public:
	GTI::Canvas* canvas;
};

class cImage : public cUI
{
public:
	cImage(GameObject* _gameObject) : cUI(_gameObject)
	{
		image = GTI::GTInterface.AddImage();

		// Load Default Texture & set Transform's width and height
		//_gameObject->SetRectTransform(1, 1);
	}
	~cImage()
	{
		delete image;
	}

	GTI::RectTransform* getTransform() const { return image->transform; }

	void DrawUI()
	{
		//Source
		//Color RGB
		//Alpha
	}

public:
	GTI::Image* image;
};

class cButton : public cUI
{
public:
	cButton(GameObject* _gameObject) : cUI(_gameObject) {}
	~cButton()
	{
		delete button;
	}

	GTI::RectTransform* getTransform() const { return button->transform; }

	void DrawUI()
	{
		//Source
	}

public:
	GTI::Button* button;
};

class cCheckbox : public cUI
{
public:
	cCheckbox(GameObject* _gameObject) : cUI(_gameObject) {}
	~cCheckbox()
	{
		delete checkbox;
	}

	GTI::RectTransform* getTransform() const { return checkbox->transform; }

	void DrawUI()
	{
		//Source
	}

public:
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

	GTI::RectTransform* getTransform() const { return input->transform; }

	void DrawUI()
	{
		//Source
	}

public:
	GTI::Input* input;
};

