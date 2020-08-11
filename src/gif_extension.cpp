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

plaintext_extension::plaintext_extension(const gif_extension * gif_parent) : gif_extension(), left_pos(0), top_pos(0),
    grid_width(0), grid_height(0), cell_width(0), cell_height(0), color_index(0), background_index(0), text_data("") {
    //// Setting extension type
    ext_type = PLAITEXT_EXTENSION;

    //// Checking parent type
    if(gif_parent->get_extension_type() != PLAITEXT_EXTENSION)
        throw std::runtime_error("Parent objects type is not PLAINTEXT_EXTENSION");

    //// Getting data from parent
    std::string tmp_data = gif_parent->get_data();

    //// Setting left_pos
    left_pos = std::stoi(tmp_data.substr(0, 3));

    //// Setting top_pos
    top_pos = std::stoi(tmp_data.substr(3, 3));

    //// Setting grid_width
    grid_width = std::stoi(tmp_data.substr(6, 3));

    //// Setting grid_height
    grid_height = std::stoi(tmp_data.substr(9, 3));

    //// Setting cell_width
    cell_width = std::stoi(tmp_data.substr(12, 3));

    //// Setting cell_height
    cell_height = std::stoi(tmp_data.substr(15, 3));

    //// Setting color_index
    color_index = std::stoi(tmp_data.substr(18, 3));

    //// Setting background_index
    background_index = std::stoi(tmp_data.substr(21, 3));

    //// Setting text_data
    text_data = tmp_data.substr(24);
}

plaintext_extension::~plaintext_extension() {
    left_pos = top_pos = 0;
    grid_width = grid_height = 0;
    cell_width = cell_height = 0;
    color_index = background_index = 0;

    text_data = "";
}

void plaintext_extension::read_data(std::ifstream & fin_gif) {
    //// Creating temp
    char temp_data = 0x00;

    //// Checking extension data block size
    fin_gif.read(&temp_data, 1);
    if(temp_data != 12) throw std::runtime_error("Data block of PLAINTEXT_EXTENSION mest exist from 12 bytes");
    temp_data = 0x00;

    //// Getting left_pos
    fin_gif.read(&temp_data, 1);
    left_pos = static_cast<std::uint8_t>(temp_data);

    //// Getting top_pos
    fin_gif.read(&temp_data, 1);
    top_pos = static_cast<std::uint8_t>(temp_data);

    //// Getting grid_wigth
    fin_gif.read(&temp_data, 1);
    grid_width = static_cast<std::uint8_t>(temp_data);

    //// Getting grid_height
    fin_gif.read(&temp_data, 1);
    grid_height = static_cast<std::uint8_t>(temp_data);

    //// Getting cell_wigth
    fin_gif.read(&temp_data, 1);
    cell_width = static_cast<std::uint8_t>(temp_data);

    //// Getting cell_height
    fin_gif.read(&temp_data, 1);
    cell_height = static_cast<std::uint8_t>(temp_data);

    //// Getting color_index
    fin_gif.read(&temp_data, 1);
    color_index = static_cast<std::uint8_t>(temp_data);

    //// Getting background_index
    fin_gif.read(&temp_data, 1);
    background_index = static_cast<std::uint8_t>(temp_data);

    //// Text_data zero
    text_data = "";

    //// Reading text_data
    while (1) {
        fin_gif.read(&temp_data, 1);
        if(temp_data == 0x00) break;

        text_data += temp_data;
    }
}

std::string plaintext_extension::get_data() const {
    //// Data string creating
    std::string result = "";

    //// Adding background_index infor
    result = std::to_string(background_index);
    while(result.length() < 3) result = '0' + result;

    //// Adding color_index infor
    result = std::to_string(color_index);
    while(result.length() < 6) result = '0' + result;

    //// Adding cell_height infor
    result = std::to_string(cell_height);
    while(result.length() < 9) result = '0' + result;

    //// Adding cell_width infor
    result = std::to_string(cell_width);
    while(result.length() < 12) result = '0' + result;

    //// Adding grid_height infor
    result = std::to_string(grid_height);
    while(result.length() < 15) result = '0' + result;

    //// Adding grid_width infor
    result = std::to_string(grid_width);
    while(result.length() < 18) result = '0' + result;

    //// Adding top_pos infor
    result = std::to_string(top_pos);
    while(result.length() < 21) result = '0' + result;

    //// Adding left_pos infor
    result = std::to_string(left_pos);
    while(result.length() < 24) result = '0' + result;

    //// Adding text_data and return info
    return result + text_data;
}

};
