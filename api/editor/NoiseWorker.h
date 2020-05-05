#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <generators/Generators.h>
#include <map>
#include <set>

class NoiseWorker : public wxThread {
    wxPanel* parent;
    int evid;
    int cur_part;
    int total_part;
    int seed;

    std::string generator_type;
    std::map<std::string, wxAny> cur_param_dict;
    std::set<std::string> cur_avail_params;
    double* noise_data;

    int width;
    int height;

private:
    void doFinishNotify();

public:
    // when finish, send a wxEVT_COMMAND_TEXT_UPDATED with evfinishid
    // wxEVT_COMMAND_TEXT_UPDATED is just a lazy decision (orz)
    NoiseWorker(wxPanel* parent
        , int evfinish_id
        , int cur_part
        , int total_part
        , int seed
        , std::string generator_type
        , std::map<std::string, wxAny> cur_param_dict
        , std::set<std::string> cur_avail_params
        , double *noise_data);

    template<typename T, typename... U>
    void invokeGenerator(U&&... u) {
        T* generator = new T(std::forward<U>(u)...);

        // p'th part: 
        // - [ p * std::floor(total_pixel / total_part)
        //     , (p+1) * std::floor(total_pixel / total_part) )
        // if p is the last, then to the end

        int total_pixel = width * height;
        int sector_len = (int)std::floor((double)total_pixel/total_part);

        int start_pix = cur_part * sector_len;
        int end_pix = (cur_part == total_part - 1) ? total_pixel
                                                   : start_pix + sector_len;

        // scan horizontally (height in inner cycle)
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                int cur_index = i * height + j;
                if (cur_index < start_pix || cur_index >= end_pix) {
                    continue;
                }
                noise_data[cur_index] = generator->get(i, j);
            }
        }

        delete generator;
    }

    virtual ExitCode Entry();
};