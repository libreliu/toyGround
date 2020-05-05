#include <Editor.h>
#include <EditorInst.h>
#include <wx/aboutdlg.h>

void EditorInst::MethodComboChange(wxCommandEvent &event) {
    // current selection
    std::string cur_sel = 
        static_cast<const char *>(MethodCombo->GetValue().c_str());
    wxLogDebug("Current selection: %s", cur_sel);

    // clear propgrid
    NoisePropertyGrid->Clear();

    // get all params
    NoiseShow->changeGenerator(cur_sel);
    auto key_list = NoiseShow->getAvailParams();
    for (auto &k: key_list) {
        auto val = NoiseShow->getCurVal(k);
        if (val.CheckType<int>()) {
            NoisePropertyGrid->Append(new wxIntProperty(k, wxPG_LABEL, val.As<int>()));
        } else if (val.CheckType<double>()) {
            NoisePropertyGrid->Append(new wxFloatProperty(k, wxPG_LABEL, val.As<double>()));
        } else {
            wxLogError("Wrong type!");
            return;
        }
    }

}

void EditorInst::PropertyChanged(wxPropertyGridEvent &event) {
    wxPGProperty *property = event.GetProperty();
    wxLogDebug("property %s changed", property->GetName());

    NoiseShow->setCurVal(
        std::string(property->GetName().c_str()), property->GetValue());
}

void EditorInst::OnMenuSave(wxCommandEvent &event) {
    if (!NoiseShow->isImageAvailable()) {
        wxLogStatus("Nothing to be saved for now..");
        return;
    }

    wxString path = wxSaveFileSelector("image", "PNG Files (*.png)|*.png");
    if (path.empty()) {
        wxLogStatus("Sorry, but it seems that you've entered nothing..");
        return;
    }

    NoiseShow->saveTo(path);
}

void EditorInst::OnMenuExit(wxCommandEvent &event) {
    Close(true);
}

void EditorInst::OnMenuAbout(wxCommandEvent &event) {
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("toyGround Editor");
    aboutInfo.SetDescription("A simple noise generator and editor.");
    aboutInfo.AddDeveloper("Libre Liu");

    wxAboutBox(aboutInfo, this);
}

void EditorInst::NoiseStartButtonClicked(wxCommandEvent &event) {
    NoiseShow->start();
}
