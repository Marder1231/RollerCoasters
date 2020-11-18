/************************************************************************
     File:        TrainView.H

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu

     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     
						The TrainView is the window that actually shows the 
						train. Its a
						GL display canvas (Fl_Gl_Window).  It is held within 
						a TrainWindow
						that is the outer window with all the widgets. 
						The TrainView needs 
						to be aware of the window - since it might need to 
						check the widgets to see how to draw

	  Note:        we need to have pointers to this, but maybe not know 
						about it (beware circular references)

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#pragma once

// Preclarify for preventing the compiler error
class TrainWindow;
class CTrack;


//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


// The TrainView also needs to handle its user interface events.
// since this is an FLTk Window, we need to include FlTk Headers
// the pragmas turn off the warnings from FlTk - our code should have no warnings,
// but their code doesn't meet that standard
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <Fl/Fl_Gl_Window.h>
#include <time.h>

//#include "GL/gl.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GL/glu.h"
#include "TestGuy.h"
#include "Lamp.h"
#pragma warning(pop)


// this uses the old ArcBall Code
#include "Utilities/ArcBallCam.H"
#include "Utilities/Pnt3f.H"

class Smoke : public GameObject
{
	int existCounter = 0;
	const int existMaxTimeCounter = 624;
public:
	float SmokeWeakenRate = 1.0624f;
	Pnt3f GoDir;

	Smoke(float _speed, Pnt3f trainDir) : GameObject() { GoDir = Pnt3f(trainDir.x, _speed, trainDir.z); }
	Smoke(Pnt3f _pos, Pnt3f _size, float _speed, Pnt3f trainDir) : GameObject(_pos, _size) { GoDir = Pnt3f(trainDir.x, _speed, trainDir.z); }

	/// <summary>
	/// if return false, have to destroy smoke
	/// </summary>
	/// <returns></returns>
	bool SmokeFloat()
	{
		Pnt3f newDoDir(GoDir.x + float(rand() % 41 - 20) / 60.0f, -(GoDir.y / SmokeWeakenRate + 0.01f), GoDir.z + float(rand() % 41 - 20) / 60.0f);
		
		Pnt3f nextPos = this->GetPosition() - newDoDir;
		this->SetPosition(nextPos);
		GoDir = newDoDir;
		GoDir.y = -newDoDir.y;

		existCounter++;
		if (existCounter >= existMaxTimeCounter)
			return false;
		return true;
	}
};

//smoke must to compute in GPU
class Environment
{
public:
	static std::vector<Smoke*> Smokes;

	static void SmokesFloat();
};

class Train : public GameObject
{
public:
	Train() : GameObject() { }
	Train(Pnt3f _pos, Pnt3f _size) : GameObject(_pos, _size) { }

	float NowDistance = 0;

	struct EmBodyPartialIndex
	{
		enum _
		{
			Body = 0,
			Funnel = 1,
		};
	};

	class Funnel : public Component
	{
	public:
		int ShotSmokeCounter = 0;
		const int WhichTimeShotSmoke = 1;
		bool WhetherShotSmoke = false;

		Funnel(Pnt3f _size, BasicObject* parent, Pnt3f modifyPos, int _index) : Component(_size, parent, modifyPos, _index) { }

		void ShotSmoke(int amount = 1)
		{
			if (WhetherShotSmoke == false)
				return;
			ShotSmokeCounter--;
			if (ShotSmokeCounter > 0)
				return;

			for (int i = 0; i < amount; i++)
			{
				Smoke* smoke = new Smoke(this->GetPosition(), Pnt3f(1, 1, 1), 2, this->GetViewDir());
				int blackColor[3] = { 24, 24, 24 };
				smoke->printer.SetSideColor(CubePrinter::EmSideIndex::Top, blackColor);
				smoke->printer.SetSideColor(CubePrinter::EmSideIndex::Back, blackColor);
				smoke->printer.SetSideColor(CubePrinter::EmSideIndex::Bottom, blackColor);
				smoke->printer.SetSideColor(CubePrinter::EmSideIndex::Front, blackColor);
				smoke->printer.SetSideColor(CubePrinter::EmSideIndex::Right, blackColor);
				smoke->printer.SetSideColor(CubePrinter::EmSideIndex::Left, blackColor);

				Environment::Smokes.push_back(smoke);
			}
			ShotSmokeCounter = ShotSmokeCounter;;
		}
	};

	void Init(Pnt3f _pos, Pnt3f _orient)
	{
		Pnt3f _size(1, 1, 1);

		Pnt3f trainHeadSize(10, 8, 12);
		AddComponent(trainHeadSize, Pnt3f(0, 0, 0), 0);
		int frontColor[3] = { 0, 255, 255 };
		int topColor[3] = { 255, 255, 0 };
		Components[0]->SetOrient(_orient);
		Components[0]->printer.SetSideColor(CubePrinter::EmSideIndex::Top, topColor);
		Components[0]->printer.SetSideColor(CubePrinter::EmSideIndex::Front, frontColor);
		Components[0]->printer.DrawMethod = NPGameObject::CubePrinter::EmDrawMethod::Top;

		Pnt3f funnelSize(1, 2, 1);
		Pnt3f funnelModifyPos(0, Components[EmBodyPartialIndex::Body]->Size.y, 0);
		Funnel* funnel = new Funnel(funnelSize, this, funnelModifyPos, EmBodyPartialIndex::Funnel);
		AddComponent(funnel, funnel->index);
	}

	void WhetherShowSmoke(bool whether = true)
	{
		Funnel* funnel = dynamic_cast<Funnel*>(Components[EmBodyPartialIndex::Funnel]);
		funnel->WhetherShotSmoke = whether;
	}

	void ShotSmoke(int _amount = 1)
	{
		Funnel* funnel = dynamic_cast<Funnel*>(Components[EmBodyPartialIndex::Funnel]);
		funnel->ShotSmoke(_amount);
	}
};

class TrainView : public Fl_Gl_Window
{
	public:
		// note that we keep the "standard widget" constructor arguments
		TrainView(int x, int y, int w, int h, const char* l = 0);
		~TrainView();

		void Init();

		// overrides of important window things
		virtual int handle(int);
		virtual void draw();

		// all of the actual drawing happens in this routine
		// it has to be encapsulated, since we draw differently if
		// we're drawing shadows (no colors, for example)
		void drawStuff(bool doingShadows=false);

		// setup the projection - assuming that the projection stack has been
		// cleared for you
		void setProjection();


		// Reset the Arc ball control
		void resetArcball();

		// pick a point (for when the mouse goes down)
		void doPick();

		void Distance2T(float& _dsitance, int& _turnCounter, float& _trainU);
		void Distance2Pos(float _distance, Pnt3f& _pos, Pnt3f& _orient);
		float T2Distance(int& _turnCounter, float& _trianU);

		void GetPos(float const t, Pnt3f& pos, Pnt3f& orient);

		TestGuy fuckingMan;
		const int TrainAmount = 4;
		std::vector<Train*> Trains;
		void SetTrainPos();

		void ComputeDistance();
		void ARCGoGo(float);

		float TotalDistance = 0;
	public:
		ArcBallCam		arcball;			// keep an ArcBall for the UI
		int				selectedCube;  // simple - just remember which cube is selected

		TrainWindow*	tw;				// The parent of this display window
		CTrack*			m_pTrack;		// The track of the entire scene
};