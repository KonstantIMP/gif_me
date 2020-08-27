#include "../include/gif_decoder.hpp"

#include <iostream>

namespace KonstantIMP {

gif_frame::gif_frame() {
    std::memset(&frame_d, 0, sizeof (frame_descriptor));
    lwz_code_size = 0;  lwz_data.clear();
    lct.clear();
}

gif_frame::~gif_frame() {
    std::memset(&frame_d, 0, sizeof (frame_descriptor));
    lwz_code_size = 0;  lwz_data.clear();
    lct.clear();
}

void gif_frame::read_data(std::ifstream & fin) {
    char data[3]; std::memset(data, 0, 3);

    fin.read(data, 2);
    frame_d.left = static_cast<std::uint16_t>(((static_cast<std::uint16_t>(data[1]) & 0x00ff) << 8) | (static_cast<std::uint16_t>(data[0]) & 0x00ff));

    fin.read(data, 2);
    frame_d.top = static_cast<std::uint16_t>(((static_cast<std::uint16_t>(data[1]) & 0x00ff) << 8) | (static_cast<std::uint16_t>(data[0]) & 0x00ff));

    fin.read(data, 2);
    frame_d.width = static_cast<std::uint16_t>(((static_cast<std::uint16_t>(data[1]) & 0x00ff) << 8) | (static_cast<std::uint16_t>(data[0]) & 0x00ff));

    fin.read(data, 2);
    frame_d.height = static_cast<std::uint16_t>(((static_cast<std::uint16_t>(data[1]) & 0x00ff) << 8) | (static_cast<std::uint16_t>(data[0]) & 0x00ff));

    std::memset(data, 0, 3); fin.read(data, 1);
    frame_d.use_lct = static_cast<bool>(0b10000000 & data[0]);
    frame_d.sort_flag = static_cast<bool>(0b00100000 & data[0]);
    frame_d.is_interlaced = static_cast<bool>(0b01000000 & data[0]);
    frame_d.lct_size = static_cast<std::uint8_t>(0b00000111 & data[0]);

    for(std::size_t i{0}; i < static_cast<std::size_t>(frame_d.lct_size); i++) {
        rgba_color * tmp = new rgba_color;
        char data[4]; std::memset(data, 0, 3);

        fin.read(data, 3);
        tmp->r = static_cast<std::uint8_t>(data[0]);
        tmp->g = static_cast<std::uint8_t>(data[1]);
        tmp->b = static_cast<std::uint8_t>(data[2]);

        lct.push_back(std::unique_ptr<rgba_color>(tmp));
    }

    std::memset(data, 0, 3); fin.read(data, 1);
    lwz_code_size = static_cast<std::size_t>(data[0]) + 1;

    while (1) {
        std::memset(data, 0, 3); fin.read(data, 1);
        std::size_t b_size = static_cast<std::size_t>(static_cast<std::uint8_t>(data[0]));

        if(b_size == 0) break;

        for(std::size_t i{0}; i < b_size; i++) {
            std::memset(data, 0, 3); fin.read(data, 1);
            lwz_data.push_back(data[0]);
        }
    }
}

}
