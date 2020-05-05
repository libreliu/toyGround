#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/rawbmp.h>

#include <generators/Generators.h>
#include <set>
#include <memory>
#include <NoiseWorker.h>

class NoisePanel : public wxPanel 
{
private:
    // unique id for worker event
    // shouldn't conflict with wx's id
    // and also a compile-time constant
    static const int evfinish_id = 114514;

    std::string generator_type;

    std::map<std::string, wxAny> cur_param_dict;
    std::set<std::string> cur_avail_params;

    // -- graphics resources --
    bool image_available;
    wxBitmap image;
    // ----------------------

    // -- worker resources --
    std::unique_ptr<double[]> noise_data;
    std::vector<NoiseWorker *> worker_ptrs;

    bool worker_running;
    std::vector<int> worker_status;
    int cur_width;
    int cur_height;
    int cur_num_workers;
    int cur_seed;
    // ----------------------

    // hard coded for now
    void initParams();
    void clearParams();
    void paintNow();

public:
    // Status related
    NoisePanel(wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr);

    ~NoisePanel();

    // Window related
    void paintEvent(wxPaintEvent & evt);
    void OnSize(wxSizeEvent& event);
    void render(wxDC& dc);

    void changeGenerator(std::string gen_new);
    inline const std::set<std::string> &getAvailParams() {
        return cur_avail_params;
    }

    inline const wxAny &getCurVal(const std::string &key) {
        return cur_param_dict[key];
    }

    void start();
    void setCurVal(const std::string &key, wxAny value);

    void onWorkerCompletion(wxCommandEvent &evt);

    inline bool isImageAvailable() {
        return image_available;
    }
    void saveTo(const wxString& path);

    DECLARE_EVENT_TABLE();
};

