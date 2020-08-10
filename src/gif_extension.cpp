#include "../include/gif_decoder.hpp"

namespace KonstantIMP {

graphic_extension::graphic_extension(const gif_extension * gif_parent) : gif_extension(),
    dis_met(NO_DISPOSAL), user_input(false), transparency(true), delay(10), transparency_index(0) {
    //// Setting extension type
    ext_type = GRAPHIC_EXTENSION;

    //// Type checking
    if(gif_parent->get_extension_type() != GRAPHIC_EXTENSION)
        throw std::runtime_error("Parent objects type is not GRAPHIC_EXTENSION");

    //// Getting data from parent
    std::string tmp_data = gif_parent->get_data();

    //// Setting disposal method
    dis_met = static_cast<DISPOSAL_METHOD>(std::stoi(tmp_data.substr(0, 1)));

    //// Setting user input flag
    user_input = static_cast<bool>(std::stoi(tmp_data.substr(1, 1)));

    //// Setting transparency flag
    transparency = static_cast<bool>(std::stoi(tmp_data.substr(2, 1)));

    //// Setting delay
    delay = static_cast<std::uint8_t>(std::stoi(tmp_data.substr(3, 3)));

    //// Setting transparency index
    transparency_index = static_cast<std::uint8_t>(std::stoi(tmp_data.substr(6)));
}

graphic_extension::~graphic_extension() {
    dis_met = NO_DISPOSAL; user_input = false;
    transparency = true; delay = 10;
    transparency_index = 0;
}

void graphic_extension::read_data(std::ifstream & fin_gif) {
    //// Creating temp
    char temp_data = 0x00;

    //// Extension markup checking
    fin_gif.read(&temp_data, 1);
    if(temp_data != 4) throw std::runtime_error("Graphic extension block size must be 4 byres (incorrect .gif)");
    temp_data = 0x00;

    //// Getting data from <Packed Fields>
    fin_gif.read(&temp_data, 1);
    user_input = static_cast<bool>(temp_data & 0x00000010);
    transparency = static_cast<bool>(temp_data & 0x00000001);
    dis_met = static_cast<DISPOSAL_METHOD>((temp_data & 0x00011100) >> 2);

    //// Gettong delay time
    fin_gif.read(&temp_data, 1);
    delay = static_cast<std::uint8_t>(temp_data);

    //// Getting transparency color index
    fin_gif.read(&temp_data, 1);
    transparency_index = static_cast<std::uint8_t>(temp_data);

    //// End of block
    fin_gif.read(&temp_data, 1);
    if(temp_data != 0x00) throw std::runtime_error("Extension block must end zero-byte(0x00) (incorrect .gif)");
}

std::string graphic_extension::get_data() const {
    //// Resulted str
    std::string result = "";

    //// Adding delay to data
    result = std::to_string(delay);
    while (result.length() < 3) result = '0' + result;

    //// Adding transparency flag
    if(transparency) result = '1' + result;
    else result = '0' + result;

    //// Adding user input flag
    if(user_input) result = '1' + result;
    else result = '0' + result;

    //// Adding disposal method info
    result = std::to_string(static_cast<std::uint8_t>(dis_met)) + result;

    //// Adding delay info
    result += std::to_string(delay);

    return result;
}

comment_extension::comment_extension(const gif_extension * gif_parent) : gif_extension(), comment_msg("") {
    //// Setting extension type
    ext_type = COMMENT_EXTENSION;

    //// Type checking
    if(gif_parent->get_extension_type() != COMMENT_EXTENSION)
        throw std::runtime_error("Parent objects type is not COMMENT_EXTENSION");
    //// Data setting
    comment_msg = gif_parent->get_data();
}

void comment_extension::read_data(std::ifstream & fin_gif) {
    //// Temp character for data reading
    char character = 0;

    //// Read data before 0x00 byte
    while (1) {
        fin_gif.read(&character, 1);

        if(character == 0x00) return;
        comment_msg += character;
    }
}

};
