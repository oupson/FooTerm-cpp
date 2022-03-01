#include "term_windows.hpp"

#include <gtkmm.h>

int main() {
    auto app = Gtk::Application::create("org.gtkmm.examples.base");
    FooTermWindow window;
    return app->run(window);
}
