#include <Editor.h>
#include <EditorInst.h>

// Insert appropriate main() function for each platform
wxIMPLEMENT_APP(Editor);

bool Editor::OnInit()
{
    wxInitAllImageHandlers();
    EditorInst *frame = new EditorInst(NULL);
    SetTopWindow(frame);
    frame->Show();
    return true;
}
