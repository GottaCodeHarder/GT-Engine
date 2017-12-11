#include "Component.h"
#include "GTInterface\GTInterface.h"
class GameObject;

class cUI : public Component
{
public:
	cUI(GameObject* _gameObject) : Component(UI, _gameObject), transformChanged(true){}
	virtual ~cUI(){}

	virtual void Save(JSON_Object &object) const {};
	virtual void Load(const JSON_Object &object) {};
	virtual uint Serialize(char* buffer) { return 0; };

public:

	bool transformChanged;
};
class cCanvas : public cUI
{
public:
	cCanvas(GameObject* _gameObject) : cUI(_gameObject) {}
	~cCanvas() {};

	bool interactable = true;
};

class cImage : public cUI
{
public:
	cImage(GameObject* _gameObject) : cUI(_gameObject)
	{
		image = GTI::GTInterface.AddImage();

		// Load Default Texture & set Transform's width and height
		_gameObject->SetRectTransform(1, 1);
	}
	~cImage()
	{
		delete image;
	}

	void RealUpdate()
	{
		if (transformChanged)
		{
			// image->UpdatePos(...); UPDATE PRINTING POSITION

			transformChanged = false;
		}
	}

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

	void RealUpdate()
	{
		if (transformChanged)
		{
			// button->UpdatePos(...); UPDATE PRINTING POSITION

			transformChanged = false;
		}
	}

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

	void RealUpdate()
	{
		if (transformChanged)
		{
			// checkbox->UpdatePos(...); UPDATE PRINTING POSITION

			transformChanged = false;
		}
	}

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

	void RealUpdate()
	{
		if (transformChanged)
		{
			// input->UpdatePos(...); UPDATE PRINTING POSITION

			transformChanged = false;
		}
	}

	void DrawUI()
	{
		//Source
	}

public:
	GTI::Input* input;
};




