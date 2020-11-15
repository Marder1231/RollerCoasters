#pragma once

#include <memory.h>
#include <vector>
#include <map>
#include <Windows.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GL/glu.h"
#include "Utilities/Pnt3f.H"

namespace NPGameObject
{
	/// <summary>
	/// this class draw the cube
	/// side of color is white
	/// </summary>
	class CubePrinter
	{
		int sideColor[6][3];
	public:

		CubePrinter()
		{
			//memset(sideColor, 255, sizeof(sideColor[0][0]) * 6 * 3);
			int initialColor = 255;
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					sideColor[i][j] = initialColor;
				}
			}
		}
		enum EmSideIndex
		{
			Top = 0,
			Front,
			Right,
			Left,
			Back,
			Bottom,
		};
		enum class EmRGB
		{
			Red = 0,
			Green = 1,
			Blue = 2,
			R = 0,
			G = 1,
			B = 2,
		};
		void SetSideColor(int _where, int _color[3])
		{
			sideColor[_where][0] = _color[0];
			sideColor[_where][1] = _color[1];
			sideColor[_where][2] = _color[2];
		}

		void SideDrawer(int _where, Pnt3f _size, bool doingShadows = false)
		{
			if (!doingShadows)
				glColor3ub(sideColor[_where][0], sideColor[_where][1], sideColor[_where][2]);

			float width = _size.x;
			float height = _size.y;
			float length = _size.z;

			switch (_where)
			{
			case EmSideIndex::Top:
				glVertex3f(width / 2, height / 2, length / 2);
				glVertex3f(width / 2, height / 2, -length / 2);
				glVertex3f(-width / 2, height / 2, -length / 2);
				glVertex3f(-width / 2, height / 2, length / 2);
				break;
			case EmSideIndex::Back:
				glVertex3f(width / 2, height / 2, -length / 2);
				glVertex3f(-width / 2, height / 2, -length / 2);
				glVertex3f(-width / 2, -height / 2, -length / 2);
				glVertex3f(width / 2, -height / 2, -length / 2);
				break;
			case EmSideIndex::Front:
				glVertex3f(width / 2, height / 2, length / 2);
				glVertex3f(-width / 2, height / 2, length / 2);
				glVertex3f(-width / 2, -height / 2, length / 2);
				glVertex3f(width / 2, -height / 2, length / 2);
				break;
			case EmSideIndex::Bottom:
				glVertex3f(width / 2, -height / 2, length / 2);
				glVertex3f(width / 2, -height / 2, -length / 2);
				glVertex3f(-width / 2, -height / 2, -length / 2);
				glVertex3f(-width / 2, -height / 2, length / 2);
				break;
			case EmSideIndex::Right:
				glVertex3f(width / 2, height / 2, -length / 2);
				glVertex3f(width / 2, height / 2, length / 2);
				glVertex3f(width / 2, -height / 2, length / 2);
				glVertex3f(width / 2, -height / 2, -length / 2);
				break;
			case EmSideIndex::Left:
				glVertex3f(-width / 2, height / 2, -length / 2);
				glVertex3f(-width / 2, height / 2, length / 2);
				glVertex3f(-width / 2, -height / 2, length / 2);
				glVertex3f(-width / 2, -height / 2, -length / 2);
				break;
			default:
				break;
			}
		}
	};

	class BasicObject
	{
	public:
		BasicObject* Parent = nullptr;

		Pnt3f Position;
		Pnt3f LocalPosition;
		Pnt3f Orient;
		Pnt3f ViewDir;

		Pnt3f Size;
		CubePrinter printer;

		BasicObject()
		{
			Pnt3f pos(0, 0, 0);
			Pnt3f size(1, 1, 1);
			Init(pos, size);
		}
		BasicObject(Pnt3f _pos, Pnt3f _size)
		{
			Init(_pos, _size);
		}

		inline void Init(Pnt3f _pos, Pnt3f _size)
		{
			Position = _pos;
			Size = _size;
			Orient = Pnt3f(0, 1, 0);
			ViewDir = Pnt3f(1, 0, 0);
			LocalPosition = Pnt3f(0, 0, 0);
		}

		void CenterDraw(bool doingShadows)
		{
			Pnt3f u = ViewDir; u.normalize();
			Pnt3f w = u * Orient; w.normalize();
			Pnt3f v = w * u; v.normalize();
			float rotation[16] =
			{
				u.x, u.y, u.z, 0.0f,
				v.x, v.y, v.z, 0.0f,
				w.x, w.y, w.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};

			glPushMatrix();
			glTranslatef(Position.x, Position.y, Position.z);
			glMultMatrixf(rotation);
			glRotatef(90, 0, 1, 0);
			glBegin(GL_QUADS);

			printer.SideDrawer(CubePrinter::EmSideIndex::Top, Size, doingShadows);
			printer.SideDrawer(CubePrinter::EmSideIndex::Bottom, Size, doingShadows);
			printer.SideDrawer(CubePrinter::EmSideIndex::Front, Size, doingShadows);
			printer.SideDrawer(CubePrinter::EmSideIndex::Back, Size, doingShadows);
			printer.SideDrawer(CubePrinter::EmSideIndex::Right, Size, doingShadows);
			printer.SideDrawer(CubePrinter::EmSideIndex::Left, Size, doingShadows);

			glEnd();
			glPopMatrix();
		}
	};

	//component interface
	class IComponent
	{
	public:
		/// <summary>
		/// set localPosition, have to set parent first
		/// modify := where is this object from position of parent object 
		/// </summary>
		/// <param name=""></param>
		virtual void Modify(Pnt3f) = 0;
		virtual void SetParent(BasicObject*) = 0;
	};

	/// <summary>
	/// component center pos need at joint
	/// </summary>
	class Component : public BasicObject, public IComponent
	{
		void initialSize(Pnt3f _size)
		{
			Size = _size;
		}

		int index = -1;
	public:
		Component() : NPGameObject::BasicObject() { }
		Component(Pnt3f _pos, Pnt3f _size) : NPGameObject::BasicObject(_pos, _size) { }
		Component(Pnt3f _pos, Pnt3f _size, BasicObject* parent, Pnt3f modifyPos, int _index)
		{
			Init(_pos, parent, modifyPos);
			index = _index;
		}

		virtual void Modify(Pnt3f modifyPos) override
		{
			Pnt3f parentPos = Position;
			Position = Position + modifyPos;
			LocalPosition = Position - parentPos;
		}

		virtual void SetParent(BasicObject* _parent) override
		{
			Parent = _parent;
		}

		//void change head size or didn't use preset head size
		void Init(Pnt3f _size, BasicObject* _parent, Pnt3f modifyPos)
		{
			NPGameObject::BasicObject::Init(_parent->Position, _size);
			SetParent(_parent);
			Modify(modifyPos);
		}
	};

	class GameObject : public BasicObject
	{
	public:
		std::map<int, Component*> Components;

		GameObject() : BasicObject() { }
		GameObject(Pnt3f _pos, Pnt3f _size) : BasicObject(_pos, _size) { }

		void AddComponent(Pnt3f _size, Pnt3f _modifyPos, int _index)
		{
			Component* newComponent = new Component(_size, this->Position, this, _modifyPos, _index);
			Components[_index] = newComponent;
		}

		~GameObject()
		{
			for (int i = 0; i < Components.size(); i++)
			{
				delete Components[i];
			}
			Components.clear();
		}
	};
}

using namespace NPGameObject;
class TestGuy : public GameObject
{
public:

	TestGuy() : GameObject(){ }

	TestGuy(Pnt3f _pos, Pnt3f _size) : GameObject(_pos, _size){ }

	struct EmBodyPartial
	{
		enum _
		{
			Head = 0,
			Body = 1,
			RightHand = 2,
			LeftHand = 3,
			RightLeg = 4,
			LeftLeg = 5,
		};
	};

	void Init(Pnt3f _pos)
	{
		Pnt3f _size(1, 1, 1);
		NPGameObject::BasicObject::Init(_pos, _size);

		Pnt3f bodySize(3, 5, 3);
		Pnt3f bodyModifyPos(0, 0, 0);
		AddComponent(bodySize, bodyModifyPos, EmBodyPartial::Body);

		Pnt3f headSize(1.5f, 1.5f, 1.5f);
		Pnt3f headModifyPos(0, Size.y / 2, 0);
		AddComponent(headSize, headModifyPos, EmBodyPartial::Head);

		Pnt3f handSize(1, 5, 1);
		Pnt3f rightHandModifyPos(Components[EmBodyPartial::Body]->Size.x / 2.0, Components[EmBodyPartial::Body]->Size.y / 2, 0);
		AddComponent(handSize, rightHandModifyPos, EmBodyPartial::RightHand);
		Components[EmBodyPartial::RightHand]->Orient.y = -1;
	}

	void Draw(bool doingShadows)
	{
		for (auto& partial : Components)
		{
			partial.second->CenterDraw(doingShadows);
		}
	}
};