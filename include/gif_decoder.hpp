#ifndef GIF_DECODER_HPP
#define GIF_DECODER_HPP

#include "gif_defines.hpp"

#include "gif_frame.hpp"

namespace KonstantIMP {

/**
 * @brief The gif_decoder class
 *
 * Class for decoding gif_frame
 */
class gif_decoder {
public:
    /**
     * @brief decode
     *
     * Static function to decode frame
     *
     * @param[in] frame to decode
     * @param[in] gct GlobalColorTable for decoding
     * @param{in} debug See debug output
     * @return Already decoded frame (array from array from rgba_color)
     */
    static std::vector<std::vector<rgba_color>> decode(gif_frame & frame, std::vector<std::unique_ptr<rgba_color>> & gct, const bool & debug = false);
};

}

#endif // GIF_DECODER_HPP
