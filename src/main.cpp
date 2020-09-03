#include "../include/gif_me.hpp"

#include <iostream>
#include <gtkmm.h>

int main(int argc, char * argv[]) {
    std::cout << "Hello!\n";

    if(argc == 1) {
        std::cout << "Use : gif_me [PATH TO GIF]\n";
        return 0;
    }

    std::cout << "Let\'s decode \"" << argv[1] << "\"\n\n";

    KonstantIMP::gif_parser first(argv[1]);

    first.parse(true);

    return 0;
}
