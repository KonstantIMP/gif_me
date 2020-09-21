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
        lwz_table[cc] = std::vector<std::uint16_t> {};
        lwz_table[ec] = std::vector<std::uint16_t> {};

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

    if(debug) std::clog << "[DEBUG] Splitting data into blocks of " << static_cast<std::uint16_t>(frame.get_lwz_code_size()) << " bits\n";

    std::vector<std::uint16_t> lwz_codes {};



    if(frame.get_lwz_code_size() == 3) {

    }
    else if(frame.get_lwz_code_size() == 4) {
        for(auto & iter : frame.get_lwz_data_ref()) {
            if(debug) std::clog << "\tFrom" << (iter < 16 ? " 0" : " ") << std::hex << static_cast<std::uint16_t>(iter) << " to " <<
                                   static_cast<std::uint16_t>(static_cast<std::uint8_t>(iter) & 0b00001111) << " and " <<
                                    static_cast<std::uint16_t>((static_cast<std::uint8_t>(iter) & 0b11110000) >> 4) << '\n';

            lwz_codes.push_back(static_cast<std::uint16_t>(static_cast<std::uint8_t>(iter) & 0b00001111));
            lwz_codes.push_back(static_cast<std::uint16_t>((static_cast<std::uint8_t>(iter) & 0b11110000) >> 4));
        }

        if(debug) std::clog << std::dec;
    }
    else if(frame.get_lwz_code_size() == 5) {

    }
    else if(frame.get_lwz_code_size() == 6) {

    }
    else if(frame.get_lwz_code_size() == 7) {

    }
    else if(frame.get_lwz_code_size() == 8) {
        if(debug) std::clog << "\tCoping data (size is equal)\n\t";

        std::size_t i {0};

        for(auto & iter : frame.get_lwz_data_ref()) {
            i++; if(i % 16 == 0 && debug) std::clog << "\n\t";

            if(debug) std::clog << std::hex << (iter < 16 ? " 0" : " ") << static_cast<std::uint16_t>(iter);

            lwz_codes.push_back(static_cast<std::uint16_t>(iter));
        }

        if(debug) std::clog << std::dec;
    }
    else {

    }



    if(debug) {
        std::clog << "\n[DEBUG] LWZ codes\n\t";

        std::size_t i{0};
        for(auto & iter : lwz_codes) {
            if(i % 16 == 0 && i) std::clog << "\n\t";

            std::clog << (iter < 16 ? " 0" : " ") << std::hex << iter; i++;
        } std::clog << '\n' << std::dec << '\n';
    }

    if(debug) std::clog << "[DEBUG] Decoding data\n";

    if(lwz_codes.at(0) == cc) {
        if (debug) std::cout << "\tFirst data code : " << static_cast<std::uint16_t>(cc) << " (CLEAR CODE)\n\n";
    }
    else throw std::runtime_error("Incorrect GIFs lwz data block");

   /* std::uint16_t next_index = ec + 1;
    std::uint16_t last_code = lwz_codes.at(1);

    for(std::size_t i{1}; i < lwz_codes.size(); i++) {
        if(lwz_table.find(lwz_codes.at(i)) == lwz_table.end()) {
            if(debug) std::clog << '\t' << static_cast<std::uint16_t>(lwz_codes.at(i)) << " not found in the table\n";
            break;
        }
        else {
            if(debug) std::clog << "\tThe " << static_cast<std::uint16_t>(lwz_codes.at(i)) << " found in the table\n";

            std::uint16_t k_data = lwz_table.find(lwz_codes.at(i))->second.at(0);
            if(debug) std::clog << "\tFirst code at index " << lwz_codes.at(i) << " is : " << k_data << '\n';

            std::vector<std::uint16_t> f_data = lwz_table.find(last_code)->second;
            if(debug) {
                std::clog << "\tLast code is : " << last_code << '\n';
                std::clog << "\tData at " << last_code << " index is :\n\t\t";

                for(auto & iter : f_data) std::clog << iter << ' ';

                std::clog << '\n';
            }

            f_data.push_back(k_data); lwz_table[next_index] = f_data;
            next_index++; last_code = lwz_codes.at(i);

            if(debug) {
                std::clog << "\tAdded LWZ data to dict is :\n\t\t";
                for(auto & iter : f_data) std::clog << iter << ' ';

                std::clog << "\n\n";
            }
        }
    }*/

    //std::uint16_t last_code = lwz_codes.at(0);

    lwz_codes.erase(lwz_codes.begin());

    for(std::size_t i{1}; i < lwz_codes.size(); i++) {
        if(lwz_table.find(lwz_codes.at(i)) != lwz_table.end()) {
            if(debug) std::clog << "\tData with index " << lwz_codes.at(i) << " found\n";

            std::uint16_t k_data = lwz_table[lwz_codes.at(i)].at(0);
            if(debug) std::clog << "\tFirst LWZ code at this index is : " << k_data << '\n';

            std::vector<std::uint16_t> f_data = lwz_table[lwz_codes.at(i - 1)];
            f_data.push_back(k_data); lwz_table[lwz_table.size()] = f_data;

            if(debug) {
                std::clog << "\tAdded new record to dict at " << lwz_table.size() - 1 << " index :\n\t";

                for(auto & iter : f_data) std::clog << iter << ' ';

                std::clog << "\n\n";
            }
        }
        else {
            if(debug) std::clog << "\tData with index " << lwz_codes.at(i) << " not found\n";

            std::vector<std::uint16_t> f_data = lwz_table[lwz_codes.at(i - 1)];
            f_data.push_back(f_data.at(0));

            lwz_table[lwz_table.size()] = f_data;

            if(debug) {
                std::clog << "\tAdded new record to dict at " << lwz_table.size() - 1 << " index :\n\t";

                for(auto & iter : f_data) std::clog << iter << ' ';

                std::clog << "\n\n";
            }
        }
    }

    return std::vector<std::vector<rgba_color>>();
}

}
