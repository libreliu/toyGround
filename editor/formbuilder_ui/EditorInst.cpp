///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "EditorInst.h"

///////////////////////////////////////////////////////////////////////////

EditorInst::EditorInst( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	EditorMenuBar = new wxMenuBar( 0 );
	file = new wxMenu();
	wxMenuItem* save;
	save = new wxMenuItem( file, ID_SAVE, wxString( wxT("Save") ) , wxEmptyString, wxITEM_NORMAL );
	file->Append( save );

	wxMenuItem* exit;
	exit = new wxMenuItem( file, ID_EXIT, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	file->Append( exit );

	EditorMenuBar->Append( file, wxT("File") );

	help = new wxMenu();
	wxMenuItem* about;
	about = new wxMenuItem( help, ID_ABOUT, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
	help->Append( about );

	EditorMenuBar->Append( help, wxT("Help") );

	this->SetMenuBar( EditorMenuBar );

	EditorInst_statusbar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( EditorInst::m_splitter1OnIdle ), NULL, this );
	m_splitter1->SetMinimumPaneSize( 150 );

	NoiseCtrl = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( NoiseCtrl, wxID_ANY, wxT("Noise Control") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 5 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	NoiseLabel1 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
	NoiseLabel1->Wrap( 0 );
	fgSizer1->Add( NoiseLabel1, 0, wxALIGN_CENTER, 0 );

	MethodCombo = new wxComboBox( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	MethodCombo->Append( wxT("Uniform") );
	MethodCombo->Append( wxT("Perlin") );
	MethodCombo->Append( wxT("Worley") );
	MethodCombo->Append( wxT("Fractual-Perlin") );
	MethodCombo->Append( wxT("Composite-FP") );
	fgSizer1->Add( MethodCombo, 0, wxEXPAND, 0 );


	sbSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );

	NoisePropertyGrid = new wxPropertyGrid(sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE);
	sbSizer1->Add( NoisePropertyGrid, 150, wxALL|wxEXPAND, 5 );

	NoiseStartButton = new wxButton( sbSizer1->GetStaticBox(), wxID_ANY, wxT("Run!"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( NoiseStartButton, 0, wxALL|wxEXPAND, 5 );


	NoiseCtrl->SetSizer( sbSizer1 );
	NoiseCtrl->Layout();
	sbSizer1->Fit( NoiseCtrl );
	NoiseShow = new NoisePanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_splitter1->SplitVertically( NoiseCtrl, NoiseShow, 150 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	// Connect Events
	file->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( EditorInst::OnMenuSave ), this, save->GetId());
	file->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( EditorInst::OnMenuExit ), this, exit->GetId());
	help->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( EditorInst::OnMenuAbout ), this, about->GetId());
	MethodCombo->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( EditorInst::MethodComboChange ), NULL, this );
	NoisePropertyGrid->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( EditorInst::PropertyChanged ), NULL, this );
	NoiseStartButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorInst::NoiseStartButtonClicked ), NULL, this );
}

EditorInst::~EditorInst()
{
	// Disconnect Events
	MethodCombo->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( EditorInst::MethodComboChange ), NULL, this );
	NoisePropertyGrid->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( EditorInst::PropertyChanged ), NULL, this );
	NoiseStartButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorInst::NoiseStartButtonClicked ), NULL, this );

}
