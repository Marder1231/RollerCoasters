#pragma once

#include <memory.h>
#include <vector>
#include <map>
#include <Windows.h>

const double PI = 3.141592;

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GL/glu.h"
#include "Utilities/Pnt3f.H"
#include "Utilities/3DUtils.h"

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
		struct EmSideIndex
		{
			enum _
			{
				Top = 0,
				Front,
				Right,
				Left,
				Back,
				Bottom,
			};
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

		//the center pos at where
		enum class EmDrawMethod
		{
			Center = 0,
			Top = 1,
			Side = 2,
		}DrawMethod = EmDrawMethod::Center;

		void SetSideColor(int _where, int _color[3])
		{
			sideColor[_where][0] = _color[0];
			sideColor[_where][1] = _color[1];
			sideColor[_where][2] = _color[2];
		}

		void SideDrawer(int _where, Pnt3f _size, bool doingShadows = false, EmDrawMethod method = EmDrawMethod::Center)
		{
			if (!doingShadows)
				glColor3ub(sideColor[_where][0], sideColor[_where][1], sideColor[_where][2]);

			float width = _size.x;
			float height = _size.y;
			float length = _size.z;

			if (method == EmDrawMethod::Center)
			{
				switch (_where)
				{
				case EmSideIndex::EmSideIndex::Top:
					glVertex3f(width / 2, height / 2, length / 2);
					glVertex3f(width / 2, height / 2, -length / 2);
					glVertex3f(-width / 2, height / 2, -length / 2);
					glVertex3f(-width / 2, height / 2, length / 2);
					break;
				case EmSideIndex::EmSideIndex::Back:
					glVertex3f(width / 2, height / 2, -length / 2);
					glVertex3f(-width / 2, height / 2, -length / 2);
					glVertex3f(-width / 2, -height / 2, -length / 2);
					glVertex3f(width / 2, -height / 2, -length / 2);
					break;
				case EmSideIndex::EmSideIndex::Front:
					glVertex3f(width / 2, height / 2, length / 2);
					glVertex3f(-width / 2, height / 2, length / 2);
					glVertex3f(-width / 2, -height / 2, length / 2);
					glVertex3f(width / 2, -height / 2, length / 2);
					break;
				case EmSideIndex::EmSideIndex::Bottom:
					glVertex3f(width / 2, -height / 2, length / 2);
					glVertex3f(width / 2, -height / 2, -length / 2);
					glVertex3f(-width / 2, -height / 2, -length / 2);
					glVertex3f(-width / 2, -height / 2, length / 2);
					break;
				case EmSideIndex::EmSideIndex::Right:
					glVertex3f(width / 2, height / 2, -length / 2);
					glVertex3f(width / 2, height / 2, length / 2);
					glVertex3f(width / 2, -height / 2, length / 2);
					glVertex3f(width / 2, -height / 2, -length / 2);
					break;
				case EmSideIndex::EmSideIndex::Left:
					glVertex3f(-width / 2, height / 2, -length / 2);
					glVertex3f(-width / 2, height / 2, length / 2);
					glVertex3f(-width / 2, -height / 2, length / 2);
					glVertex3f(-width / 2, -height / 2, -length / 2);
					break;
				default:
					break;
				}
			}
			else if (method == EmDrawMethod::Top)
			{
				switch (_where)
				{
				case EmSideIndex::Top:
					glVertex3f(width / 2, height, length / 2);
					glVertex3f(width / 2, height, -length / 2);
					glVertex3f(-width / 2, height, -length / 2);
					glVertex3f(-width / 2, height, length / 2);
					break;
				case EmSideIndex::Back:
					glVertex3f(width / 2, height, -length / 2);
					glVertex3f(-width / 2, height, -length / 2);
					glVertex3f(-width / 2,0, -length / 2);
					glVertex3f(width / 2, 0, -length / 2);
					break;
				case EmSideIndex::Front:
					glVertex3f(width / 2,  height, length / 2);
					glVertex3f(-width / 2, height, length / 2);
					glVertex3f(-width / 2, 0, length / 2);
					glVertex3f(width / 2,  0, length / 2);
					break;
				case EmSideIndex::Bottom:
					glVertex3f(width / 2,  0, length / 2);
					glVertex3f(width / 2,  0, -length / 2);
					glVertex3f(-width / 2, 0, -length / 2);
					glVertex3f(-width / 2, 0, length / 2);
					break;
				case EmSideIndex::Right:
					glVertex3f(width / 2, height, -length / 2);
					glVertex3f(width / 2, height, length / 2);
					glVertex3f(width / 2, 0, length / 2);
					glVertex3f(width / 2, 0, -length / 2);
					break;
				case EmSideIndex::Left:
					glVertex3f(-width / 2, height, -length / 2);
					glVertex3f(-width / 2, height, length / 2);
					glVertex3f(-width / 2, 0, length / 2);
					glVertex3f(-width / 2, 0, -length / 2);
					break;
				default:
					break;


				}
			}
			else if (method == EmDrawMethod::Side)
			{
				switch (_where)
				{
				case EmSideIndex::EmSideIndex::Top:
					glVertex3f(width / 2, height / 2, length);
					glVertex3f(width / 2, height / 2, 0);
					glVertex3f(-width / 2, height / 2, 0);
					glVertex3f(-width / 2, height / 2, length);
					break;
				case EmSideIndex::EmSideIndex::Back:
					glVertex3f(width / 2, height / 2,  0);
					glVertex3f(-width / 2, height / 2, 0);
					glVertex3f(-width / 2, -height / 2, 0);
					glVertex3f(width / 2, -height / 2, 0);
					break;
				case EmSideIndex::EmSideIndex::Front:
					glVertex3f(width / 2, height / 2, length);
					glVertex3f(-width / 2, height / 2, length);
					glVertex3f(-width / 2, -height / 2, length);
					glVertex3f(width / 2, -height / 2, length);
					break;
				case EmSideIndex::EmSideIndex::Bottom:
					glVertex3f(width / 2, -height / 2, length);
					glVertex3f(width / 2, -height / 2, 0);
					glVertex3f(-width / 2, -height / 2,0);
					glVertex3f(-width / 2, -height / 2, length);
					break;
				case EmSideIndex::EmSideIndex::Right:
					glVertex3f(width / 2, height / 2, 0);
					glVertex3f(width / 2, height / 2, length);
					glVertex3f(width / 2, -height / 2, length);
					glVertex3f(width / 2, -height / 2, 0);
					break;
				case EmSideIndex::EmSideIndex::Left:
					glVertex3f(-width / 2, height / 2, 0);
					glVertex3f(-width / 2, height / 2,  length);
					glVertex3f(-width / 2, -height / 2, length);
					glVertex3f(-width / 2, -height / 2, 0);
					break;
				default:
					break;
				}
			}
		}
	};


	class BasicObject
	{
		Pnt3f position;
		Pnt3f localPosition;
		Pnt3f orient;
		Pnt3f viewDir;
	public:
		BasicObject* Parent = nullptr;

		virtual void SetPosition(Pnt3f _pos)
		{
			position = _pos;
		}
		Pnt3f& GetPosition() { return position; }

		void SetLocalPosition(Pnt3f _pos)
		{
			localPosition = _pos;
		}
		Pnt3f& GetLocalPosition() { return localPosition; }
		
		virtual void SetOrient(Pnt3f _pos)
		{
			orient = _pos;
		}
		Pnt3f& GetOrient() { return orient; }

		virtual void SetViewDir(Pnt3f _pos)
		{
			viewDir = _pos;
		}
		Pnt3f& GetViewDir() { return viewDir; }

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
			position = _pos;
			Size = _size;
			orient = Pnt3f(0, 1, 0);
			viewDir = Pnt3f(0, 0, 1);
			localPosition = Pnt3f(0, 0, 0);
		}

		void Draw(bool doingShadows)
		{
			Pnt3f u = viewDir; u.normalize();
			Pnt3f w = u * orient; w.normalize();
			Pnt3f v = w * u; v.normalize();
			float rotation[16] =
			{
				u.x, u.y, u.z, 0.0f,
				v.x, v.y, v.z, 0.0f,
				w.x, w.y, w.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};

			glPushMatrix();
			glTranslatef(position.x, position.y, position.z);
			glMultMatrixf(rotation);
			glRotatef(90, 0, 1, 0);
			glBegin(GL_QUADS);

			printer.SideDrawer(CubePrinter::EmSideIndex::Top, Size, doingShadows, printer.DrawMethod);
			printer.SideDrawer(CubePrinter::EmSideIndex::Bottom, Size, doingShadows, printer.DrawMethod);
			printer.SideDrawer(CubePrinter::EmSideIndex::Front, Size, doingShadows, printer.DrawMethod);
			printer.SideDrawer(CubePrinter::EmSideIndex::Back, Size, doingShadows, printer.DrawMethod);
			printer.SideDrawer(CubePrinter::EmSideIndex::Right, Size, doingShadows, printer.DrawMethod);
			printer.SideDrawer(CubePrinter::EmSideIndex::Left, Size, doingShadows, printer.DrawMethod);

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

	public:
		int index = -1;
		Component(Pnt3f _size, BasicObject* parent, Pnt3f modifyPos, int _index)
		{
			Init(_size, parent, modifyPos);
			index = _index;
		}

		virtual void Modify(Pnt3f modifyPos) override
		{
			Pnt3f parentPos = GetPosition();
			SetPosition(GetPosition() + modifyPos);
			SetLocalPosition(GetPosition() - parentPos);
		}

		virtual void SetParent(BasicObject* _parent) override
		{
			Parent = _parent;
		}

		//void change head size or didn't use preset head size
		void Init(Pnt3f _size, BasicObject* _parent, Pnt3f modifyPos)
		{
			NPGameObject::BasicObject::Init(_parent->GetPosition(), _size);
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
			Component* newComponent = new Component(_size, this, _modifyPos, _index);
			Components[_index] = newComponent;
		}
		void AddComponent(Component* _compoenent, int _index)
		{
			Components[_index] = _compoenent;
		}

		virtual void SetOrient(Pnt3f _pos) override
		{
			_pos.normalize();
			BasicObject::SetOrient(_pos);

			//$$$ not change children orient
			for (auto& partial : Components)
			{
				Pnt3f u = this->GetViewDir(); u.normalize();
				Pnt3f w = u * this->GetOrient(); w.normalize();
				Pnt3f v = w * u; v.normalize();
				glm::mat4x4 rotation =
				{
					u.x, u.y, u.z, 0.0f,
					v.x, v.y, v.z, 0.0f,
					w.x, w.y, w.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				glm::vec4 curPos = { partial.second->GetLocalPosition().x,
									 partial.second->GetLocalPosition().y,
									 partial.second->GetLocalPosition().z,
									 1};
				glm::vec4 newPos = rotation * curPos;

				partial.second->SetPosition(Pnt3f(newPos.x + this->GetPosition().x, 
												  newPos.y + this->GetPosition().y, 
												  newPos.z + this->GetPosition().z) );
				partial.second->SetOrient(_pos);
			}
		}

		virtual void SetPosition(Pnt3f _pos) override
		{
			BasicObject::SetPosition(_pos);

			for (auto& partial : Components)
			{
				partial.second->SetPosition(_pos);
				partial.second->Modify(partial.second->GetLocalPosition());
			}
		}

		virtual void SetViewDir(Pnt3f _pos) override
		{
			_pos.normalize();
			BasicObject::SetViewDir(_pos);

			for (auto& partial : Components)
			{
				Pnt3f u = this->GetViewDir(); u.normalize();
				Pnt3f w = u * this->GetOrient(); w.normalize();
				Pnt3f v = w * u; v.normalize();
				glm::mat4x4 rotation =
				{
					u.x, u.y, u.z, 0.0f,
					v.x, v.y, v.z, 0.0f,
					w.x, w.y, w.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				glm::vec4 curPos = { partial.second->GetLocalPosition().x,
									 partial.second->GetLocalPosition().y,
									 partial.second->GetLocalPosition().z,
									 1 };
				glm::vec4 newPos = rotation * curPos;

				partial.second->SetPosition(Pnt3f(newPos.x + this->GetPosition().x,
					newPos.y + this->GetPosition().y,
					newPos.z + this->GetPosition().z));
				partial.second->SetViewDir(_pos);
			}
		}

		void Draws(bool doingShadows)
		{
			for (auto& partial : Components)
			{
				partial.second->Draw(doingShadows);
			}
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

	class RightHand : public Component
	{
	public:
		RightHand(Pnt3f _size, BasicObject* parent, Pnt3f modifyPos, int _index) : Component(_size, parent, modifyPos, _index) { }

		int HandUpCounter = 0;
		bool HandUping = true;
		void HandUp()
		{
			if (HandUping == true)
				HandUpCounter++;
			else
				HandUpCounter--;

			if (HandUpCounter > 9)
				HandUping = false;
			else if (HandUpCounter < 0)
				HandUping = true;

			float radian = 10 * PI / 180.0;
			if (HandUping == false)
				radian = -radian;

			glm::vec3 _orient = { GetOrient().x, GetOrient().y, GetOrient().z };
			glm::vec3 _viewDir = { GetViewDir().x, GetViewDir().y, GetViewDir().z };
			glm::mat3x3 rotation = {
				1, 0, 0,
				0, cos(radian), -sin(radian),
				0, sin(radian), cos(radian)
			};
			glm::vec3 newOrient = rotation * _orient;
			glm::vec3 newViewDir = rotation * _viewDir;
			
			GetOrient().x = newOrient.x;
			GetOrient().y = newOrient.y;
			GetOrient().z = newOrient.z;
			GetOrient().normalize();

			GetViewDir().x = newViewDir.x;
			GetViewDir().y = newViewDir.y;
			GetViewDir().z = newViewDir.z;
			GetViewDir().normalize();
		}
	};

	void HandUp()
	{
		RightHand* rightHand = dynamic_cast<RightHand*>(Components[EmBodyPartial::RightHand]);
		rightHand->HandUp();
	}

	void Init(Pnt3f _pos)
	{
		Pnt3f _size(1, 1, 1);
		NPGameObject::BasicObject::Init(_pos, _size);

		Pnt3f bodySize(3, 5, 3);
		Pnt3f bodyModifyPos(0, 0, 0);
		AddComponent(bodySize, bodyModifyPos, EmBodyPartial::Body);

		Pnt3f headSize(3.5f, 2, 3.5f);
		Pnt3f headModifyPos(0, Components[EmBodyPartial::Body]->Size.y / 2, 0);
		AddComponent(headSize, headModifyPos, EmBodyPartial::Head);
		int greenColor[3] = { 0, 255, 0 };
		int redColor[3] = { 255, 0, 0 };
		Components[EmBodyPartial::Head]->printer.SetSideColor(CubePrinter::EmSideIndex::Top, greenColor);
		Components[EmBodyPartial::Head]->printer.SetSideColor(CubePrinter::EmSideIndex::Front, redColor);

		Pnt3f handSize(1, 5, 1);
		Pnt3f rightHandModifyPos(Components[EmBodyPartial::Body]->Size.x / 2.0, Components[EmBodyPartial::Body]->Size.y / 2, 0);
		RightHand* rightHand = new RightHand(handSize, this, rightHandModifyPos, EmBodyPartial::RightHand);
		AddComponent(rightHand, rightHand->index);
		Components[EmBodyPartial::RightHand]->SetOrient(Pnt3f(
			Components[EmBodyPartial::RightHand]->GetOrient().x, -1, Components[EmBodyPartial::RightHand]->GetOrient().z));
		Components[EmBodyPartial::RightHand]->printer.DrawMethod = CubePrinter::EmDrawMethod::Top;
	}
};