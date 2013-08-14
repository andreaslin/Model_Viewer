#include "scene.hpp"
#include <iostream>
#include <cmath>

#ifndef TO_RADIAN
#define TO_RADIAN M_PI / 180.0
#endif

SceneNode::SceneNode(const std::string& name)
	: m_name(name)
{
	rotation = Vector3D();
	changed = true;				// The normal has to be calculated for the first time
}

SceneNode::~SceneNode()
{
}

void SceneNode::walk_gl(bool picking) const
{
	// Walk through the child and apply the transformation
	// REMEMBER!!! OpenGL matrix is column-major, so transpose our matrix before multiply
	for ( ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++ ) {
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		glMultMatrixd( get_transform().transpose().begin() ); 				// Apply the transformation 
		(*it)->walk_gl(picking); 											// Walk down the hierachy 
		glPopMatrix();
	}
	
}

void SceneNode::rotate(char axis, double angle) 
{
#ifdef DEBUG1
	std::cerr << "Stub: Rotate " << m_name << " around " << axis << " by " << angle << std::endl;
#endif
	Matrix4x4 r;
	double angle_radian = TO_RADIAN * angle;
	switch ( axis ) {
	case 'x':
		rotation[0] += angle;
		if ( rotation[0] > 360.0 ) rotation[0] -= 360.0;
		if ( rotation[0] < -360.0 ) rotation[0] += 360.0;
		r[1][1] = cos( angle_radian );
		r[1][2] = -sin( angle_radian );
		r[2][1] = sin( angle_radian );
		r[2][2] = cos( angle_radian);
		break;
	case 'y':
		rotation[1] += angle;
		if ( rotation[1] > 360.0 ) rotation[1] -= 360.0;
		if ( rotation[1] < -360.0 ) rotation[1] += 360.0;
		r[0][0] = cos( angle_radian );
		r[0][2] = sin( angle_radian );
		r[2][0] = -sin( angle_radian );
		r[2][2] = cos( angle_radian );
		break;
	case 'z':
		r[0][0] = cos( angle_radian );
		r[0][1] = -sin( angle_radian );
		r[1][0] = sin( angle_radian );
		r[1][1] = cos( angle_radian );
		break;
	default:
		std::cerr << "Error while rotating" << std::endl;
		break;
	}

	// Apply the rotation
	set_transform( m_trans * r );

	// Indicate the node has changed
	hasChanged();

	// Check limits
	if ( this->is_joint() ) ((JointNode *)this)->checkLimits();
}

void SceneNode::scale(const Vector3D& amount)
{
#ifdef DEBUG1
	std::cerr << "Stub: Scale " << m_name << " by " << amount << std::endl;
#endif
	Matrix4x4 s;
	s[0][0] = amount[0];
	s[1][1] = amount[1];
	s[2][2] = amount[2];
  
	// Apply scaling
	set_transform( m_trans * s );

	// Indicate the node has changed
	hasChanged();
}

void SceneNode::translate(const Vector3D& amount)
{
#ifdef DEBUG1
	std::cerr << "Stub: Translate " << m_name << " by " << amount << std::endl;
#endif
	Matrix4x4 t;
	t[0][3] = amount[0];
	t[1][3] = amount[1];
	t[2][3] = amount[2];

	// Apply translation
	set_transform( m_trans * t );

	// Indicate the node has changed
	hasChanged();
}

bool SceneNode::is_joint() const
{
	return false;
}

void SceneNode::findJoints( std::map<unsigned int, Info> &sj ) {
	for ( ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++ ) {
		if ( this->is_joint() ) {
			Info temp;
			temp.old_m_trans = get_transform();
			temp.old_rotation = rotation;
			sj.insert( std::pair<unsigned int, Info>( (unsigned int)this, temp ) );
		}
		(*it)->findJoints( sj );
	}
}

JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	picked = false;
}

JointNode::~JointNode()
{
}

void JointNode::walk_gl(bool picking) const
{
	// Walk through the child and apply the transformation
	// REMEMBER!!! OpenGL matrix is column-major, so transpose our matrix before multiply
	glPushName( (unsigned int)this ); // Using the address of the node as name for easy retrieval
	for ( ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++ ) {
		glPushMatrix();
		glMultMatrixd( get_transform().transpose().begin() ); // Apply the transformation
		if ( picking && !( (*it)->is_joint() ) ) {			  // Only apply the picking if our next node is not a joint
			(*it)->walk_gl( picked );
		} else {
			(*it)->walk_gl( picking );						  // Walk down the hierachy 
		}
		glPopMatrix();
	}
	glPopName();
}

bool JointNode::is_joint() const
{
	return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

void JointNode::set_pick() {
	picked = !picked;
}

bool JointNode::get_pick() {
	return picked;
}

void JointNode::checkLimits() {
	// Checking the maximum rotation angle of x and y
	if ( rotation[0] > m_joint_x.max ) {
		rotate('x', m_joint_x.max - rotation[0] );
		rotation[0] = m_joint_x.max;
	} else if ( rotation[0] < m_joint_x.min ) {
		rotate('x', m_joint_x.min - rotation[0] );
		rotation[0] = m_joint_x.min;
	}
	if ( rotation[1] > m_joint_y.max ) {
		rotate('y', m_joint_y.max - rotation[1] );
		rotation[1] = m_joint_y.max;
	} else if ( rotation[1] < m_joint_y.min ) {
		rotate('y', m_joint_y.min - rotation[1] );
		rotation[1] = m_joint_y.min;			
	}	
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
	: SceneNode(name),
	  m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

void GeometryNode::walk_gl(bool picking) const
{
	// Draw the actual sphere
	// REMEMBER!!! OpenGL matrix is column-major, so transpose our matrix before multiply
	glPushMatrix();
	glMultMatrixd( get_transform().transpose().begin() );
	glPushName( (unsigned int)this );	// Use the address of the node as it's name for easy retrieval
	m_material->apply_gl();				// Apply material
	if ( changed ) {
		m_primitive->hasChanged();		// If the primitive has changed, inform.
		changed = false;
	}
	m_primitive->walk_gl(picking);		// Apply primitive
	glPopName();
	glPopMatrix();
}
 
