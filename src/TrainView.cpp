/************************************************************************
     File:        TrainView.cpp

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

#include <iostream>
#include <Fl/fl.h>

// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>
#include <iostream>



#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"


#ifdef EXAMPLE_SOLUTION
#	include "TrainExample/TrainExample.H"
#endif

/*
	addition :
		$$$ arc,
		refactor draw train
		lamp with light, 
		a raycast at viewspace center
		smoke,
		people can hand up,
*/

//************************************************************************
//
// * Constructor to set up the GL window
//========================================================================
TrainView::
TrainView(int x, int y, int w, int h, const char* l) 
	: Fl_Gl_Window(x,y,w,h,l)
//========================================================================
{
	mode( FL_RGB|FL_ALPHA|FL_DOUBLE | FL_STENCIL );

	resetArcball();
}

void TrainView::Init()
{
	ComputeDistance();
	
	for (int i = 0; i < TrainAmount; i++)
	{
		Pnt3f pos, orient;
		Distance2Pos(i * -5, pos, orient);
		Train* train = new Train();
		train->Init(pos, orient);
		Trains.push_back(train);
	}

	Pnt3f manPos(0, 5, 0);
	fuckingMan.Init(manPos);
}

TrainView::~TrainView()
{
	for (int i = 0; i < Trains.size(); i++)
	{
		delete Trains[i];
	}
	Trains.clear();
}

void TrainView::ARCGoGo(float _speed)
{
	for (int i = 0; i < TrainAmount; i++)
	{
		Trains[i]->NowDistance += _speed;
		Distance2T(Trains[i]->NowDistance, m_pTrack->TurnCounter, m_pTrack->trainU);
	}
}
void TrainView::Distance2T(float& _dsitance, int& _turnCounter, float& _trainU)
{
	if (_dsitance < 0)
		_dsitance += TotalDistance;
	if (_dsitance > TotalDistance)
		_dsitance -= TotalDistance;

	//at where
	float computeDistance = _dsitance;
	int nowControlPointIndex = 0;
	for (int i = 0; i < m_pTrack->points.size(); i++)
	{
		computeDistance -= m_pTrack->PointDistances[i];
		nowControlPointIndex = i;
		if (computeDistance <= 0)
			break;
	}

	_turnCounter = nowControlPointIndex;
	_trainU = (computeDistance + m_pTrack->PointDistances[nowControlPointIndex])
		/ m_pTrack->PointDistances[nowControlPointIndex];
}
void TrainView::Distance2Pos(float _distance, Pnt3f& _pos, Pnt3f& _orient)
{
	int _turnCounter = 0;
	float _trainU = 0;
	Distance2T(_distance, _turnCounter, _trainU);

	GetPos(_turnCounter + _trainU, _pos, _orient);
}
float TrainView::T2Distance(int& _turnCounter, float& _trianU)
{
	float _distance = 0;
	for (int i = 0; i < _turnCounter; i++)
	{
		_distance += m_pTrack->PointDistances[i];
	}

	int nextIndex = _turnCounter;
	if (nextIndex < 0)
		nextIndex = 0;
	_distance += m_pTrack->PointDistances[nextIndex] * _trianU;
	return _distance;
}
void TrainView::ComputeDistance()
{
	const int clipcounter = 100;
	TotalDistance = 0;
	m_pTrack->PointDistances.clear();
	//compute distance
	Pnt3f cps;
	Pnt3f cpso;
	GetPos(0, cps, cpso);
	for (int i = 0; i < m_pTrack->points.size(); i++)
	{
		float t = i;
		float percent = 1.0f / clipcounter;

		float distancetotal = 0;

		for (int j = 0; j < clipcounter; j++)
		{
			t += percent;
			float interval = t - int(t);

			Pnt3f cpn;
			Pnt3f cpno;
			GetPos(t, cpn, cpno);

			Pnt3f diff = cpn - cps;
			float distance = abs(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
			distance = sqrt(distance);
			distancetotal += distance;
			TotalDistance += distance;
			cps = cpn;
		}
		m_pTrack->PointDistances.push_back(distancetotal);
	}
}

//************************************************************************
//
// * Reset the camera to look at the world
//========================================================================
void TrainView::
resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

//************************************************************************
//
// * FlTk Event handler for the window
//########################################################################
// TODO: 
//       if you want to make the train respond to other events 
//       (like key presses), you might want to hack this.
//########################################################################
//========================================================================
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, 
	// then we're done
	// note: the arcball only gets the event if we're in world view
	if (tw->worldCam->value())
		if (arcball.handle(event)) 
			return 1;

	ComputeDistance();
	// remember what button was used
	static int last_push;

	switch(event) {
		// Mouse button being pushed event
		case FL_PUSH:
			last_push = Fl::event_button();
			// if the left button be pushed is left mouse button
			if (last_push == FL_LEFT_MOUSE  ) {
				doPick();
				damage(1);
				return 1;
			};
			break;

	   // Mouse button release event
		case FL_RELEASE: // button release
			damage(1);
			last_push = 0;
			return 1;

		// Mouse button drag event
		case FL_DRAG:

			// Compute the new control point position
			if ((last_push == FL_LEFT_MOUSE) && (selectedCube >= 0)) {
				ControlPoint* cp = &m_pTrack->points[selectedCube];

				double r1x, r1y, r1z, r2x, r2y, r2z;
				getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

				double rx, ry, rz;
				mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z, 
								static_cast<double>(cp->pos.x), 
								static_cast<double>(cp->pos.y),
								static_cast<double>(cp->pos.z),
								rx, ry, rz,
								(Fl::event_state() & FL_CTRL) != 0);

				cp->pos.x = (float) rx;
				cp->pos.y = (float) ry;
				cp->pos.z = (float) rz;
				damage(1);
			}
			break;

		// in order to get keyboard events, we need to accept focus
		case FL_FOCUS:
			return 1;

		// every time the mouse enters this window, aggressively take focus
		case FL_ENTER:	
			focus(this);
			break;

		case FL_KEYBOARD:
		 		int k = Fl::event_key();
				int ks = Fl::event_state();
				if (k == 'p') {
					// Print out the selected control point information
					if (selectedCube >= 0) 
						printf("Selected(%d) (%g %g %g) (%g %g %g)\n",
								 selectedCube,
								 m_pTrack->points[selectedCube].pos.x,
								 m_pTrack->points[selectedCube].pos.y,
								 m_pTrack->points[selectedCube].pos.z,
								 m_pTrack->points[selectedCube].orient.x,
								 m_pTrack->points[selectedCube].orient.y,
								 m_pTrack->points[selectedCube].orient.z);
					else
						printf("Nothing Selected\n");

					return 1;
				};
				break;
	}

	return Fl_Gl_Window::handle(event);
}

//************************************************************************
//
// * this is the code that actually draws the window
//   it puts a lot of the work into other routines to simplify things
//========================================================================
void TrainView::draw()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//initialized glad
	if (gladLoadGL())
	{
		//initiailize VAO, VBO, Shader...
	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	// Set up the view port
	glViewport(0, 0, w(), h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0, 0, .3f, 0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (tw->topCam->value()) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[] = { 0,1,1,0 }; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };
	GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
	GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };
	GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };

	/*
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);
	*/
	float DirnoAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float blueAmbientDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float position[] = { 1.0f, 0.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT1, GL_AMBIENT, DirnoAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, blueAmbientDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	//float noAmbient[] = { 0, 0, 0.2f, 1 };
	//float diffuse[] = { 1, 0, 1, 1 };
	//float spotPosition[] = { 0, 10, 0, 1 };

	//glLightfv(GL_LIGHT2, GL_AMBIENT, noAmbient);
	//glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	//glLightfv(GL_LIGHT2, GL_POSITION, spotPosition);

	//float direction[] = { 0, -1, 0 };
	//glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, direction);
	//glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 10);
	//glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 1.0f);

	//glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1.0f);
	//glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.0f);
	//glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.0f);


	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)
	glUseProgram(0);

	setupFloor();
	//glDisable(GL_LIGHTING);
	drawFloor(200, 10);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (!tw->topCam->value()) {

		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	// Check whether we use the world camp
	if (tw->worldCam->value())
		arcball.setProjection(false);
	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		}
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90, 1, 0, 0);
	}
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
		glClear(GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		gluPerspective(60, aspect, 0.01, 200);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		Pnt3f viewPos = Trains[0]->Components[0]->GetPosition() + Trains[0]->Components[0]->GetOrient() * (Trains[0]->Components[0]->Size.y / 2) + Trains[0]->Components[0]->GetViewDir() * (Trains[0]->Components[0]->Size.z * 1.1f / 2);
		gluLookAt(viewPos.x, viewPos.y, viewPos.z,
			viewPos.x + Trains[0]->Components[0]->GetViewDir().x,
			viewPos.y + Trains[0]->Components[0]->GetViewDir().y,
			viewPos.z + Trains[0]->Components[0]->GetViewDir().z,
			Trains[0]->Components[0]->GetOrient().x, 
			Trains[0]->Components[0]->GetOrient().y,
			Trains[0]->Components[0]->GetOrient().z);


#ifdef EXAMPLE_SOLUTION
		trainCamView(this, aspect);
#endif
	}
}

void TrainView::GetPos(float const t, Pnt3f& pos, Pnt3f& orient)
{
	if (tw->splineBrowser->selected(1) == true)
	{
		// pos
		Pnt3f cp1 = m_pTrack->points[int(t) % m_pTrack->points.size()].pos;
		Pnt3f cp2 = m_pTrack->points[(int(t) + 1) % m_pTrack->points.size()].pos;

		//orient
		Pnt3f cp1o = m_pTrack->points[int(t) % m_pTrack->points.size()].orient;
		Pnt3f cp2o = m_pTrack->points[(int(t) + 1) % m_pTrack->points.size()].orient;

		float interval = t - int(t);

		pos = (1 - interval) * cp1 + interval * cp2;
		orient = (1 - interval) * cp1o + interval * cp2o;
		orient.normalize(); 
	}
	else if (tw->splineBrowser->selected(2) == true)
	{
		// pos
		ControlPoint cp1 = m_pTrack->points[int(t) % m_pTrack->points.size()];
		ControlPoint cp2 = m_pTrack->points[(int(t) + 1) % m_pTrack->points.size()];
		ControlPoint cp3 = m_pTrack->points[(int(t) + 2) % m_pTrack->points.size()];
		ControlPoint cp4 = m_pTrack->points[(int(t) + 3) % m_pTrack->points.size()];

		float interval = t - int(t);

		glm::mat4x3 matG =
		{
			cp1.pos.x, cp1.pos.y, cp1.pos.z,
			cp2.pos.x, cp2.pos.y, cp2.pos.z,
			cp3.pos.x, cp3.pos.y, cp3.pos.z,
			cp4.pos.x, cp4.pos.y, cp4.pos.z,
		};		
		glm::mat4x3 matO =
		{
			cp1.orient.x, cp1.orient.y, cp1.orient.z,
			cp2.orient.x, cp2.orient.y, cp2.orient.z,
			cp3.orient.x, cp3.orient.y, cp3.orient.z,
			cp4.orient.x, cp4.orient.y, cp4.orient.z,
		};
		glm::mat4x4 matM =
		{
			-0.5f, 1.5f, -1.5f, 0.5f, 
			1, -2.5f, 2, -0.5f, 
			-0.5f, 0, 0.5f, 0, 
			0, 1, 0, 0
		};
		glm::fvec4 vecT = { interval * interval * interval, interval * interval, interval, 1 };

		glm::fvec3 qt = matG * matM * vecT;
		glm::fvec3 ot = matO * matM * vecT;

		pos.x = qt.x;
		pos.y = qt.y;
		pos.z = qt.z;
		orient.x = ot.x;
		orient.y = ot.y;
		orient.z = ot.z;
	}
	else if (tw->splineBrowser->selected(3) == true)
	{
		// pos
		ControlPoint cp1 = m_pTrack->points[int(t) % m_pTrack->points.size()];
		ControlPoint cp2 = m_pTrack->points[(int(t) + 1) % m_pTrack->points.size()];
		ControlPoint cp3 = m_pTrack->points[(int(t) + 2) % m_pTrack->points.size()];
		ControlPoint cp4 = m_pTrack->points[(int(t) + 3) % m_pTrack->points.size()];

		float interval = t - int(t);

		glm::mat4x3 matG =
		{
			cp1.pos.x, cp1.pos.y, cp1.pos.z,
			cp2.pos.x, cp2.pos.y, cp2.pos.z,
			cp3.pos.x, cp3.pos.y, cp3.pos.z,
			cp4.pos.x, cp4.pos.y, cp4.pos.z,
		};
		glm::mat4x3 matO =
		{
			cp1.orient.x, cp1.orient.y, cp1.orient.z,
			cp2.orient.x, cp2.orient.y, cp2.orient.z,
			cp3.orient.x, cp3.orient.y, cp3.orient.z,
			cp4.orient.x, cp4.orient.y, cp4.orient.z,
		};
		glm::mat4x4 matM =
		{
			-1 / 6.0f, 3 / 6.0f, -3 / 6.0f, 1 / 6.0f,
			3 / 6.0f, -6 / 6.0f, 3 / 6.0f, 0,
			-3 / 6.0f, 0, 3 / 6.0f, 0,
			1 / 6.0f, 4 / 6.0f, 1 / 6.0f, 0
		};
		glm::fvec4 vecT = { interval * interval * interval, interval * interval, interval, 1 };

		glm::fvec3 qt = matG * matM * vecT;
		glm::fvec3 ot = matO * matM * vecT;

		pos.x = qt.x;
		pos.y = qt.y;
		pos.z = qt.z;
		orient.x = ot.x;
		orient.y = ot.y;
		orient.z = ot.z;
	}
}

void TrainView::SetTrainPos()
{
	Pnt3f trainPos;
	Pnt3f trainOrient;

	GetPos(m_pTrack->trainU + m_pTrack->TurnCounter, trainPos, trainOrient);

	Pnt3f dir = trainPos - Trains[0]->Components[0]->GetPosition();
	if (dir.x == 0 && dir.y == 0 && dir.z == 0)
	{

	}
	else
	{
		Trains[0]->SetPosition(trainPos);
		Trains[0]->Components[0]->SetViewDir(dir);
		Trains[0]->Components[0]->SetOrient(trainOrient);
	}

	float headDistance = T2Distance(m_pTrack->TurnCounter, m_pTrack->trainU);
	for (int i = 1; i < TrainAmount; i++)
	{
		float _distance = headDistance - i * 12.46f;
		Pnt3f _pos;
		Pnt3f _orient;

		Distance2Pos(_distance, _pos, _orient);

		Pnt3f _dir = _pos - Trains[i]->Components[0]->GetPosition();
		if (dir.x == 0 && dir.y == 0 && dir.z == 0)
		{

		}
		else
		{
			Trains[i]->SetPosition(_pos);
			Trains[i]->Components[0]->SetViewDir(_dir);
			Trains[i]->Components[0]->SetOrient(_orient);
		}
	}
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (!tw->trainCam->value()) {
		for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if (((int)i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			m_pTrack->points[i].draw();
		}
	}
	// draw the track
	//####################################################################
	// TODO: 

	Pnt3f cpS;
	Pnt3f cpSo;

	GetPos(0, cpS, cpSo);


	for (int i = 0; i < m_pTrack->points.size(); i++)
	{
		float dividLine = m_pTrack->PointDistances[i] / 5;
		float t = i;
		float percent = 1.0f / dividLine;

		for (int j = 0; j < dividLine; j++)
		{
			t += percent;
			float interval = t - int(t);

			Pnt3f cpN;
			Pnt3f cpNo;
			GetPos(t, cpN, cpNo);

			Pnt3f cross = (cpN - cpS) * cpNo;
			cross.normalize();
			cross = cross * 2.5f;

			Pnt3f u = cpN - cpS; u.normalize();
			Pnt3f w = u * cpNo; w.normalize();
			Pnt3f v = w * u; v.normalize();
			

			glLineWidth(3);
			if (!doingShadows)
				glColor3ub(32, 32, 64);
			glLineWidth(3);
			glBegin(GL_LINES);
			glVertex3f(cpS.x + cross.x, cpS.y + cross.y, cpS.z + cross.z);
			glVertex3f(cpN.x + cross.x, cpN.y + cross.y, cpN.z + cross.z);
			glVertex3f(cpS.x - cross.x, cpS.y - cross.y, cpS.z - cross.z);
			glVertex3f(cpN.x - cross.x, cpN.y - cross.y, cpN.z - cross.z);
			glEnd();

			float barWidth = 2;
			float barHeight = 0.5f;
			float barLength = 6;

			float rotation[16] =
			{
				u.x, u.y, u.z, 0.0f,
				v.x, v.y, v.z, 0.0f,
				w.x, w.y, w.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f,
			};

			glPushMatrix();
			glTranslatef(cpN.x, cpN.y, cpN.z);
			glMultMatrixf(rotation);
			glRotated(90, 0, 1, 0);

			glBegin(GL_QUADS);
			if (!doingShadows)
				glColor3ub(255, 255, 255);
			glNormal3f(0, -1, 0);
			glVertex3f(-barLength / 2, -barHeight, -barWidth / 2);
			glVertex3f(-barLength / 2, -barHeight, barWidth / 2);
			glVertex3f(barLength / 2, -barHeight, barWidth / 2);
			glVertex3f(barLength / 2, -barHeight, -barWidth / 2);


			glNormal3f(-1, 0, 0);
			glVertex3f(-barLength / 2, barHeight / 2, -barWidth / 2);
			glVertex3f(-barLength / 2, barHeight / 2, barWidth / 2);
			glVertex3f(-barLength / 2, -barHeight, barWidth / 2);
			glVertex3f(-barLength / 2, -barHeight, -barWidth / 2);

			glNormal3f(1, 0, 0);
			glVertex3f(barLength / 2, barHeight / 2, -barWidth / 2);
			glVertex3f(barLength / 2, barHeight / 2, barWidth / 2);
			glVertex3f(barLength / 2, -barHeight, barWidth / 2);
			glVertex3f(barLength / 2, -barHeight, -barWidth / 2);

			glNormal3f(0, 0, -1);
			glVertex3f(-barLength / 2, -barHeight, -barWidth / 2);
			glVertex3f(-barLength / 2, barHeight / 2, -barWidth / 2);
			glVertex3f(barLength / 2, barHeight / 2, -barWidth / 2);
			glVertex3f(barLength / 2, -barHeight, -barWidth / 2);


			glNormal3f(0, 0, 1);
			glVertex3f(-barLength / 2, -barHeight, barWidth / 2);
			glVertex3f(-barLength / 2, barHeight / 2, barWidth / 2);
			glVertex3f(barLength / 2, barHeight / 2, barWidth / 2);
			glVertex3f(barLength / 2, -barHeight, barWidth / 2);

			if (!doingShadows)
				glColor3ub(255, 0, 0);
			glNormal3f(-barHeight, 1, -barHeight);
			glVertex3f(-barLength / 2, barHeight / 2, -barWidth / 2);
			glVertex3f(-barLength / 2, barHeight / 2, barWidth / 2);
			glVertex3f(barLength / 2, barHeight / 2, barWidth / 2);
			glVertex3f(barLength / 2, barHeight / 2, -barWidth / 2);

			glEnd();
			glPopMatrix();

			cpS = cpN;
		}
	}


	// call your own track drawing code
	//####################################################################

#ifdef EXAMPLE_SOLUTION
	drawTrack(this, doingShadows);
#endif

	// draw the train
	//####################################################################
	// TODO: 

	if (!tw->trainCam->value())
	{
		for (int i = 0; i < TrainAmount; i++)
			Trains[i]->Draws(doingShadows);
	}
	fuckingMan.Draws(doingShadows);

	//	call your own train drawing code
	//####################################################################
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif
}

// 
//************************************************************************
//
// * this tries to see which control point is under the mouse
//	  (for when the mouse is clicked)
//		it uses OpenGL picking - which is always a trick
//########################################################################
// TODO: 
//		if you want to pick things other than control points, or you
//		changed how control points are drawn, you might need to change this
//########################################################################
//========================================================================
void TrainView::
doPick()
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	make_current();		

	// where is the mouse?
	int mx = Fl::event_x(); 
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
						5, 5, viewport);

	ComputeDistance();
	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n",selectedCube);
}