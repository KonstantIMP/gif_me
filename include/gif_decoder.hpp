#ifndef GIF_DECODER_HPP
#define GIF_DECODER_HPP

/**
 * @file gif_decoder.hpp
 * @brief This file contaons gif_decoder class and its dependences
 * @author KonstantIMP
 */

#include <cstddef>
#include <cstdint>

#include <fstream>

#include <string>

#include <vector>

namespace KonstantIMP {

/// Gif versions enumaration
enum GIF_VERSION : std::uint8_t {
    GIF_87a,    ///< GIF87a type (old)
    GIF_89a,    ///< GIF89a type
    NOT_GIF     ///< File is not a .gif
};

/// Gif block identificators
enum GIF_BLOCK_TYPE : std::uint8_t {
    EXTENSION_BLOCK = 0x21, ///< Block is an extension
    IMAGE_BLOCK     = 0x2c, ///< Block is an image
    GIF_EOF_BLOCK   = 0x3b, ///< Block is the end of the file
    UNDEFINED       = 0x00  ///< Block doesn't exist
};

/// Extension types enumeration
enum GIF_EXTENSION_TYPE : std::uint8_t {
    PLAITEXT_EXTENSION = 0x01,  ///< Extension is a plain text
    GRAPHIC_EXTENSION  = 0xf9,  ///< It is an extension for work with images
    COMMENT_EXTENSION  = 0xfe,  ///< Extension is a Comment
    PROGRAM_EXTENSION  = 0xff,  ///< Extension get an info for an app
    UNREAL_EXTENSION   = 0x00   ///< Undefined extension type
};

/// Disposal method types
enum DISPOSAL_METHOD : std::uint8_t {
    NO_DISPOSAL   = 0,  ///< Don't do anything
    NOT_DISPOS    = 1,  ///< Draw onto another frame
    RESTORE_BACK  = 2,  ///< Restore baskground before drawing
    RESTORE_PREV  = 3,  ///< Restore gif place before drawing
    TO_BE_DEFINED = 4   ///< Not defined for now (by RFC)
};

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
    std::uint8_t gct_size = 0;

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
 * @brief The rgba_color struct
 *
 * Struct to contain color in RGBA format
 */
struct rgba_color {
    /// @brief r Amount of red
    std::uint8_t r = 0;
    /// @brief g Amount of green
    std::uint8_t g = 0;
    /// @brief b Amount of blue
    std::uint8_t b = 0;

    /// @brief a Amount of ALPHA chanell
    double a = 1.f;
};

/**
 * @brief The frame_descriptor struct
 *
 * This sctruct describes frame (Its specifications)
 */
struct frame_descriptor {
    /// @brief left Indent to the left of the frame (in pixels). Calculated from firs 4 bytes (reverse)
    std::uint16_t left = 0;
    /// @brief top Indent to the top of the frame (in pixels). Calculated from next 4 bytes (reverse)
    std::uint16_t top = 0;

    /// @brief width Frame width (in pixels). Calculated from next 4 bytes (reverse)
    std::uint16_t width = 0;
    /// @brief height Frame height (in pixels). Calculated from next 4 bytes (reverse)
    std::uint16_t height = 0;

    /// @brief use_ctl Does the gif use lct. Calculated from next bit
    bool use_lct = false;
    /// @brief is_interlaced Which interlacin method in use. Calculated from next bit
    bool is_interlaced = false;

    /// @brief sort_flag If it enabled colors sorted by important less. Calculated from next bit
    bool sort_flag = false;

    //// Here is 2 reserved bits

    /// @brief lct_size Contains size of lct (if it was enabled)
    std::uint8_t lct_size = 0;
};

/**
 * @brief The gif_extension class
 *
 * Parent (abstract) class for all gif extension (made for simple work with pointers)
 */
class gif_extension {
public:
    /**
     * @brief gif_extension
     *
     * Standart constructor, which set extension type as UNREAL_EXTENSION
     */
    gif_extension() : ext_type(UNREAL_EXTENSION) {};
    /**
     * @brief ~gif_extension
     *
     * Virtual destructor is empty for this class, by need for children
     */
    virtual ~gif_extension() {};

    /**
     * @brief read_data
     *
     * Abstract method to fill extension data from file stream
     *
     * @param[in] fin_gif std::ifstream object for reading from .gif
     *
     * @throw std::runtime_error If file is incorrect
     */
    virtual void read_data(std::ifstream & fin_gif) = 0;

    /**
     * @brief get_data
     *
     * Abstract method to get extensions data
     *
     * @return String with extension data (It will be formated for some types)
     */
    virtual std::string get_data() const = 0;

    /**
     * @brief get_extension_type
     *
     * Method to getting extension type
     *
     * @return Extension type ( GIF_EXTENSION_TYPE element )
     */
    inline GIF_EXTENSION_TYPE get_extension_type() const {
        return ext_type;
    };

protected:
    /// @brief ext_type Contains extension type
    GIF_EXTENSION_TYPE ext_type;
};

/**
 * @brief The graphic_extension class
 *
 * Child class for KonstantIMP::gif_extension for containing graphic control extensions
 */
class graphic_extension : public gif_extension {
    graphic_extension();

    graphic_extension(const gif_extension *);

    virtual ~graphic_extension();

    virtual void read_data(std::ifstream & fin_gif);

    virtual std::string get_data() const;

private:

};

/**
 * @brief The comment_extension class
 *
 * Child class for KonstantIMP::gif_extension for containing comment extensions
 */
class comment_extension : public gif_extension {
    /**
     * @brief comment_extension
     *
     * Standart constructor. Set extension type as COMMENT_EXTENSION
     */
    comment_extension() : gif_extension(), comment_msg("") {ext_type = COMMENT_EXTENSION;}

    /**
     * @brief comment_extension
     *
     * Constructor for creating KonstantIMP::comment_extension from parent KonstantIMP::gif_extension (usefull for simple work with data)
     *
     * @param[in] gif_parent Parent object for getting data
     *
     * @throw std::runtime_error if parent extension type is not COMMENT_EXTENSION
     */
    comment_extension(const gif_extension * gif_parent);

    /**
     * @brief ~comment_extension
     *
     * Clear extension data (set comment_msg as "")
     */
    virtual ~comment_extension() {comment_msg = "";}

    /**
     * @brief read_data
     *
     * Method to fill extension data (comment message) from file stream
     *
     * @param[in] fin_gif std::ifstream object for reading from .gif
     *
     * @throw std::runtime_error If file is incorrect
     */
    virtual void read_data(std::ifstream & fin_gif);

    /**
     * @brief get_data
     *
     * Method to get extensions data (comment message)
     *
     * @return String with extension data (It is only comment message from extension for this class)
     */
    virtual std::string get_data() const {return this->get_comment_msg();}

    /**
     * @brief get_comment_msg
     *
     * Getter for comment messagr from read extension
     *
     * @return String with comment message
     */
    inline std::string get_comment_msg() const {return comment_msg;}

private:
    /// @brief comment_msg std::string for containing comment message
    std::string comment_msg;
};



class gif_frame {
public:
};

class gif_decoder {
public:
    gif_decoder(const std::string & file_name = "");
    ~gif_decoder();

    void open(const std::string & file_name);

    void clear();

private:

};

}

#endif // GIF_DECODER_HPP
