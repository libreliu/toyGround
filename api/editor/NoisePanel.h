#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class NoisePanel : public wxPanel 
{
private:


public:
    // Status related
    NoisePanel(wxPanel *parent, wxWindowID id);

    // Window related
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    void OnSize(wxSizeEvent& event);
    void render(wxDC& dc);

    DECLARE_EVENT_TABLE();
};

