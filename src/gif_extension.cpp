#include "../include/gif_decoder.hpp"

namespace KonstantIMP {

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
