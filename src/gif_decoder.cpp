#include "../include/gif_decoder.hpp"

#include <iostream>

void KonstantIMP::gif_decoder::decode(const bool & debug) {
    if(debug) std::clog << "[DEBUG] Decoding start\n";

    if(!gif.is_open()) {
        if(debug) std::clog << "[DEBUG] GIF hasn\'t opened yet\n";
        throw std::runtime_error("GIF hasn\'t opened yet");
    }
    else if(debug) std::clog << "[DEBUG] GIF has opened\n\n";

    gif.seekg(0);

    //// GIF HEADER DECODING BLOCK
    {
        if(debug) std::clog << "[DEBUG] Header decoding\n";

        char data[3]; std::memset(data, 0, 3);
        gif.read(data, 3);

        if(std::strncmp(data, "GIF", 3)) {
            std::clog << "[DEBUG] File is not a gif\n";
            throw std::runtime_error("File is not a gif");
        }

        std::memset(data, 0, 3); gif.read(data, 3);
        if(std::strncmp(data, "87a", 3) == 0) {
            if(debug) std::clog << "\tGIF version is : GIF87a\n";
            head.version = GIF_87a;
        }
        else if(std::strncmp(data, "89a", 3) == 0) {
            if(debug) std::clog << "\tGIF version is : GIF89a\n";
            head.version = GIF_89a;
        }
        else {
            if(debug) std::clog << "\t[DEBUG] Gif version is unspec\n";
            head.version = NOT_GIF;
            throw std::runtime_error("Gif version is unspec");
        }

        std::memset(data, 0, 3); gif.read(data, 2);
        head.gif_width = static_cast<std::uint16_t>(((static_cast<std::uint16_t>(data[1]) & 0x00ff) << 8) | (static_cast<std::uint16_t>(data[0]) & 0x00ff));
        if(debug) std::clog << "\tGIF width is : " << head.gif_width << '\n';

        std::memset(data, 0, 3); gif.read(data, 2);
        head.gif_height = static_cast<std::uint8_t>(((static_cast<std::uint16_t>(data[1]) & 0x00ff) << 8) | (static_cast<std::uint16_t>(data[0]) & 0x00ff));
        if(debug) std::clog << "\tGIF height is : " << head.gif_height << '\n';

        std::memset(data, 0, 3); gif.read(data, 1);

        head.gct_flag = static_cast<bool>(data[0] & 0b10000000);
        if(debug) std::clog << "\tUsing GlobalColorTable : " << (head.gct_flag ? "YES" : "NO") << '\n';

        head.color_resolution = ((static_cast<std::uint8_t>(data[0]) & 0b01110000) >> 4) + 1;
        if(debug) std::clog <<"\tColor resolution is : " << (static_cast<std::uint16_t>(head.color_resolution) & 0x00ff) << '\n';

        head.sort_flag = static_cast<bool>(data[0] & 0b00001000);
        if(debug) std::clog << "\tUsing GCT sort : " << (head.sort_flag ? "YES" : "NO") << '\n';

        head.gct_size = static_cast<std::size_t>(std::pow(2, static_cast<std::size_t>(data[0] & 0b00000111) + 1));
        if(debug) std::clog << "\tGlobalColorTable size is : " << head.gct_size << '\n';

        std::memset(data, 0, 3); gif.read(data, 1);
        head.back_color = static_cast<std::uint8_t>(data[0]);
        if(debug) std::clog << "\tBackground color index is : " << (static_cast<std::uint8_t>(head.back_color) & 0x00ff) << '\n';

        std::memset(data, 0, 3); gif.read(data, 1);
        head.aspect_ratio = static_cast<std::uint8_t>(data[0]);
        if(debug) std::clog << "\tAspect ratio is : " << static_cast<double>(static_cast<double>(head.aspect_ratio + 15) / static_cast<double>(64)) << " (" << (static_cast<std::uint8_t>(head.aspect_ratio) & 0x00ff) << ")\n\n";
    }
    //// GIF HEADER DECODING BLOCK

    //// GCT DECODING BLOCK
    {
        if(head.gct_flag) {
            if(debug) std::clog << "[DEBUG] GlobalColorTable (" << head.gct_size << " colors)\n";

            for(std::size_t i{0}; i < head.gct_size; i++) {
                char data[4]; std::memset(data, 0, 4); gif.read(data, 3);

                rgba_color * tmp_color = new rgba_color;
                tmp_color->r = static_cast<std::uint8_t>(data[0]);
                tmp_color->g = static_cast<std::uint8_t>(data[1]);
                tmp_color->b = static_cast<std::uint8_t>(data[2]);

                gct.push_back(std::unique_ptr<rgba_color>(tmp_color));

                if(debug) {
                    std::clog << "\tr : " << static_cast<std::uint16_t>(tmp_color->r) << "\tg : " << static_cast<std::uint16_t>(tmp_color->g) << "\tb : " << static_cast<std::uint16_t>(tmp_color->b) << "\t\talpha : " << tmp_color->a << '\n';
                }
            }
            if(debug) std::clog << '\n';
        }
        else if(debug) std::clog << "[DEBUG] GlobalColorTable doesn\'t exist\n\n";
    }
    //// GCT DECODING BLOCK

    //// MAIN GIF PART DECODING BLOCK
    {
        if(debug) std::clog << "[DEBUG] Main GIF part decoding\n\n";

        while (!gif.eof()) {
            char data[2]; std::memset(data, 0, 2);
            gif.read(data, 1);

            if(data[0] == static_cast<char>(EXTENSION_BLOCK)) {
                if(debug) std::clog << "[DEBUG] Found extension block\n";

                std::memset(data, 0, 2); gif.read(data, 1);

                if(data[0] == static_cast<char>(PLAITEXT_EXTENSION)) {
                    return;
                }
                else if(data[0] == static_cast<char>(GRAPHIC_EXTENSION)) {
                    if(debug) std::clog << "\tExtension type : graphics control extension\n\n";

                    gif_extension * tmp = new graphic_extension;
                    tmp->read_data(gif); extensions.push_back(std::unique_ptr<gif_extension>(tmp));

                    if(debug) {
                        graphic_extension gui(tmp);

                        std::clog << "\tDisposal method : ";
                        switch (gui.get_disposal()) {
                        case NO_DISPOSAL: std::clog << "no disposal\n"; break;
                        case NOT_DISPOS: std::clog << "not dispos\n"; break;
                        case RESTORE_BACK: std::clog << "restore background\n"; break;
                        case RESTORE_PREV: std::clog << "restore previous frame\n"; break;
                        default: std::clog << "to be defined (by RFC)\n"; break;
                        }

                        std::clog << "\tNeed user input : " << (gui.is_user_input() ? "YES" : "NO") << '\n';
                        std::clog << "\tTransparency flag : " << (gui.is_transparency() ? "YES" : "NO") << '\n';

                        std::clog << "\tDelay time is  : " << gui.get_delay() << "/100 seconds\n";

                        std::clog << "\tTransparent color index is : " << static_cast<std::uint16_t>(gui.get_transparency_index()) << "\n\n";
                    }
                }
                else if(data[0] == static_cast<char>(COMMENT_EXTENSION)) {
                    return;
                }
                else if(data[0] == static_cast<char>(PROGRAM_EXTENSION)) {
                    if(debug) std::clog << "\tExtension type : application extension\n\n";

                    gif_extension * tmp = new application_extension;
                    tmp->read_data(gif); extensions.push_back(std::unique_ptr<gif_extension>(tmp));

                    if(debug) {
                        application_extension app(tmp);
                        std::clog << "\tApplication name : " << app.get_app_name() << '\n';
                        std::clog << "\tApplication code : " << app.get_app_code() << '\n';
                        std::clog << "\tApplication data size : " << app.get_app_data_ref().size() << " bytes\n\n";

                        std::clog << "\tApplication data dump :\n\t";
                        for(std::size_t i{0}; i < app.get_app_data_ref().size(); i++) {
                            if(app.get_app_data_ref().at(i) < 16) std::clog << '0';
                            std::clog << std::hex << static_cast<std::uint16_t>(app.get_app_data_ref().at(i)) << std::dec << ' ';

                            if(((i % 11) == 10) || (i == (app.get_app_data_ref().size() - 1))) {
                                for(std::size_t j{0}; j < 10 - (i % 11); j++) std::clog << "   ";
                                std::clog << " |  ";
                                for(std::size_t j{i - (i % 11)}; j <= i; j++) {
                                    if(app.get_app_data_ref().at(j) > 31 && app.get_app_data_ref().at(j) < 127)
                                        std::clog << app.get_app_data_ref().at(i);
                                    else std::clog << '.';
                                }
                                std::clog << "\n\t";
                            }
                        }

                        std::clog << "\n";
                    }
                }
                else {
                    if(debug) std::clog << "[DEBUG] Found UNSPEC extension\n";
                    throw std::runtime_error("Found UNSPEC extension");
                }
            }
            else if(data[0] == static_cast<char>(IMAGE_BLOCK)) {
                if(debug) std::clog << "[DEBUG] Image block found\n\n";

                gif_frame * frame = new gif_frame;
                frame->read_data(gif); frames.push_back(std::unique_ptr<gif_frame>(frame));

                if(debug) {
                    std::clog << "\tFrame descriptor\n\n";

                    std::clog << "\tLeft align : " << frame->get_frame_d().left << '\n';
                    std::clog << "\tTop align : " << frame->get_frame_d().top << '\n';
                    std::clog << "\tFrame width : " << frame->get_frame_d().width << '\n';
                    std::clog << "\tFrame height : " << frame->get_frame_d().height << '\n';

                    std::clog << "\tLocalColorTable using : " << (frame->get_frame_d().use_lct ? "YSE" : "NO") << '\n';

                    std::clog << "\tInterlacing : " << (frame->get_frame_d().is_interlaced ? "YSE" : "NO") << '\n';

                    std::clog << "\tLocalColorTable sort : " << (frame->get_frame_d().sort_flag ? "YES" : "NO") << '\n';

                    std::clog << "\tLocalColorTable size : " << static_cast<std::uint16_t>(frame->get_frame_d().lct_size) << "\n\n";

                    if(frame->get_frame_d().use_lct) {
                        std::clog << "\tLocalColorTable\n";
                        for (auto & iter : frame->get_lct_ref()) {
                            std::clog << "\tr : " << iter->r << "\tg : " << iter->g << "\tb : " << iter->b << "\t\talpha : " << iter->a << '\n';
                        }
                        std::clog << '\n';
                    }
                }

                return;
            }
            else if(data[0] == static_cast<char>(GIF_EOF_BLOCK)) {
                if(debug) std::clog << "[DEBUG] GIF_EOF found. Decoded\n";
                return;
            }
            else {
                if(debug) std::clog << "[DEBUG] Found UNSPEC extension\n";
                throw std::runtime_error("Found UNSPEC extension");
            }
        }
    }
    //// MAIN GIF PART DECODING BLOCK
}
