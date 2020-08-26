#include "../include/gif_decoder.hpp"

namespace KonstantIMP {

gif_frame::gif_frame() {
    std::memset(&frame_d, 0, sizeof (frame_descriptor));
    lwz_code_size = 0;  lwz_data.clear();
}

gif_frame::~gif_frame() {
    std::memset(&frame_d, 0, sizeof (frame_descriptor));
    lwz_code_size = 0;  lwz_data.clear();
}

void gif_frame::read_data(std::ifstream & fin) {
    char data[3]; std::memset(data, 0, 3);

    fin.read(data, 2);
    frame_d.left = (static_cast<std::uint16_t>(data[1]) << 8) | static_cast<std::uint16_t>(data[0]);

    fin.read(data, 2);
    frame_d.top = (static_cast<std::uint16_t>(data[1]) << 8) | static_cast<std::uint16_t>(data[0]);

    fin.read(data, 2);
    frame_d.width = (static_cast<std::uint16_t>(data[1]) << 8) | static_cast<std::uint16_t>(data[0]);

    fin.read(data, 2);
    frame_d.height = (static_cast<std::uint16_t>(data[1]) << 8) | static_cast<std::uint16_t>(data[0]);

    std::memset(data, 0, 3); fin.read(data, 1);
    frame_d.use_lct = static_cast<bool>(0b10000000 & data[0]);
    frame_d.sort_flag = static_cast<bool>(0b00100000 & data[0]);
    frame_d.is_interlaced = static_cast<bool>(0b01000000 & data[0]);
    frame_d.lct_size = static_cast<std::uint8_t>(0b00000111 & data[0]);

    std::memset(data, 0, 3); fin.read(data, 1);
    lwz_code_size = static_cast<std::size_t>(data[0]);

    std::memset(data, 0, 3); fin.read(data, 1);
    for(std::size_t i{0}; i < static_cast<std::size_t>(data[0]); i++) {
        std::memset(data, 0, 3); fin.read(data, 1);
        lwz_data.push_back(data[0]);
    }

    std::memset(data, 0, 3); fin.read(data, 1);
    if(data[0] != 0x00) throw std::runtime_error("Incorrect frame block (It must end by 0x00)");
}

}
