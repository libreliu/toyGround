#include <Editor.h>
#include <EditorFrame.h>

// Insert appropriate main() function for each platform
wxIMPLEMENT_APP(Editor);

bool Editor::OnInit()
{
    EditorFrame *frame = new EditorFrame();
    frame->Show(true);
    return true;
}
