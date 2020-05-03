#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/image.h>

#include <NoisePanel.h>

class EditorFrame : public wxFrame
{
public:
    EditorFrame(wxWindow* parent, 
        wxWindowID id, 
        const wxString& title, 
        const wxPoint& pos=wxDefaultPosition, 
        const wxSize& size=wxDefaultSize, 
        long style=wxDEFAULT_FRAME_STYLE);

private:
    void set_properties();
    void do_layout();

protected:

    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    // begin wxGlade: EditorFrame::attributes
    wxMenuBar* EditorMenuBar;
    wxComboBox* MethodCombo;
    wxPropertyGridManager* NoisePropertyGrid;
    wxPanel* NoiseCtrl;
    NoisePanel* NoisePanelInst;
    wxPanel* NoiseShow;
    wxStatusBar* EditorInst_statusbar;
    // end wxGlade

    DECLARE_EVENT_TABLE();
};