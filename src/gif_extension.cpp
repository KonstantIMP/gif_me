#include "../include/gif_decoder.hpp"

namespace KonstantIMP {

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
