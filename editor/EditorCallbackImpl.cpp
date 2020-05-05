#include <Editor.h>
#include <EditorInst.h>

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

}

void EditorInst::OnMenuExit(wxCommandEvent &event) {
    
}

void EditorInst::OnMenuAbout(wxCommandEvent &event) {
    
}

void EditorInst::NoiseStartButtonClicked(wxCommandEvent &event) {
    NoiseShow->start();
}
