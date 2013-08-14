#include "material.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
	: m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl() const
{
	// Perform OpenGL calls necessary to set up this material.
	// Most of the code below is copy from OpenGL Programming Guide: Chapter 6 - Example 6-1
	GLfloat mat_diffuse[] = { m_kd.R(), m_kd.G(), m_kd.B(), 1.0 };
	GLfloat mat_shininess[] = { m_shininess };
	// Do I need this? Yes, position suggested by instructor
	GLfloat light_position[] = { 10.0, 10.0, 4.0, 1.0 };

	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	
}
