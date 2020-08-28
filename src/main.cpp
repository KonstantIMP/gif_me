#include "../include/gif_decoder.hpp"
#include <iostream>

using namespace std;

int main(int argc, char * argv[]) {
    std::cout << "Hello!\n";

    if(argc == 1) {
        std::cout << "Use : gif_me [PATH TO GIF]\n";
        return 0;
    }

    std::cout << "Let\'s decode \"" << argv[1] << "\"\n\n";

    KonstantIMP::gif_decoder first(argv[1]);

    first.decode(true);

    while (1) {

    }

    return 0;
}
