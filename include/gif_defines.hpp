#ifndef  GIF_DEFINES_HPP
#define  GIF_DEFINES_HPP

/**
 * @file gif_defines.hpp
 * @brief File for containing main enumerations and structures
 * @author KonstantIMP
 */

#include <exception>
#include <stdexcept>

#include <fstream>
#include <cstdint>
#include <cstring>

#include <string>
#include <vector>
#include <memory>

#include <cmath>

#include <map>

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
    NO_DISPOSAL     = 0,  ///< Don't do anything
    NOT_DISPOS      = 1,  ///< Draw onto another frame
    RESTORE_BACK    = 2,  ///< Restore baskground before drawing
    RESTORE_PREV    = 3,  ///< Restore gif place before drawing
    TO_BE_DEFINED0  = 4,  ///< Not defined for now (by RFC)
    TO_BE_DEFINED1  = 5,  ///< Not defined for now (by RFC)
    TO_BE_DEFINED2  = 6,  ///< Not defined for now (by RFC)
    TO_BE_DEFINED3  = 7,  ///< Not defined for now (by RFC)
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

}

#endif //GIF_DEFINES_HPP
