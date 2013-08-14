#include "primitive.hpp"

Primitive::Primitive() { changed = true; }

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
	// Remember to delete the display list
	glDeleteLists( mysphereID, 1 );
}

void Sphere::walk_gl(bool picking) const
{
	if ( changed || ( picking != old_picking ) ) {
		// Delete any previous display list
		glDeleteLists( mysphereID, 1 );
		
		// From OpenGL wiki - 2D Texture Mapping a Sphere
		GLUquadricObj *sphere = NULL;
		sphere = gluNewQuadric();
		// Picked parts will be drawn using only line
		if ( picking ) {
			gluQuadricDrawStyle( sphere, GLU_LINE );
		} else {
			gluQuadricDrawStyle( sphere, GLU_FILL );
		}
		gluQuadricTexture(sphere, true);
		gluQuadricNormals(sphere, GLU_SMOOTH);
		
		// Making a display list
		mysphereID = glGenLists(1);
		glNewList( mysphereID, GL_COMPILE );
		gluSphere(sphere, 1.0, 20, 20);
		glEndList();
		gluDeleteQuadric( sphere );
		changed = false;
	}
	glCallList(mysphereID);		// Draw Using a display list
	old_picking = picking;		// Save whether the node was picked or not
}

	void Primitive::hasChanged() {
		changed = true;
	}
