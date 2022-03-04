//
// Created by oupson on 01/03/2022.
//

#include <libssh2_sftp.h>
#include <thread>

#include <gtkmm/treemodelsort.h>

#include <fmt/core.h>

#include "panel.hpp"

Panel::Panel(EventLoop &eventLoop, const char *host, int port, const char *username, const char *password) : path("/") {
    this->openSession(host, port, username, password);
    this->vte.spawnShell(eventLoop, this->session);

    this->paned.pack2(*this->vte.asGtkWidget());
    this->paned.pack1(this->mScrolledWindow);
    mScrolledWindow.add(mTreeView);

    mScrolledWindow.set_policy(Gtk::PolicyType::POLICY_AUTOMATIC, Gtk::PolicyType::POLICY_AUTOMATIC);
    // mScrolledWindow.set_expand();

    mScrolledWindow.set_size_request(50);

    mRefTreeModel = Gtk::ListStore::create(fileModel);

    auto sortModel = Gtk::TreeModelSort::create(mRefTreeModel);
    mTreeView.set_model(sortModel);

    sortModel->set_sort_column(0, Gtk::SortType::SORT_ASCENDING);

    mTreeView.append_column("Name", fileModel.colName);
    mTreeView.append_column("Size", fileModel.colSize);

    mTreeView.signal_row_activated().connect(sigc::mem_fun(this, &Panel::onCellDoubleClicked));

    auto pColumn1 = mTreeView.get_column(0);
    pColumn1->set_sort_column(fileModel.colName);

    auto pColumn2 = mTreeView.get_column(1);
    pColumn2->set_sort_column(fileModel.colSize);

    listFiles();

    this->paned.show_all();
}

void Panel::openSession(const char *host, int port, const char *username, const char *password) {
    session.openConnection(host, port, username, password);
}

void Panel::onCellDoubleClicked(const Gtk::TreePath &treePath, [[maybe_unused]] Gtk::TreeViewColumn *column) {
    auto row = *this->mTreeView.get_selection()->get_selected();
    std::string nPath = row.get_value(fileModel.colName);

    this->path.append(nPath);

    listFiles();
}

void Panel::listFiles() {
    LIBSSH2_SFTP_HANDLE *dir;
    int rc;

    do {
        dir = libssh2_sftp_opendir(session.getSftp(), this->path.c_str());
    } while (dir == nullptr && libssh2_session_last_errno(session.getSession()) == LIBSSH2_ERROR_EAGAIN);

    if (dir == nullptr && libssh2_session_last_errno(session.getSession()) < 0) {
        char *error;
        int errorLen;
        libssh2_session_last_error(session.getSession(), &error, &errorLen, true);

        throw SessionConnectException(fmt::format("unable to opendir \"{}\": {}", this->path.c_str(), error));
    }

    char buffer[1024];
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    this->mRefTreeModel->clear();

    do {
        rc = libssh2_sftp_readdir(dir, buffer, sizeof(buffer) - 1, &attrs);

        if (rc > 0 && !(buffer[0] == '.' && buffer[1] == 0)) {
            auto row = *mRefTreeModel->append();
            row[this->fileModel.colName] = buffer;
            row[this->fileModel.colSize] = attrs.filesize;
            //std::cerr << buffer << std::endl;
        }
    } while (rc > 0 || rc == LIBSSH2_ERROR_EAGAIN);

    if (rc < 0) {
        char *error;
        int errorLen;
        libssh2_session_last_error(session.getSession(), &error, &errorLen, true);
        throw SessionConnectException(fmt::format("unable to readdir \"{}\": {}", this->path.c_str(), error));
    }

    do {
        rc = libssh2_sftp_closedir(dir);
    } while (rc == LIBSSH2_ERROR_EAGAIN);

    if (rc < 0) {
        char *error;
        int errorLen;
        libssh2_session_last_error(session.getSession(), &error, &errorLen, true);
        throw SessionConnectException(fmt::format("unable to closedir \"{}\": {}", this->path.c_str(), error));
    }
}