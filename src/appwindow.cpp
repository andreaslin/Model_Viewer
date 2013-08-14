#include "appwindow.hpp"

AppWindow::AppWindow()
{
	set_title("Advanced Ergonomics Laboratory");

	// A utility class for constructing things that go into menus, which
	// we'll set up next.
	using Gtk::Menu_Helpers::MenuElem;
  
	// Set up the application menu
	// The slot we use here just causes AppWindow::hide() on this,
	// which shuts down the application.
	sigc::slot1<void, Viewer::Reset> reset_slot = 
		sigc::mem_fun( m_viewer, &Viewer::reset);
	m_menu_app.items().push_back(MenuElem("Reset Pos_ition", Gtk::AccelKey("I"),
										  sigc::bind(reset_slot, Viewer::POS)));
	m_menu_app.items().push_back(MenuElem("Reset _Orientation", Gtk::AccelKey("O"),
										  sigc::bind(reset_slot, Viewer::ORIENT)));
	m_menu_app.items().push_back(MenuElem("Reset Joi_nts", Gtk::AccelKey("N"),
										  sigc::bind(reset_slot, Viewer::JOINTS_R)));
	m_menu_app.items().push_back(MenuElem("Reset _All", Gtk::AccelKey("A"),
										  sigc::bind(reset_slot, Viewer::ALL)));
	m_menu_app.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
										  sigc::mem_fun(*this, &AppWindow::hide)));
  
	// Set up the mode menu
	sigc::slot1<void, Viewer::Modes> mode_slot = 
		sigc::mem_fun( m_viewer, &Viewer::setMode);
	m_menu_mode.items().push_back(Gtk::Menu_Helpers::RadioMenuElem( m_mode_group, "_Position/Orientation",
																	Gtk::AccelKey("P"),
																	sigc::bind(mode_slot, Viewer::POS_ORIENT)));
	m_menu_mode.items().push_back(Gtk::Menu_Helpers::RadioMenuElem( m_mode_group, "_Joints",
																	Gtk::AccelKey("J"),
																	sigc::bind(mode_slot, Viewer::JOINTS)));
	// Set initial mode to POS_ORIENT 
	m_viewer.setMode( Viewer::POS_ORIENT);


	// Set up the edit menu
	m_menu_edit.items().push_back(MenuElem("_Undo", Gtk::AccelKey("U"),
										   sigc::mem_fun(m_viewer, &Viewer::undo)));
	m_menu_edit.items().push_back(MenuElem("_Redo", Gtk::AccelKey("R"),
										   sigc::mem_fun(m_viewer, &Viewer::redo)));

	// Set up the option menu
	sigc::slot1<void, Viewer::Options> option_slot =
		sigc::mem_fun( m_viewer, &Viewer::setOption);
	m_menu_options.items().push_back(Gtk::Menu_Helpers::CheckMenuElem("_Circle", Gtk::AccelKey("C"),
																	  sigc::bind(option_slot, Viewer::CIRCLE)));
	m_menu_options.items().push_back(Gtk::Menu_Helpers::CheckMenuElem("_Z-buffer", Gtk::AccelKey("Z"),
																	  sigc::bind(option_slot, Viewer::Z_BUFFER)));
	m_menu_options.items().push_back(Gtk::Menu_Helpers::CheckMenuElem("_Backface Cull", Gtk::AccelKey("B"),
																	  sigc::bind(option_slot, Viewer::BACK_CULL)));
	m_menu_options.items().push_back(Gtk::Menu_Helpers::CheckMenuElem("_Frontface Cull", Gtk::AccelKey("F"),
																	  sigc::bind(option_slot, Viewer::FRONT_CULL)));

	// Set up the menu bar
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", m_menu_app));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Mode", m_menu_mode));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Edit", m_menu_edit));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Options", m_menu_options));
  
	// Pack in our widgets
  
	// First add the vertical box as our single "top" widget
	add(m_vbox);

	// Put the menubar on the top, and make it as small as possible
	m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);

	// Put the viewer below the menubar. pack_start "grows" the widget
	// by default, so it'll take up the rest of the window.
	m_viewer.set_size_request(500, 500);
	m_vbox.pack_start(m_viewer);

	show_all();
}
