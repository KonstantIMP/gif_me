#ifndef  GIF_PARSER_HPP
#define  GIF_PARSER_HPP

/**
 * @file gif_decoder.hpp
 * @brief This file contaons gif_decoder class and its dependences
 * @author KonstantIMP
 */

#include "gif_defines.hpp"

#include <gif_extension.hpp>
#include "gif_frame.hpp"

namespace KonstantIMP {

/**
 * @brief The gif_header struct
 *
 * This struct contains information which exist in GIFs header
 */
struct gif_header {
    /**
     * @brief version
     *
     * Theis parametr contains gif format version. It calculated from first 6 bytes of file
     */
    GIF_VERSION version = NOT_GIF;

    /**
     * @brief gif_width
     *
     * gif_width contains gif resolution width and is calculated from next 4 bytes (reverse)
     */
    std::uint16_t gif_width = 0;
    /**
     * @brief gif_height
     *
     * gif_height contains gif resolution height and is calculated from next 4 bytes (reverse)
     */
    std::uint16_t gif_height = 0;

    /**
     * @brief gct_flag
     *
     * This parameter indicates the presence of gct in the file. Calculated from next bit
     */
    bool gct_flag = true;
    /**
     * @brief color_resolution
     *
     * Contains the number of bits of color resolution. Calculated from next 3 bits plus 1 (as number)
     */
    std::uint8_t color_resolution = 0;

    /**
     * @brief sort_flag
     *
     * If it is TRUE, colors in gct sorted by important less. Calculated from next bit
     */
    bool sort_flag = false;
    /**
     * @brief gct_size
     *
     * Contains gct size. Calculated from next 3 bits (N) by gct_size = 2 ^ (M + 1)
     */
    std::size_t gct_size = 0;

    /**
     * @brief back_color
     *
     * It is background color INDEX. Calculated from next byte
     */
    std::uint8_t back_color = 0;
    /**
     * @brief aspect_ratio
     *
     * Images aspect ration. Calculated from next byte
     */
    std::uint8_t aspect_ratio = 0;
};

/**
 * @brief The gif_decoder class
 *
 * Need for simple gif decoding
 */
class gif_parser {
public:
    /**
     * @brief gif_decoder
     *
     * Standart constructor
     *
     * @param[in] file_name optional gif path(Call open method)
     */
    gif_parser(const std::string & file_name = "") {
        this->clear();
        if(file_name != "") open(file_name);
    }

    /**
     * @brief ~gif_decoder
     *
     * Clear data before using (allocate memory)
     */
    ~gif_parser() {clear();};

    /**
     * @brief open
     *
     * Open gif for decoding
     *
     * @param[in] file_name PATH to decoded gif file
     */
    void open(const std::string & file_name) {
        if(gif.is_open()) throw std::runtime_error("GIF already has opened");
        gif.open(file_name, std::ios_base::in | std::ios_base::binary);
        if(!gif.is_open()) throw std::runtime_error("Can\'t open GIF");
    }

    /**
     * @brief decode
     *
     * Starts decoding process
     *
     * @param[in] debug See debug output (std::clog)
     */
    void parse(const bool & debug = false);

    /**
     * @brief clear
     *
     * Clearing data after decoding
     */
    void clear() {
        if(gif.is_open()) gif.close();
        std::memset(&head, 0, sizeof (gif_header));
        extensions.clear();
        frames.clear();
        gct.clear();
    };

    /**
     * @brief get_extensions_ref
     *
     * Extensions vector getter
     *
     * @return Ref to extensions vector
     */
    inline std::vector<std::unique_ptr<gif_extension>> & get_extensions_ref() const {
        return const_cast<std::vector<std::unique_ptr<gif_extension>> &>(extensions);
    }

    /**
     * @brief get_frames_ref
     *
     * Frames vector getter
     *
     * @return Ref to frames vectore
     */
    inline std::vector<std::unique_ptr<gif_frame>> & get_frames_ref() const {
        return const_cast<std::vector<std::unique_ptr<gif_frame>> &>(frames);
    }

    /**
     * @brief get_gtc_ref
     *
     * GCT vector getter
     *
     * @return Ref to GCT vector
     */
    inline std::vector<std::unique_ptr<rgba_color>> & get_gtc_ref() const {
        return const_cast<std::vector<std::unique_ptr<rgba_color>> &>(gct);
    }

    /**
     * @brief get_gif_header
     *
     * Gif header getter
     *
     * @return gif header struct
     */
    inline gif_header get_gif_header() const {
        return head;
    }

private:
    //// Extensions pointer vector
    std::vector<std::unique_ptr<gif_extension>> extensions;
    //// Frames pointer vector
    std::vector<std::unique_ptr<gif_frame>> frames;

    //// GlobalColorTable (rgba)
    std::vector<std::unique_ptr<rgba_color>> gct;

    //// File reading object
    std::ifstream gif;

    //// GIF Header(RFC)
    gif_header head;
};

}

#endif // GIF_DECODER_HPP
