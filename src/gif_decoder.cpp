#include "../include/gif_decoder.hpp"

#include <algorithm>
#include <iostream>

namespace KonstantIMP {

std::vector<rgba_color> gif_decoder::decode(gif_frame & frame, std::vector<std::unique_ptr<rgba_color> > & gct, const bool & debug) {
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

    if(debug) std::clog << "[DEBUG] Splitting data into blocks of code size bits\n";

    std::vector<std::uint16_t> lwz_codes {};

    std::size_t read_bits {0}, read_codes{0}, code_size {frame.get_lwz_code_size()};

    while(read_bits / 8 < frame.get_lwz_data_ref().size()) {
        std::uint32_t tmp_data_block {0x00000000}, mask{0x00000000};

        tmp_data_block = static_cast<std::uint32_t>(static_cast<std::uint32_t>(frame.get_lwz_data_ref().at(read_bits / 8)) & 0xff);
        if((read_bits / 8) + 1 < frame.get_lwz_data_ref().size())
            tmp_data_block = tmp_data_block | static_cast<std::uint32_t>((static_cast<std::uint32_t>(frame.get_lwz_data_ref().at((read_bits / 8) + 1)) & 0xff) << 8);
        if((read_bits / 8) + 2 < frame.get_lwz_data_ref().size())
            tmp_data_block = tmp_data_block | static_cast<std::uint32_t>((static_cast<std::uint32_t>(frame.get_lwz_data_ref().at((read_bits / 8) + 2)) & 0xff) << 16);
        if((read_bits / 8) + 3 < frame.get_lwz_data_ref().size())
            tmp_data_block = tmp_data_block | static_cast<std::uint32_t>((static_cast<std::uint32_t>(frame.get_lwz_data_ref().at((read_bits / 8) + 3)) & 0xff) << 24);

        tmp_data_block = tmp_data_block >> (read_bits % 8);

        for(std::size_t i{0}; i < code_size; i++) mask = (mask << 1) + 1;

        lwz_codes.push_back(static_cast<std::uint16_t>(tmp_data_block & mask)); read_bits += code_size; read_codes++;

        /*std::uint16_t tmp_data_block { 0x0000 }, mask( 0x0000 );
        tmp_data_block = static_cast<std::uint16_t>(frame.get_lwz_data_ref().at(read_bits / 8));
        if(read_bits / 8 + 1 != frame.get_lwz_data_ref().size())
            tmp_data_block = (static_cast<std::uint16_t>(frame.get_lwz_data_ref().at(read_bits / 8 + 1)) << 8) | tmp_data_block;

        tmp_data_block = tmp_data_block >> (read_bits % 8);

        for(std::size_t i{0}; i < code_size; i++) mask = (mask << 1) + 1;

        lwz_codes.push_back(tmp_data_block & mask); read_bits += code_size; read_codes++;*/

        for(std::size_t i{3}; i < 100000; i++) {
            if(lwz_table.size() + read_codes - 2 == std::pow(2, i)) {
                code_size = i + 1; break;
            }
            if(lwz_table.size() + read_codes - 2 < std::pow(2, i)) break;
        }

        /*if(lwz_table.size() + read_codes == std::pow(2, 3) + 2) code_size = 4;
        if(lwz_table.size() + read_codes == std::pow(2, 4) + 2) code_size = 5;
        if(lwz_table.size() + read_codes == std::pow(2, 5) + 2) code_size = 6;
        if(lwz_table.size() + read_codes == std::pow(2, 6) + 2) code_size = 7;
        if(lwz_table.size() + read_codes == std::pow(2, 7) + 2) code_size = 8;
        if(lwz_table.size() + read_codes == std::pow(2, 8) + 2) code_size = 9;
        if(lwz_table.size() + read_codes == std::pow(2, 9) + 2) code_size = 10;*/
    }

    if(debug) {
        std::clog << "\n[DEBUG] LWZ codes (" << lwz_codes.size() << " items)\n\t";

        std::size_t i{0};
        for(auto & iter : lwz_codes) {
            if(i % 16 == 0 && i) std::clog << "\n\t";

            std::clog << (iter < 16 ? " 0" : " ") << std::hex << iter; i++;
        } std::clog << '\n' << std::dec << '\n';
    }

    if(debug) std::clog << "[DEBUG] Decoding data\n";

    if(lwz_codes.at(0) == cc) {
        lwz_codes.erase(lwz_codes.begin());
        if (debug) std::cout << "\tFirst data code : " << static_cast<std::uint16_t>(cc) << " (CLEAR CODE)\n\n";
    }
    else throw std::runtime_error("Incorrect LWZ codes (It must start from " + std::to_string(cc) + ")");

    if(lwz_codes.at(lwz_codes.size() - 1) == ec) {
        if (debug) std::cout << "\tLast data code : " << static_cast<std::uint16_t>(ec) << " (END CODE)\n\n";
        lwz_codes.erase(--lwz_codes.end());
    }
    else  if(lwz_codes.at(lwz_codes.size() - 2) == ec) {
        if (debug) std::cout << "\tLast data code : " << static_cast<std::uint16_t>(ec) << " (END CODE)\n\n";
        lwz_codes.erase(--lwz_codes.end()); lwz_codes.erase(--lwz_codes.end());
    }
    else throw std::runtime_error("Incorrect LWZ codes (It must end to " + std::to_string(ec) + ")");



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

    if(debug) std::clog << "[DEBUG] Table done\n\n[DEBUG] Creating image array\n";

    for(std::size_t i{0}; i < lwz_codes.size(); i++) {
        auto data_vec = lwz_table[lwz_codes.at(i)];

        //if(debug) std::clog << "\t" << i << " - LWZ code\n";

        for(std::size_t j{0}; j < data_vec.size(); j++) {
            if(frame.get_frame_d().use_lct) {
                pixel_list.push_back(*frame.get_lct_ref().at(data_vec.at(j)).get());
            }
            else {
                pixel_list.push_back(*gct.at(data_vec.at(j)).get());
            }
        }
    }

    if(debug) std::clog << "\n[DEBUG] Done\n";

    return pixel_list;
}

}
