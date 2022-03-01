//
// Created by oupson on 28/02/2022.
//

#ifndef FOOTERM_VTE_HPP
#define FOOTERM_VTE_HPP

#include <vte/vte.h>
#include <gtkmm/widget.h>

#include "eventloop.hpp"

class Vte {
public:
    Vte();

    void spawnShell(EventLoop &eventLoop, const char *host, int port, const char *username, const char *password);

    Gtk::Widget *asGtkWidget();

private:
    GtkWidget *terminal;
};

#endif //FOOTERM_VTE_HPP
