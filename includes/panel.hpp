//
// Created by oupson on 01/03/2022.
//

#ifndef FOOTERM_PANEL_HPP
#define FOOTERM_PANEL_HPP

#include <filesystem>

#include <gtkmm/widget.h>
#include <gtkmm/paned.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/listboxrow.h>

#include "vte.hpp"

class FileModel : public Gtk::TreeModel::ColumnRecord {
public:
    FileModel() {
        add(colName);
        add(colSize);
    }

    Gtk::TreeModelColumn<Glib::ustring> colName;
    Gtk::TreeModelColumn<unsigned long> colSize;
};

class Panel {
private:
    Gtk::Paned paned;
    Vte vte;
    Session session;

    FileModel fileModel;
    std::filesystem::path path;

    Gtk::ScrolledWindow mScrolledWindow;
    Gtk::TreeView mTreeView;
    Glib::RefPtr<Gtk::ListStore> mRefTreeModel;

    void onCellDoubleClicked(const Gtk::TreePath &treePath, [[maybe_unused]] Gtk::TreeViewColumn *column);

    void openSession(const char *host, int port, const char *username, const char *password);

public:
    Panel(EventLoop &eventLoop, const char *host, int port, const char *username, const char *password);

    void listFiles();

    Gtk::Paned &getPaned() {
        return this->paned;
    }

    Vte &getVte() {
        return this->vte;
    }

    Session &getSession() {
        return this->session;
    }
};

#endif //FOOTERM_PANEL_HPP
