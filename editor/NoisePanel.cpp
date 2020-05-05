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
// Size event
EVT_SIZE(NoisePanel::OnSize)

EVT_COMMAND  (NoisePanel::evfinish_id, wxEVT_COMMAND_TEXT_UPDATED, NoisePanel::onWorkerCompletion)
END_EVENT_TABLE()


NoisePanel::NoisePanel(wxWindow *parent,
            wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name) 
            : wxPanel(parent, winid, pos, size, style, name)
            , worker_running(false)
            , image_available(false)
              
{
    // register event handler

}

NoisePanel::~NoisePanel() {
    // wait until thread terminates execution

}

void NoisePanel::paintEvent(wxPaintEvent& event)
{
    paintNow();
}

// manual update!
void NoisePanel::paintNow() {
    if (image_available) {
        wxPaintDC dc(this);
        this->render(dc);
    }
}

void NoisePanel::OnSize(wxSizeEvent& event) {
    
}

void NoisePanel::render(wxDC &dc) {
    dc.DrawBitmap(image, 0, 0, false);
}

// logic related, hard-wired for now
void NoisePanel::initParams() {
    clearParams();

    // common params
    cur_param_dict.insert(std::make_pair("width", wxAny(1024)));
    cur_param_dict.insert(std::make_pair("height", wxAny(1024)));
    cur_param_dict.insert(std::make_pair("num_workers", wxAny(2)));
    cur_param_dict.insert(std::make_pair("seed", wxAny(0)));

    if (generator_type == "Uniform") {
        // no param
    } else if (generator_type == "Perlin") {
        cur_param_dict.insert(std::make_pair("grid_max", wxAny(50)));
    } else if (generator_type == "Worley") {
        cur_param_dict.insert(std::make_pair("grid_max", wxAny(50)));
        cur_param_dict.insert(std::make_pair("prob_next", wxAny(0.5)));
        cur_param_dict.insert(std::make_pair("grid_point_max", wxAny(5)));
    } else if (generator_type == "Fractual-Perlin") {
        cur_param_dict.insert(std::make_pair("level", wxAny(50)));
        cur_param_dict.insert(std::make_pair("grid_max", wxAny(50)));
    } else if (generator_type == "Composite-FP") {
        cur_param_dict.insert(std::make_pair("total", wxAny(10)));
        cur_param_dict.insert(std::make_pair("level", wxAny(5)));
        cur_param_dict.insert(std::make_pair("grid_max", wxAny(50)));
    } else {
        wxLogError("Unknown noise type. Abort.");
        clearParams();
        return;
    }

    for (auto &p: cur_param_dict) {
        cur_avail_params.insert(p.first);
    }
}

void NoisePanel::clearParams() {
    cur_param_dict.clear();
    cur_avail_params.clear();
}

void NoisePanel::changeGenerator(std::string gen_new) {
    generator_type = gen_new;
    initParams();
}

void NoisePanel::setCurVal(const std::string &key, wxAny value) {
    // see if we have that key
    if (cur_avail_params.find(key) == cur_avail_params.end()) {
        wxLogError("Unknown key. Abort.");
        return;
    }

    auto it = cur_param_dict.find(key);

    if (!value.HasSameType(it->second)) {
        wxLogError("Type mismatch. Abort.");
        return;
    }

    wxLogDebug("Property %s set", key.c_str());
    it->second = value;
}

void NoisePanel::start() {
    if (worker_running) {
        wxLogStatus("Workers are running, please wait!");
        return;
    }

    int width = this->getCurVal("width").As<int>();
    int height = this->getCurVal("height").As<int>();
    int num_workers = this->getCurVal("num_workers").As<int>();
    int seed = this->getCurVal("seed").As<int>();

    cur_width = width;
    cur_height = height;
    cur_num_workers = num_workers;
    cur_seed = seed;

    // allocate buffer, which is guaranteed to exist during running
    noise_data = std::make_unique<double[]>(cur_width * cur_height);
    memset(noise_data.get(), 0, sizeof(double) * cur_width * cur_height);

    // status mgmt
    worker_running = true;
    image_available = false;
    worker_status.clear();
    worker_status.reserve(num_workers);
    for (int i = 0; i < num_workers; i++) {
        worker_status.push_back(1);
    }

    // initialize workers
    worker_ptrs.clear();
    worker_ptrs.reserve(num_workers);
    for (int i = 0; i < num_workers; i++) {
        NoiseWorker *new_worker = 
            new NoiseWorker(this, evfinish_id, i, num_workers, seed
            , generator_type, cur_param_dict, cur_avail_params, noise_data.get());
        worker_ptrs.push_back(new_worker);


    }

    // run all workers
    for (int i = 0; i < num_workers; i++) {
        worker_ptrs[i]->Run();
    }
    wxLogStatus("Noise generation started. (%d workers)", num_workers);

}

void NoisePanel::onWorkerCompletion(wxCommandEvent &evt) {
    int num_received = evt.GetInt();
    if (num_received >= cur_num_workers) {
        wxLogError("Receiving illegal completion signal!");
    }

    if (worker_status[num_received] == 0) {
        wxLogError("Duplicate completion signal received!");
    } else {
        worker_status[num_received] = 0;
        wxLogStatus("Finished thread %d", num_received);
    }

    bool completion = true;
    for (auto p : worker_status) {
        if (p) {
            completion = false;
        }
    }
    
    if (!completion) {
        return;
    } else {
        worker_running = false;

        // final processing
        // (no need to clear)
        wxImage image_imaged = wxImage(cur_width, cur_height, false);
        wxImagePixelData data(image_imaged);

        if (!data) {
            wxLogError("wxImagePixelData not supported!");
            return;
        }

        wxImagePixelData::Iterator p(data);
        for (int i = 0; i < cur_width; i++) {
            for (int j = 0; j < cur_height; j++) {
                p.MoveTo(data, i, j);
                int intensity = std::clamp((int)(noise_data[i * cur_height + j] * 255), 0, 255);
                p.Red() = intensity;
                p.Green() = intensity;
                p.Blue() = intensity;
            }
        }

        image = wxBitmap(image_imaged);
        image_available = true;

        paintNow();

    }
} 