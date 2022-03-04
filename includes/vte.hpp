//
// Created by oupson on 28/02/2022.
//

#ifndef FOOTERM_VTE_HPP
#define FOOTERM_VTE_HPP

#include <vte/vte.h>
#include <gtkmm/widget.h>

#include "eventloop.hpp"
#include "ssh.hpp"

class Vte {
private:
    GtkWidget *terminal;

public:
    Vte();

    void spawnShell(EventLoop &eventLoop, Session& session);

    Gtk::Widget *asGtkWidget();
};

#endif //FOOTERM_VTE_HPP
