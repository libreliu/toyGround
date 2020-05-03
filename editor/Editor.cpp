#include <Editor.h>
#include <EditorFrame.h>

// Insert appropriate main() function for each platform
wxIMPLEMENT_APP(Editor);

bool Editor::OnInit()
{
    wxInitAllImageHandlers();
    EditorFrame *frame = new EditorFrame(NULL, wxID_ANY, wxEmptyString);
    SetTopWindow(frame);
    frame->Show();
    return true;
}
