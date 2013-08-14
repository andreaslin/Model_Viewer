#include "viewer.hpp"
#include "algebra.hpp"
#include <iostream>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

Viewer::Viewer()
{
	Glib::RefPtr<Gdk::GL::Config> glconfig;

	// Ask for an OpenGL Setup with
	//  - red, green and blue component colour
	//  - a depth buffer to avoid things overlapping wrongly
	//  - double-buffered rendering to avoid tearing/flickering
	glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
									   Gdk::GL::MODE_DEPTH |
									   Gdk::GL::MODE_DOUBLE);
	if (glconfig == 0) {
		// If we can't get this configuration, die
		std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
		abort();
	}

	// Accept the configuration
	set_gl_capability(glconfig);

	// Register the fact that we want to receive these events
	add_events(Gdk::BUTTON1_MOTION_MASK    |
			   Gdk::BUTTON2_MOTION_MASK    |
			   Gdk::BUTTON3_MOTION_MASK    |
			   Gdk::BUTTON_PRESS_MASK      | 
			   Gdk::BUTTON_RELEASE_MASK    |
			   Gdk::VISIBILITY_NOTIFY_MASK |
			   Gdk::POINTER_MOTION_MASK );

	initialize();
}

Viewer::~Viewer()
{
	// Nothing to do here right now.
}

void Viewer::invalidate()
{
	// Force a rerender
	Gtk::Allocation allocation = get_allocation();
	get_window()->invalidate_rect( allocation, false);
}

void Viewer::on_realize()
{
	// Do some OpenGL setup.
	// First, let the base class do whatever it needs to
	Gtk::GL::DrawingArea::on_realize();
  
	Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
  
	if (!gldrawable)
		return;

	if (!gldrawable->gl_begin(get_gl_context()))
		return;

	glShadeModel(GL_SMOOTH);
	glClearColor( 0.4, 0.4, 0.4, 0.0 );
	//glEnable(GL_DEPTH_TEST);

	gldrawable->gl_end();
}

bool Viewer::on_expose_event(GdkEventExpose* event)
{
	Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

	if (!gldrawable) return false;

	if (!gldrawable->gl_begin(get_gl_context()))
		return false;

	// Set up for perspective drawing 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, get_width(), get_height());
	gluPerspective(40.0, (GLfloat)get_width()/(GLfloat)get_height(), 0.1, 1000.0);

	// change to model view for drawing
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up lighting


	// Process options
	if ( z_buf ) {
		glEnable( GL_DEPTH_TEST );
	} else {
		glDisable( GL_DEPTH_TEST );
	}
	if ( bf_cull ) {
		glEnable( GL_CULL_FACE );
		glCullFace(GL_BACK);
	} else if ( ff_cull ) {
		glEnable( GL_CULL_FACE );
		glCullFace(GL_FRONT);
	} else {
		glDisable( GL_CULL_FACE );
	}

	// Draw stuff
	draw_puppet( mode == Viewer::JOINTS );

	if ( circle && mode != Viewer::JOINTS ) draw_trackball_circle();

	// Swap the contents of the front and back buffers so we see what we
	// just drew. This should only be done if double buffering is enabled.
	gldrawable->swap_buffers();

	gldrawable->gl_end();

	return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event)
{
	Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

	if (!gldrawable) return false;
  
	if (!gldrawable->gl_begin(get_gl_context()))
		return false;

	// Set up perspective projection, using current size and aspect
	// ratio of display

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, event->width, event->height);
	gluPerspective(40.0, (GLfloat)event->width/(GLfloat)event->height, 0.1, 1000.0);

	// Reset to modelview matrix mode
  
	glMatrixMode(GL_MODELVIEW);

	gldrawable->gl_end();

	return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event)
{
#ifdef DEBUG1
	std::cerr << "Stub: Button " << event->button << " pressed" << std::endl;
#endif
	if ( event->button == 1 ) button1_pressed = true;
	if ( event->button == 2 ) button2_pressed = true;
	if ( event->button == 3 ) button3_pressed = true;

	// Start picking joints
	if ( mode == Viewer::JOINTS ) {
		if ( event->button == 1 ) selectMode( event->x, event->y );
	}
	old_x = event->x;
	old_y = event->y;
	return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event)
{
#ifdef DEBUG1
	std::cerr << "Stub: Button " << event->button << " released" << std::endl;
#endif
	// If B2 or B3 was held down, update action stack
	if ( mode == Viewer::JOINTS && ( button2_pressed || button3_pressed ) ) {
		// Update action stack
		std::map<JointNode*, Info> sj;
		// Save all joints and their transformation
		for( std::map<unsigned int, Info>::iterator it = allJoints.begin(); it != allJoints.end(); it++ ) {
			Info temp;
			JointNode *node = (JointNode *)((*it).first);
			temp.old_m_trans = node->get_transform();
			temp.old_rotation = node->get_rotation();
			sj.insert( std::pair<JointNode*, Info>( node, temp ) );
		}

		// If the action pointer is not pointing the top, clear anything above the pointer and add the new action
		if ( action_it != actionStack.begin() ) actionStack.erase( actionStack.begin(), action_it );

		// Insert the new entry
		Action temp;
		temp.joints = sj;
		actionStack.push_front( temp ); 
		action_it = actionStack.begin(); // Remember to update the action pointer
	}

	if ( event->button == 1 ) button1_pressed = false;
	if ( event->button == 2 ) button2_pressed = false;
	if ( event->button == 3 ) button3_pressed = false;
	old_x = event->x;
	old_y = event->y;


	return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event)
{
#ifdef DEBUG1
	std::cerr << "Stub: Motion at " << event->x << ", " << event->y << std::endl;
#endif
	double angle = 1.0;
	double dis = 1.0;
	switch( mode ) {
	case Viewer::POS_ORIENT:
		vPerformTransfo( old_x, event->x, old_y, event->y );
		break;
	case Viewer::JOINTS:
		// Rotate the selected joints
		for( std::list<JointNode *>::iterator it = selectedJoints.begin(); it != selectedJoints.end(); it++ ) {
			if ( abs( event->y - old_y ) >= dis ) {
				double sign = ( ( event->y - old_y ) > 0 ? 1 : -1 );
				if ( button2_pressed ) (*it)->rotate( 'x', sign * angle );
			}
			if ( abs( event->x - old_x ) >= dis ) {
				double sign = ( ( event->x - old_x ) > 0 ? 1 : -1 );
				if ( button3_pressed ) (*it)->rotate( 'y', sign * angle );
			}
		}
		break;
	default:
		std::cerr << "Unknown Mode?!" << std::endl;
		break;
	}
	old_x = event->x;
	old_y = event->y;
	return true;
}

void Viewer::draw_trackball_circle()
{
	int current_width = get_width();
	int current_height = get_height();
  
	// Set up for orthogonal drawing to draw a circle on screen.
	// You'll want to make the rest of the function conditional on
	// whether or not we want to draw the circle this time around.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, current_width, current_height);
	glOrtho(0.0, (float)current_width, 
			0.0, (float)current_height, -0.1, 0.1);

	// change to model view for drawing
	glMatrixMode(GL_MODELVIEW);

	// Reset modelview matrix
	glLoadIdentity();

	// draw a circle for use with the trackball 
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glColor3f(1.0, 1.0, 1.0);
	double radius = current_width < current_height ? 
		(float)current_width * 0.25 : (float)current_height * 0.25;
	glTranslated((float)current_width * 0.5, (float)current_height * 0.5, 0);
	glBegin(GL_LINE_LOOP);
	for(size_t i=0; i<40; ++i) {
		double cosine = radius * cos(i*2*M_PI/40);
		double sine = radius * sin(i*2*M_PI/40);
		glVertex2f(cosine, sine);
	}
	glEnd();
	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_SMOOTH);
}

void Viewer::initialize() {
	button1_pressed = button2_pressed = button3_pressed = false;
	circle = z_buf = bf_cull = ff_cull = false;

	// Action stack for reseting the joints
	// This entry should never be removed from the action stack list and is always the last entry in the list
	root->findJoints( allJoints );
	std::map<JointNode*, Info> sj;
	for( std::map<unsigned int, Info>::iterator it = allJoints.begin(); it != allJoints.end(); it++ ) {
#ifdef DEBUG1
		std::cout << ((JointNode *)((*it).first))->get_name() << std::endl;
#endif
		sj.insert( std::pair<JointNode*, Info>( (JointNode *)((*it).first), (*it).second));
	}
	Action first;
	first.joints = sj;
	actionStack.push_back( first );
	action_it = actionStack.begin(); // Remember to set the action pointer
}

void Viewer::setOption( Viewer::Options option ) {
	// Check button, so just flip the boolean
	switch( option ) {
	case Viewer::CIRCLE:
		circle = !circle;
		break;
	case Viewer::Z_BUFFER:
		z_buf = !z_buf;
		break;
	case Viewer::BACK_CULL:
		bf_cull = !bf_cull;
		break;
	case Viewer::FRONT_CULL:
		ff_cull = !ff_cull;
		break;
	default:
		std::cerr << "Unknown options" << std::endl;
		break;
	}
}

void Viewer::setMode( Viewer::Modes mode ) {
	this->mode = mode;
}

void Viewer::reset( Viewer::Reset r ) {
	bool all = false;
	std::list<Action>::reverse_iterator it = actionStack.rbegin();
	switch ( r ) {
	case Viewer::ALL:
		all = true;
		// FALL THROUGH
	case Viewer::POS:
		m_translate = Matrix4x4();
		if ( !all ) break;
	case Viewer::ORIENT:
		m_rotate = Matrix4x4();
		if ( !all ) break;
	case Viewer::JOINTS_R:
		// Reset all joints using the last entry in aciton stack
		action_it = actionStack.end();
		action_it--;
		for( std::map<JointNode*, Info>::iterator it = (*action_it).joints.begin(); it != (*action_it).joints.end(); it++ ) {
			((*it).first)->set_transform( (*it).second.old_m_trans );
			((*it).first)->set_rotation( (*it).second.old_rotation );
			if ( ((*it).first)->get_pick() ) ((*it).first)->set_pick(); // Unpick all joints
		}
		// Clear action stack expect the reset entry
		actionStack.erase( actionStack.begin(), action_it );
		action_it = actionStack.begin();

		// Don't forget to clear the current selected joint list
		selectedJoints.clear();
		break;
	default:
		std::cerr << "Unknown reset options?!" << std::endl;
		break;
	}
}

void Viewer::draw_puppet( bool picking ) {
	/*	Still wondering why can't I use push and pop matrix :(
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		glLoadIdentity();
		// Apply world transformation
		glMultMatrixd( m_rotate.begin() );
		glMultMatrixd( m_translate.begin() ); */
	root->set_transform( root->get_transform() * m_rotate * m_translate );
	root->walk_gl( picking );	
	invalidate();							// Remember to redraw
	root->set_transform( root->get_transform() * ( m_rotate * m_translate ).invert() );
	//	glPopMatrix();
}

void Viewer::selectMode( int x, int y ) {
	GLint *viewport = new GLint[4];	
	glSelectBuffer( BUFFER_SIZE, pickBuffer ); 	/* initialize pick buffer */
	glGetIntegerv( GL_VIEWPORT, viewport ); 	/* set up pick view */

	// Initialize name stack
	glRenderMode( GL_SELECT );
	glInitNames();
	glPushName( -1 );

	// Save old projective matrix and set up a new one
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// Draw scene with appropriate name stack
	gluPickMatrix( x, viewport[3]-y, 1, 1, viewport );
	gluPerspective( 40.0, (GLfloat)get_width()/(GLfloat)get_height(), 0.1, 1000.0 );
	draw_puppet(false);

	// Restore projective matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Query for number of hits and process them
	int hits = glRenderMode(GL_RENDER);
#ifdef DEBUG1
	std::cout << "Number of hits: " << hits << std::endl;
#endif
	// Process the hits
	pickJoints( hits );
}

void Viewer::pickJoints( int hits ) {
	// NOTE: elements in the buffer are varies in group size, with first as number of names
	GLuint *offset = ( GLuint * )pickBuffer;
	unsigned int numNames = 0;
	SceneNode *invalid = (SceneNode *)0xffffffff;
	JointNode *joint = NULL;
	// Number of hits
	for( int i = 0; i < hits; i += 1 ) {
		numNames = *offset;
#ifdef DEBUG1
		std::cout << "Number of names: " << numNames << std::endl; offset++;
		std::cout << "Min z: " << *offset << std::endl; offset++;
		std::cout << "Max z: " << *offset << std::endl; offset++;
#endif
		offset += 3;			// Skip the min z and max z value
		// Number of names
		for ( unsigned int j = 0; j < numNames; j += 1 ) {
			SceneNode *node = (SceneNode *)(*offset);
			/* Weird place where I will get invalide JointNode pointer, so have to explictly check if the
			   JointNode pointer is valid or not */ 
			if ( node != invalid ) {
#ifdef DEBUG1
				std::cout << "Joint Name: " << node->get_name() << std::endl;
#endif
				if ( node->is_joint() ) joint = (JointNode *)node;
			}
			offset += 1;
		}
	}
	// We only keep the joint that is at the bottom of the hierarchy
	if ( joint != NULL ) {
#ifdef DEBUG1
		std::cout << "Picked Joint: " << joint->get_name() << std::endl;			
#endif			
		joint->set_pick();
		if ( joint->get_pick() ) {
			selectedJoints.push_back( joint ); // Add new joint to list of selected joints 
		} else {			
			selectedJoints.remove( joint );    // Remove joint from selected list 
		}

	}
	invalidate();
}

void Viewer::undo() {
	action_it++;				// Move pointer down by one entry
	// If action pointer is at the bottom of the stack pointing at the reset entry, nothing to undo
	if ( action_it == actionStack.end() || actionStack.size() == 1 ) {
		std::cerr << "Nothing to Undo!!!" << std::endl;
		action_it--;			// Move back to reset stack
		return;
	}
	// Restore
	for( std::map<JointNode*, Info>::iterator it = (*action_it).joints.begin(); it != (*action_it).joints.end(); it++ ) {
		((*it).first)->set_transform( (*it).second.old_m_trans );
		((*it).first)->set_rotation( (*it).second.old_rotation );
	}
}

void Viewer::redo() {
	action_it--;				// Move pointer up by one entry
	// If action pointer is at the top of stack or if the stack only contains the reset entry, nothing to redo
	if ( action_it == actionStack.end() || actionStack.size() == 1 ) {
		std::cerr << "Nothing to Redo!!!" << std::endl;
		action_it++;
		return;
	}
	for( std::map<JointNode*, Info>::iterator it = (*action_it).joints.begin(); it != (*action_it).joints.end(); it++ ) {
		((*it).first)->set_transform( (*it).second.old_m_trans );
		((*it).first)->set_rotation( (*it).second.old_rotation );
	}
	invalidate();
}

/****************************************************************
 *** Direct copy of part of code from trackball.c and event.c ***
 ****************************************************************/

/*******************************************************
 * 
 * void vCalcRotVec(float fNewX, float fNewY, 
 *                  float fOldX, float fOldY,
 *                  float fDiameter,
 *                  float *fVecX, float *fVecY, float *fVecZ);
 *
 *    Calculates a rotation vector based on mouse motion over
 *    a virtual trackball.
 *
 *    The fNew and fOld mouse positions
 *    should be in 'trackball' space. That is, they have been
 *    transformed into a coordinate system centered at the middle
 *    of the trackball. fNew, fOld, and fDiameter must all be specified
 *    in the same units (pixels for example).
 *
 * Parameters: fNewX, fNewY - New mouse position in trackball space.
 *                            This is the second point along direction
 *                            of rotation.
 *             fOldX, fOldY - Old mouse position in trackball space.
 *                            This is the first point along direction
 *                            of rotation.
 *             fDiameter - Diameter of the trackball. This should
 *                         be specified in the same units as fNew and fOld.
 *                         (ie, usually pixels if fNew and fOld are transformed
 *                         mouse positions)
 *             fVec - The output rotation vector. The length of the vector
 *                    is proportional to the angle of rotation.
 *
 *******************************************************/
void Viewer::vCalcRotVec(float fNewX, float fNewY,
						 float fOldX, float fOldY,
						 float fDiameter,
						 float *fVecX, float *fVecY, float *fVecZ) {
	//	long  nXOrigin, nYOrigin;
	float fNewVecX, fNewVecY, fNewVecZ,		/* Vector corresponding to new mouse location */
		fOldVecX, fOldVecY, fOldVecZ,        /* Vector corresponding to old mouse location */
		fLength;

	/* Vector pointing from center of virtual trackball to
	 * new mouse position
	 */
	fNewVecX    = fNewX * 2.0 / fDiameter;
	fNewVecY    = fNewY * 2.0 / fDiameter;
	fNewVecZ    = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);

	/* If the Z component is less than 0, the mouse point
	 * falls outside of the trackball which is interpreted
	 * as rotation about the Z axis.
	 */
	if (fNewVecZ < 0.0) {
		fLength = sqrt(1.0 - fNewVecZ);
		fNewVecZ  = 0.0;
		fNewVecX /= fLength;
		fNewVecY /= fLength;
	} else {
		fNewVecZ = sqrt(fNewVecZ);
	}

	/* Vector pointing from center of virtual trackball to
	 * old mouse position
	 */
	fOldVecX    = fOldX * 2.0 / fDiameter;
	fOldVecY    = fOldY * 2.0 / fDiameter;
	fOldVecZ    = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);
 
	/* If the Z component is less than 0, the mouse point
	 * falls outside of the trackball which is interpreted
	 * as rotation about the Z axis.
	 */
	if (fOldVecZ < 0.0) {
		fLength = sqrt(1.0 - fOldVecZ);
		fOldVecZ  = 0.0;
		fOldVecX /= fLength;
		fOldVecY /= fLength;
	} else {
		fOldVecZ = sqrt(fOldVecZ);
	}

	/* Generate rotation vector by calculating cross product:
	 * 
	 * fOldVec x fNewVec.
	 * 
	 * The rotation vector is the axis of rotation
	 * and is non-unit length since the length of a crossproduct
	 * is related to the angle between fOldVec and fNewVec which we need
	 * in order to perform the rotation.
	 */
	*fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
	*fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
	*fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;
}

/*******************************************************
 * void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix4x4 mNewMat)
 *    
 *    Calculate the rotation matrix for rotation about an arbitrary axis.
 *    
 *    The axis of rotation is specified by (fVecX,fVecY,fVecZ). The length
 *    of the vector is the amount to rotate by.
 *
 * Parameters: fVecX,fVecY,fVecZ - Axis of rotation
 *             mNewMat - Output matrix of rotation in column-major format
 *                       (ie, translation components are in column 3 on rows
 *                       0,1, and 2).
 *
 *******************************************************/
void Viewer::vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix4x4 &mNewMat) {
    float fRadians, fInvLength, fNewVecX, fNewVecY, fNewVecZ;

    /* Find the length of the vector which is the angle of rotation
     * (in radians)
     */
    fRadians = sqrt(fVecX * fVecX + fVecY * fVecY + fVecZ * fVecZ);

    /* If the vector has zero length - return the identity matrix */
    if (fRadians > -0.000001 && fRadians < 0.000001) {
		mNewMat = Matrix4x4();
        return;
    }

    /* Normalize the rotation vector now in preparation for making
     * rotation matrix. 
     */
    fInvLength = 1 / fRadians;
    fNewVecX   = fVecX * fInvLength;
    fNewVecY   = fVecY * fInvLength;
    fNewVecZ   = fVecZ * fInvLength;

    /* Create the arbitrary axis rotation matrix */
    double dSinAlpha = sin(fRadians);
    double dCosAlpha = cos(fRadians);
    double dT = 1 - dCosAlpha;

    mNewMat[0][0] = dCosAlpha + fNewVecX*fNewVecX*dT;
    mNewMat[0][1] = fNewVecX*fNewVecY*dT + fNewVecZ*dSinAlpha;
    mNewMat[0][2] = fNewVecX*fNewVecZ*dT - fNewVecY*dSinAlpha;
    mNewMat[0][3] = 0;

    mNewMat[1][0] = fNewVecX*fNewVecY*dT - dSinAlpha*fNewVecZ;
    mNewMat[1][1] = dCosAlpha + fNewVecY*fNewVecY*dT;
    mNewMat[1][2] = fNewVecY*fNewVecZ*dT + dSinAlpha*fNewVecX;
    mNewMat[1][3] = 0;

    mNewMat[2][0] = fNewVecZ*fNewVecX*dT + dSinAlpha*fNewVecY;
    mNewMat[2][1] = fNewVecZ*fNewVecY*dT - dSinAlpha*fNewVecX;
    mNewMat[2][2] = dCosAlpha + fNewVecZ*fNewVecZ*dT;
    mNewMat[2][3] = 0;

    mNewMat[3][0] = 0;
    mNewMat[3][1] = 0;
    mNewMat[3][2] = 0;
    mNewMat[3][3] = 1;
}

/*             
 * Name      : void vTranslate(float fTrans, char cAxis, Matrix mMat)                                                  
 *                                                                                                                           
 * Parameters: float  fAngle - The distance of translation.                                                                  
 *             char   cAxis  - The axis of rotation. 
 *             Matrix mMat   - The matrix to store the result in.
 * Returns   : void  
 * 
 * Purpose   : Computes the translation along the given axis and stores
 *             the result in mMat. 
 */
void Viewer::vTranslate(float fTrans, char cAxis, Matrix4x4 &mMat) { 
	mMat = Matrix4x4();
	switch(cAxis) {  
	case 'x': 
		mMat[0][3] = fTrans; 
		break;
	case 'y':
		mMat[1][3] = fTrans; 
		break;
	case 'z':
		mMat[2][3] = fTrans;
		break; 
	}
}
          
/*
 * Name      : void vPerformTransfo(float fOldX, float fNewX,
 *                                  float fOldY, float fNewY)
 *
 * Parameters: float fOldX - The previous X value.
 *             float fNewX - The current X value.
 *             float fOldY - The previous Y value.
 *             float fNewY - The current Y value.
 *
 * Returns   : void
 *
 * Purpose   : Updates the various transformation matrices.
 */
void Viewer::vPerformTransfo(float fOldX, float fNewX, float fOldY, float fNewY) 
{
    float  fRotVecX, fRotVecY, fRotVecZ;
    Matrix4x4 mNewMat;
	float nWinWidth = (float)get_width();
	float nWinHeight = (float)get_height();

    /*
     * Track ball rotations are being used.
     */
    if ( button3_pressed ) {
        float fDiameter;
        int iCenterX, iCenterY;
        float fNewModX, fNewModY, fOldModX, fOldModY;
	
        /* vCalcRotVec expects new and old positions in relation to the center of the
         * trackball circle which is centered in the middle of the window and
         * half the smaller of nWinWidth or nWinHeight.
         */
        fDiameter = (nWinWidth < nWinHeight) ? nWinWidth * 0.5 : nWinHeight * 0.5;
        iCenterX = nWinWidth / 2;
        iCenterY = nWinHeight / 2;
        fOldModX = fOldX - iCenterX;
        fOldModY = fOldY - iCenterY;
        fNewModX = fNewX - iCenterX;
        fNewModY = fNewY - iCenterY;

        vCalcRotVec(fNewModX, fNewModY,
					fOldModX, fOldModY,
					fDiameter,
					&fRotVecX, &fRotVecY, &fRotVecZ);
        /* Negate Y component since Y axis increases downwards
         * in screen space and upwards in OpenGL.
         */
        vAxisRotMatrix(fRotVecX, -fRotVecY, fRotVecZ, mNewMat);

        // Since all these matrices are meant to be loaded
        // into the OpenGL matrix stack, we need to transpose the
        // rotation matrix (since OpenGL wants rows stored
        // in columns)
		mNewMat = mNewMat.transpose();
        m_rotate = m_rotate * mNewMat;
    }

    /*
     * Pan translations are being used.
     */
    if ( button1_pressed ) {
        vTranslate((fNewX - fOldX) / ((float)SENS_PANX), 'x', mNewMat);
		m_translate = m_translate * mNewMat;
        vTranslate(-(fNewY - fOldY) / ((float)SENS_PANY), 'y', mNewMat);
		m_translate = m_translate * mNewMat;
    }
   
    /*
     * Zoom translations are being used.
     */
    if ( button2_pressed ) {
        vTranslate((fNewY - fOldY) / ((float)SENS_ZOOM), 'z', mNewMat);
		m_translate = m_translate * mNewMat;
    }
}

