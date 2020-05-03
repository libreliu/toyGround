#include <Editor.h>
#include <EditorFrame.h>

BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
    // begin wxGlade: EditorFrame::event_table
    // end wxGlade
END_EVENT_TABLE();


EditorFrame::EditorFrame(wxWindow* parent, wxWindowID id, 
    const wxString& title, const wxPoint& pos, const wxSize& size, 
    long style)
    : wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
    // begin wxGlade: EditorFrame::EditorFrame
    SetSize(wxSize(1024, 768));
    NoiseShow = new wxPanel(this, wxID_ANY);
    NoiseCtrl = new wxPanel(this, wxID_ANY);
    EditorMenuBar = new wxMenuBar();
    wxMenu *wxglade_tmp_menu;
    wxglade_tmp_menu = new wxMenu();
    EditorMenuBar->Append(wxglade_tmp_menu, wxT("About"));
    SetMenuBar(EditorMenuBar);
    const wxString MethodCombo_choices[] = {
        wxT("Uniform"),
        wxT("Perlin"),
        wxT("Worley"),
        wxT("Hybrid"),
    };
    MethodCombo = new wxComboBox(NoiseCtrl, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 4, MethodCombo_choices, wxCB_DROPDOWN);
    NoisePropertyGrid = new wxPropertyGridManager(NoiseCtrl, wxID_ANY);
    NoisePanelInst = new NoisePanel(NoiseShow, wxID_ANY);
    EditorInst_statusbar = CreateStatusBar(1);

    set_properties();
    do_layout();
    // end wxGlade
}
void EditorFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void EditorFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
void EditorFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}


void EditorFrame::set_properties()
{
    // begin wxGlade: EditorFrame::set_properties
    SetTitle(wxT("toyGround Editor"));
    int EditorInst_statusbar_widths[] = { -1 };
    EditorInst_statusbar->SetStatusWidths(1, EditorInst_statusbar_widths);

    // statusbar fields
    const wxString EditorInst_statusbar_fields[] = {
        wxT("EditorInst_statusbar"),
    };
    for(int i = 0; i < EditorInst_statusbar->GetFieldsCount(); ++i) {
        EditorInst_statusbar->SetStatusText(EditorInst_statusbar_fields[i], i);
    }
    // end wxGlade
}


void EditorFrame::do_layout()
{
    // begin wxGlade: EditorFrame::do_layout
    wxBoxSizer* MainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* NoiseShowSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* CtrlSizer = new wxStaticBoxSizer(new wxStaticBox(NoiseCtrl, wxID_ANY, wxT("Noise Control")), wxVERTICAL);
    wxBoxSizer* CtrlSizer2 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* CtrlSizer3 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* NoiseLabel1 = new wxStaticText(NoiseCtrl, wxID_ANY, wxT("Type"));
    CtrlSizer3->Add(NoiseLabel1, 0, 0, 0);
    CtrlSizer3->Add(MethodCombo, 0, 0, 0);
    CtrlSizer2->Add(CtrlSizer3, 1, wxEXPAND, 0);
    CtrlSizer2->Add(NoisePropertyGrid, 10, wxEXPAND, 0);
    CtrlSizer->Add(CtrlSizer2, 1, wxEXPAND, 0);
    NoiseCtrl->SetSizer(CtrlSizer);
    MainSizer->Add(NoiseCtrl, 1, wxEXPAND, 0);
    NoiseShowSizer->Add(NoisePanelInst, 1, wxEXPAND, 0);
    NoiseShow->SetSizer(NoiseShowSizer);
    MainSizer->Add(NoiseShow, 5, wxEXPAND, 0);
    SetSizer(MainSizer);
    Layout();
    // end wxGlade
}
