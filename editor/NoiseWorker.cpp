#include <NoiseWorker.h>

void NoiseWorker::doFinishNotify() {
    wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, evid );
    event.SetInt(cur_part);  // notify part number
    parent->GetEventHandler()->AddPendingEvent( event );
}   

NoiseWorker::NoiseWorker(wxPanel* parent
    , int evfinish_id
    , int cur_part
    , int total_part
    , int seed
    , std::string generator_type
    , std::map<std::string, wxAny> cur_param_dict
    , std::set<std::string> cur_avail_params
    , double *noise_data)
        : parent(parent), evid(evfinish_id)
        , cur_part(cur_part), total_part(total_part)
        , seed(seed), generator_type(generator_type)
        , cur_param_dict(cur_param_dict)
        , cur_avail_params(cur_avail_params)
        , noise_data(noise_data) {
    width = cur_param_dict["width"].As<int>();
    height = cur_param_dict["height"].As<int>();
}

wxThread::ExitCode NoiseWorker::Entry() {

    if (generator_type == "Uniform") {
        invokeGenerator<UniformNoiseGenerator>();
    } else if (generator_type == "Perlin") {
        invokeGenerator<PerlinNoiseGenerator2D>(
            cur_param_dict["grid_max"].As<int>());
    } else if (generator_type == "Worley") {
        invokeGenerator<WorleyNoiseGenerator>(
            cur_param_dict["grid_max"].As<int>(),
            cur_param_dict["prob_next"].As<double>(),
            cur_param_dict["grid_point_max"].As<int>()
        );
    } else if (generator_type == "Fractual-Perlin") {
        invokeGenerator<FractualNoiseGenerator2D<PerlinNoiseGenerator2D>>(
            cur_param_dict["level"].As<int>(),
            cur_param_dict["grid_max"].As<double>()
        );
    } else if (generator_type == "Composite-FP") {
        invokeGenerator<CompositeGenerator
                            <FractualNoiseGenerator2D
                                <PerlinNoiseGenerator2D>>>(
            cur_param_dict["total"].As<int>(),
            cur_param_dict["level"].As<int>(),
            cur_param_dict["grid_max"].As<double>()
        );
    } else {
        // pass
    }
    wxLogDebug("Thread %d finished", this->cur_part);
    doFinishNotify();
    return 0;
}