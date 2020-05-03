#include <NoisePanel.h>

// This wxPanel helds responsible for all noise related logic.

BEGIN_EVENT_TABLE(NoisePanel, wxPanel)
// some useful events
/*
 EVT_MOTION(NoisePanel::mouseMoved)
 EVT_LEFT_DOWN(NoisePanel::mouseDown)
 EVT_LEFT_UP(NoisePanel::mouseReleased)
 EVT_RIGHT_DOWN(NoisePanel::rightClick)
 EVT_LEAVE_WINDOW(NoisePanel::mouseLeftWindow)
 EVT_KEY_DOWN(NoisePanel::keyPressed)
 EVT_KEY_UP(NoisePanel::keyReleased)
 EVT_MOUSEWHEEL(NoisePanel::mouseWheelMoved)
 */

// catch paint events
EVT_PAINT(NoisePanel::paintEvent)
//Size event
EVT_SIZE(NoisePanel::OnSize)
END_EVENT_TABLE()


NoisePanel::NoisePanel(wxPanel* parent, wxWindowID id) : wxPanel(parent, id)
{
    // todo
}

void NoisePanel::paintEvent(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    this->render(dc);
}

void NoisePanel::paintNow() {

}

void NoisePanel::OnSize(wxSizeEvent& event) {
    
}

void NoisePanel::render(wxDC &dc) {

}