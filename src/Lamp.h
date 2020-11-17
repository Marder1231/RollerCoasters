#pragma once

#include "TestGuy.h"

class Lamp : public NPGameObject::GameObject
{
public:
	Lamp() : GameObject() { }
	Lamp(Pnt3f _pos, Pnt3f _size) : GameObject(_pos, _size) { }

	struct EmBodyPartial
	{
		enum _
		{
			Body = 0,
			Head = 1,
		};
	};

	class LampShade : public Component
	{
	public:
		LampShade(Pnt3f _size, BasicObject* parent, Pnt3f modifyPos, int _index) : Component(_size, parent, modifyPos, _index) { }

		void SwitchLight(bool open = true)
		{

			//float noAmbient[] = { 0, 0, 0.01f, 1.0f };
			//float diffuse[] = { 0, 0.2, 0.2, 1 };
			//float position[] = { GetPosition().x, GetPosition().y, GetPosition().z, 1 };

			//glLightfv(GL_LIGHT2, GL_AMBIENT, noAmbient);
			//glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
			//glLightfv(GL_LIGHT2, GL_POSITION, position);

			//float direction[] = { GetOrient().x, -GetOrient().y, GetOrient().z };

			//glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direction);
			//glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, GL_SPOT_CUTOFF);
			//glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 5);

			//glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1);
			//glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0);
			//glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0);

			//if (open == true)
			//	glEnable(GL_LIGHT2);
			//else
			//	glDisable(GL_LIGHT2);
		}
	};

	void Init(Pnt3f _pos)
	{
		Pnt3f _size(1, 1, 1);
		NPGameObject::BasicObject::Init(_pos, _size);

		Pnt3f bodySize(1, 10, 1);
		Pnt3f bodyModifyPos(0, 0, 0);
		AddComponent(bodySize, bodyModifyPos, EmBodyPartial::Body);
		Components[EmBodyPartial::Body]->printer.DrawMethod = CubePrinter::EmDrawMethod::Top;

		Pnt3f headSize(2, 1, 4);
		Pnt3f headModifyPos(0, Components[EmBodyPartial::Body]->Size.y, 0);
		LampShade* lampShade = new LampShade(headSize, this, headModifyPos, EmBodyPartial::Head);
		AddComponent(lampShade, EmBodyPartial::Head);
		Components[EmBodyPartial::Head]->printer.DrawMethod = CubePrinter::EmDrawMethod::Side;
		Components[EmBodyPartial::Head]->SetOrient(Pnt3f(1, 0, 0));
	}

	void SwitchLight(bool open = true)
	{
		LampShade* lampShade = dynamic_cast<LampShade*>(Components[EmBodyPartial::Head]);
		lampShade->SwitchLight(open);
	}

	
};