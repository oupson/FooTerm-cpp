//
// Created by oupson on 28/02/2022.
//

#include "vte.hpp"
#include "term_windows.hpp"

#include <thread>
#include <gtkmm/headerbar.h>
#include <gtkmm/button.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <iostream>

FooTermWindow::FooTermWindow() {
    this->set_default_size(800, 600);
    this->add(notebook);

    auto *headerBar = Gtk::make_managed<Gtk::HeaderBar>();
    headerBar->set_title("FooTerm");
    headerBar->set_show_close_button(true);

    auto *addButton = Gtk::make_managed<Gtk::Button>("");
    addButton->set_image_from_icon_name("window-new");
    addButton->signal_clicked().connect(sigc::mem_fun(this, &FooTermWindow::on_button_click));
    headerBar->add(*addButton);
    this->set_titlebar(*headerBar);

    notebook.set_scrollable(true);
    notebook.set_group_name("foo-terminal-window");

    std::thread backgroundThread(EventLoop::start, &this->eventLoop);
    backgroundThread.detach();

    this->show_all();
}

void FooTermWindow::on_button_click() {
    Gtk::Dialog m("login", *this);

    m.add_button("Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
    m.add_button("Ok", Gtk::ResponseType::RESPONSE_OK);

    Gtk::Box *content = m.get_content_area();
    Gtk::Grid contentGrid;
    contentGrid.set_column_spacing(8);
    contentGrid.set_row_spacing(8);
    contentGrid.set_hexpand(true);


    contentGrid.set_margin_start(20);
    contentGrid.set_margin_end(20);
    contentGrid.set_margin_bottom(20);
    contentGrid.set_margin_top(20);

    content->add(contentGrid);

    Gtk::Label hostLabel("Host :");
    contentGrid.attach(hostLabel, 0, 0);

    Gtk::Entry hostEntry;
    contentGrid.attach(hostEntry, 1, 0);

    Gtk::Label portLabel("Port :");
    contentGrid.attach(portLabel, 2, 0);

    Gtk::Entry portEntry;
    contentGrid.attach(portEntry, 3, 0);

    Gtk::Label usernameLabel("Username :");
    contentGrid.attach(usernameLabel, 0, 1, 1, 1);

    Gtk::Entry usernameEntry;
    contentGrid.attach(usernameEntry, 1, 1, 3, 1);

    Gtk::Label passwordLabel("Password :");
    contentGrid.attach(passwordLabel, 0, 2, 1, 1);

    Gtk::Entry passwordEntry;
    passwordEntry.set_visibility(false);
    contentGrid.attach(passwordEntry, 1, 2, 3, 1);

    content->show_all();

    int result = m.run();

    std::string host;
    int port = 22;

    std::string username;
    std::string password;

    if (result == Gtk::RESPONSE_OK) {
        host = hostEntry.get_text();

        if (portEntry.get_text_length() > 0) {
            port = std::stoi(portEntry.get_text());
        }

        username = usernameEntry.get_text();
        password = passwordEntry.get_text();
    } else {
        return;
    }

    try {
        Vte *vte = new Vte();
        vte->spawnShell(this->eventLoop, host.c_str(), port, username.c_str(), password.c_str());

        Gtk::Widget *widget = vte->asGtkWidget();
        this->notebook.append_page(*widget, host, true);
        widget->show();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
