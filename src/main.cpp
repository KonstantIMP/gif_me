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

    auto b = KonstantIMP::gif_decoder::decode(*first.get_frames_ref().at(2), first.get_gtc_ref(), true);

    argc = 1;

    auto app = Gtk::Application::create(argc, argv, "org.gif.me.base");

    Gtk::Window win;
    win.set_default_size(100, 100);
    win.set_title("Woof");

    std::size_t size_w = first.get_frames_ref().at(0)->get_frame_d().width;
    std::size_t size_h = first.get_frames_ref().at(0)->get_frame_d().height;

    Cairo::RefPtr<Cairo::ImageSurface> imSur = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size_w, size_h);
    Cairo::RefPtr<Cairo::Context> imCon = Cairo::Context::create(imSur);

    for(std::size_t i{0}; i < size_w; i++) {
        for(std::size_t j{0}; j < size_h; j++) {
            auto c = b.at(j * size_h + i);
            double red = static_cast<double>(c.r) / 255;
            double green = static_cast<double>(c.g) / 255;
            double blue = static_cast<double>(c.b) / 255;
            imCon->set_source_rgba(red, green, blue, c.a);
            imCon->rectangle(i, j, 1, 1);
            imCon->fill();
        }
    }

    Glib::RefPtr<Gdk::Pixbuf> pix_buf = Gdk::Pixbuf::create(imSur, 0, 0, size_w, size_h);

    //pix_buf = pix_buf->scale_simple(100, 100, Gdk::INTERP_BILINEAR);

    Gtk::Image img(pix_buf);

    win.add(img);

    /*Cairo::RefPtr<Cairo::ImageSurface> imSur = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size_w, size_h);
    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(imSur);

    cr->save();
    cr->set_source_rgb(1.0, 0, 0);
    cr->paint(); cr->restore();

    for(std::size_t i{0}; i < size_w; i++) {
        for(std::size_t j{0}; j < size_h; j++) {
            cr->set_source_rgb(1.0, 0.0, 0.0);
            cr->rectangle(i,j,1,1);
            cr->fill();
        }
    }

    Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_data(
    imSur->get_data(), Gdk::COLORSPACE_RGB, false, 8, size_w, size_h, imSur->get_stride());

    Gtk::Image *pImg = Gtk::manage(new Gtk::Image(pixbuf));*/

    //win.add(pImg);

    win.show_all();

    app->run(win);

    return 0;
}
