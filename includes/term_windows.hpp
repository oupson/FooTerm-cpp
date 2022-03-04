//
// Created by oupson on 28/02/2022.
//

#ifndef FOOTERM_TERM_WINDOWS_HPP
#define FOOTERM_TERM_WINDOWS_HPP

#include <gtkmm/window.h>
#include <gtkmm/notebook.h>
#include <vte/vte.h>
#include <fmt/core.h>

#include "panel.hpp"
#include "eventloop.hpp"

class FooTermWindow : public Gtk::Window {
private:
    Gtk::Notebook notebook;
    std::vector<Panel> panels;
    EventLoop eventLoop;

    void on_button_click();

public:
    FooTermWindow();

    std::vector<Panel> &getPanels() {
        return panels;
    }
};

#endif //FOOTERM_TERM_WINDOWS_HPP
