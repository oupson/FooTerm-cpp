//
// Created by oupson on 28/02/2022.
//

#ifndef FOOTERM_TERM_WINDOWS_HPP
#define FOOTERM_TERM_WINDOWS_HPP


#include <gtkmm/window.h>
#include <gtkmm/notebook.h>
#include <vte/vte.h>
#include <fmt/core.h>

#include "eventloop.hpp"

class FooTermWindow : public Gtk::Window {
private:
    Gtk::Notebook notebook;
    EventLoop eventLoop;
    void on_button_click();

public:
    FooTermWindow();
};

#endif //FOOTERM_TERM_WINDOWS_HPP
