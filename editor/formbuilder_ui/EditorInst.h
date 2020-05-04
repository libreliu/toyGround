///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statusbr.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define ID_SAVE 1000
#define ID_ABOUT 1001

///////////////////////////////////////////////////////////////////////////////
/// Class EditorInst
///////////////////////////////////////////////////////////////////////////////
class EditorInst : public EditorFrame
{
	private:

	protected:
		wxMenuBar* EditorMenuBar;
		wxMenu* file;
		wxMenu* help;
		wxStatusBar* EditorInst_statusbar;
		wxSplitterWindow* m_splitter1;
		wxPanel* NoiseCtrl;
		wxStaticText* NoiseLabel1;
		wxComboBox* MethodCombo;
		wxPropertyGrid* NoisePropertyGrid;
		wxPanel* NoiseShow;

		// Virtual event handlers, overide them in your derived class
		void MethodComboChange( wxCommandEvent& event );
		void PropertyChanged( wxPropertyGridEvent& event );
		void OnNoiseShowPaint( wxPaintEvent& event );
		void OnNoiseShowSize( wxSizeEvent& event );


	public:

		EditorInst( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("toyGround Editor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1024, 768 ), long style = wxDEFAULT_FRAME_STYLE );

		~EditorInst();

		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 150 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( EditorInst::m_splitter1OnIdle ), NULL, this );
		}

};

