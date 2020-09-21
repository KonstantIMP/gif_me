#ifndef  GIF_FRAME_HPP
#define  GIF_FRAME_HPP

#include "gif_defines.hpp"

namespace KonstantIMP {

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
 * @brief The gif_frame class
 *
 * Class for containing one GIF frame
 */
class gif_frame {
public:
    /**
     * @brief gif_frame
     *
     * Standart constructor. Zeroes data
     */
    gif_frame();

    /**
     * @brief ~gif_frame
     *
     * Standart destructor. Clears data after using
     */
    ~gif_frame();

    /**
     * @brief read_data
     *
     * Read data from file to the object
     *
     * @param[in] fin object for data reading
     */
    void read_data(std::ifstream & fin);

    /**
     * @brief get_frame_d
     *
     * Getter for frame_descriptor
     *
     * @return frame_d
     */
    inline frame_descriptor get_frame_d() const {
        return frame_d;
    }

    /**
     * @brief get_lwz_code_size
     *
     * LWZ minimum code size getter
     *
     * @return lwz_code_size
     */
    inline std::size_t get_lwz_code_size() const {
        return lwz_code_size;
    }

    /**
     * @brief get_lwz_data
     *
     * Compressed using LWZ frame image (in vector) getter by ref
     *
     * @return lwz_data vector ref
     */
    inline std::vector<unsigned char> & get_lwz_data_ref() const {
        return const_cast<std::vector<unsigned char> &>(lwz_data);
    }

    /**
     * @brief get_lct_ref
     *
     * Getter for lct vector (by ref)
     *
     * @return lct vector ref
     */
    inline std::vector<std::unique_ptr<rgba_color>> & get_lct_ref() const {
        return const_cast<std::vector<std::unique_ptr<rgba_color>> &>(lct);
    }

private:
    //// GlobalColorTable (rgba)
    std::vector<std::unique_ptr<rgba_color>> lct;

    //// Contains data about frame
    frame_descriptor frame_d;

    //// Contains minimum LWZ code size
    std::size_t lwz_code_size;

    //// Compressed using LWZ data
    std::vector<unsigned char> lwz_data;
};

}

#endif //GIF_FRAME_HPP
