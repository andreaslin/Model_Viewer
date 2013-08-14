#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include "scene_lua.hpp"
#include "scene.hpp"
#include <list>
#include <map>

// Constants from event.h for world rotation and translation
#define SENS_PANX 30.0
#define SENS_PANY 23.0
#define SENS_ZOOM 35.0

// Size of buffer
#define BUFFER_SIZE 512

extern SceneNode *root;			// Puppet

// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
	Viewer();
	virtual ~Viewer();

	// A useful function that forces this widget to rerender. If you
	// want to render a new frame, do not call on_expose_event
	// directly. Instead call this, which will cause an on_expose_event
	// call when the time is right.
	void invalidate();

	// Public modes
	enum Modes { POS_ORIENT, JOINTS };
	void setMode( Viewer::Modes mode );

	// Public options
	enum Options { CIRCLE, Z_BUFFER, BACK_CULL, FRONT_CULL };
	void setOption( Viewer::Options option );

	// Public reset options
	enum Reset { POS, ORIENT, JOINTS_R, ALL };
	void reset( Viewer::Reset r );

	// Public undo and redo options
	void undo();
	void redo();
  
protected:

	// Events we implement
	// Note that we could use gtkmm's "signals and slots" mechanism
	// instead, but for many classes there's a convenient member
	// function one just needs to define that'll be called with the
	// event.

	// Called when GL is first initialized
	virtual void on_realize();
	// Called when our window needs to be redrawn
	virtual bool on_expose_event(GdkEventExpose* event);
	// Called when the window is resized
	virtual bool on_configure_event(GdkEventConfigure* event);
	// Called when a mouse button is pressed
	virtual bool on_button_press_event(GdkEventButton* event);
	// Called when a mouse button is released
	virtual bool on_button_release_event(GdkEventButton* event);
	// Called when the mouse moves
	virtual bool on_motion_notify_event(GdkEventMotion* event);

	// Draw a circle for the trackball, with OpenGL commands.
	// Assumes the context for the viewer is active.
	void draw_trackball_circle();

	// Draw puppet
	void draw_puppet( bool picking );

	// Copy of the code for trackball from trackball.h and event.h
	void vCalcRotVec(float fNewX, float fNewY,
	                 float fOldX, float fOldY,
	                 float fDiameter,
	                 float *fVecX, float *fVecY, float *fVecZ);
	void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix4x4 &mNewMat);
	void vPerformTransfo(float fOldX, float fNewX, float fOldY, float fNewY);
	void vTranslate(float fTrans, char cAxis, Matrix4x4 &mMat);

	// Joint Selection
	void selectMode( int x, int y ); 	// Start selecting parts 
	void pickJoints( int hits );		// Pick out the joint
  
private:
	// Action structure
	typedef SceneNode::Info Info;
	struct Action {
		std::map<JointNode*, Info> joints;
	};

	bool button1_pressed, button2_pressed, button3_pressed;	// Multi-press button 
	bool circle, z_buf, bf_cull, ff_cull;                   // Circle, z-buffer, backface cull and frontface cull
	Viewer::Modes mode;                                     // Mode
	Matrix4x4 m_rotate, m_translate;                        // Matrix for world rotation and translation
	int old_x, old_y;                                       // Old position of x and y
	GLuint pickBuffer[BUFFER_SIZE];
	std::list<Action> actionStack;                          // Undo/Redo stack 
	std::list<Action>::iterator action_it;                  // Action stack iterator
	std::list<JointNode *> selectedJoints;                  // Selected Joints
	std::map<unsigned int, Info> allJoints;					// Map that constains all the joints
	
	void initialize();
};

#endif
