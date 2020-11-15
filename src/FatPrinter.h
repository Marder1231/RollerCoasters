/*
	unUse
*/
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GL/glu.h"
#include "Utilities/Pnt3f.H"

class FatPrinter
{
public:
	void static Draw(bool doingShadows, Pnt3f pos, Pnt3f dir, Pnt3f orient, float width, float height, float length)
	{
		Pnt3f u = dir; u.normalize();
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
		glTranslatef(pos.x, pos.y, pos.z);
		glMultMatrixf(rotation);
		glRotatef(90, 0, 1, 0);
		glBegin(GL_QUADS);

		glBegin(GL_QUADS);

		if (!doingShadows)
			glColor3ub(0, 255, 255);
		glNormal3f(1, 0, 0);
		glVertex3f(width / 2, height / 2, -length / 2);
		glVertex3f(width / 2, height / 2, length / 2);
		glVertex3f(width / 2, 0, length / 2);
		glVertex3f(width / 2, 0, -length / 2);

		if (!doingShadows)
			glColor3ub(255, 0, 255);
		glNormal3f(0, 0, 1);
		glVertex3f(-width / 2, 0, length / 2);
		glVertex3f(-width / 2, height / 2, length / 2);
		glVertex3f(width / 2, height / 2, length / 2);
		glVertex3f(width / 2, 0, length / 2);

		if (!doingShadows)
			glColor3ub(255, 255, 255);
		glNormal3f(0, -1, 0);
		glVertex3f(-width / 2, 0, -length / 2);
		glVertex3f(-width / 2, 0, length / 2);
		glVertex3f(width / 2, 0, length / 2);
		glVertex3f(width / 2, 0, -length / 2);

		glNormal3f(-1, 0, 0);
		glVertex3f(-width / 2, height / 2, -length / 2);
		glVertex3f(-width / 2, height / 2, length / 2);
		glVertex3f(-width / 2, 0, length / 2);
		glVertex3f(-width / 2, 0, -length / 2);

		glNormal3f(0, 0, -1);
		glVertex3f(-width / 2, 0, -length / 2);
		glVertex3f(-width / 2, height / 2, -length / 2);
		glVertex3f(width / 2, height / 2, -length / 2);
		glVertex3f(width / 2, 0, -length / 2);

		glNormal3f(0, 0, 1);
		glVertex3f(-width / 2, 0, length / 2);
		glVertex3f(-width / 2, height / 2, length / 2);
		glVertex3f(width / 2, height / 2, length / 2);
		glVertex3f(width / 2, 0, length / 2);

		if (!doingShadows)
			glColor3ub(255, 255, 0);
		glNormal3f(0, 1, 0);
		glVertex3f(-width / 2, height / 2, -length / 2);
		glVertex3f(-width / 2, height / 2, length / 2);
		glVertex3f(width / 2, height / 2, length / 2);
		glVertex3f(width / 2, height / 2, -length / 2);

		glEnd();
		glPopMatrix();
	}
	//void static Draw(bool doingShadows, Pnt3f pos, Pnt3f dir, Pnt3f orient, GameObject object)
	//{

	//}
};