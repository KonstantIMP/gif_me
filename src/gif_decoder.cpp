#include "../include/gif_decoder.hpp"

#include <iostream>

namespace KonstantIMP {

std::vector<std::vector<rgba_color> > gif_decoder::decode(gif_frame & frame, std::vector<std::unique_ptr<rgba_color> > & gct, const bool & debug) {
    if(debug) std::clog << "[DEBUG] Frame decoding\n\n";

    std::map<std::uint16_t, std::vector<std::uint16_t>> lwz_table;

    std::uint16_t cc = 0x00, ec = 0x00;

    if(frame.get_frame_d().use_lct) {
        if(debug) std::clog << "[DEBUG] Using LocalColorTable\n";

        for(std::size_t i{0}; i < frame.get_frame_d().lct_size; i++) {
            lwz_table[static_cast<std::uint16_t>(i)] = std::vector<std::uint16_t> {static_cast<std::uint16_t>(i)};
        }
        cc = static_cast<std::uint16_t>(frame.get_frame_d().lct_size); ec = cc + 1;
        lwz_table[cc] = std::vector<std::uint16_t> {cc};
        lwz_table[ec] = std::vector<std::uint16_t> {ec};

        if(debug) {
            std::clog << "\tLCT size is : " << frame.get_frame_d().lct_size << '\n';
            std::clog << "\tClearCode index : " << static_cast<std::uint16_t>(cc) << '\n';
            std::clog << "\tEndOfData index : " << static_cast<std::uint16_t>(ec) << "\n\n";
        }
    }
    else {
        if(debug) std::clog << "[DEBUG] Using GlobalColorTable\n";

        for(std::size_t i{0}; i < gct.size(); i++) {
            lwz_table[static_cast<std::uint16_t>(i)] = std::vector<std::uint16_t> {static_cast<std::uint16_t>(i)};
        }
        cc = static_cast<std::uint16_t>(gct.size()); ec = cc + 1;
        lwz_table[cc] = std::vector<std::uint16_t> {cc};
        lwz_table[ec] = std::vector<std::uint16_t> {ec};

        if(debug) {
            std::clog << "\tGCT size is : " << gct.size() << '\n';
            std::clog << "\tClearCode index : " << static_cast<std::uint16_t>(cc) << '\n';
            std::clog << "\tEndOfData index : " << static_cast<std::uint16_t>(ec) << "\n\n";
        }
    }

    std::vector<rgba_color> pixel_list {};
    std::uint16_t next_code = ec + 1;
    std::uint16_t cur_code = 0;


    return std::vector<std::vector<rgba_color>>();
}

}
