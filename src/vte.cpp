//
// Created by oupson on 28/02/2022.
//

#include "vte.hpp"
#include "term_windows.hpp"
#include "ssh.hpp"

#include <fmt/core.h>
#include <pty.h>
#include <fcntl.h>

static void got_child_exited([[maybe_unused]] VteTerminal *vte, [[maybe_unused]] gint status, [[maybe_unused]] Vte *window) {
    // TODO WHEN LOCAL TERMINAL
}

static void
termStateCallback([[maybe_unused]] VteTerminal *terminal, GPid pid, GError *error,
                  [[maybe_unused]] gpointer windowPtr) {
    if (error == nullptr) {
        fmt::print("{} started. (PID: {})\n", "foo", pid);
    } else {
        fmt::print("An error occurred: {}\n", error->message);
        g_clear_error(&error);
    }
}


Vte::Vte() {
    this->terminal = vte_terminal_new();

#if VTE_CHECK_VERSION(0, 62, 0)
    vte_terminal_set_enable_sixel(VTE_TERMINAL(this->terminal), true);
#endif
}

void Vte::spawnShell(EventLoop &eventLoop, Session& session) {
    int master, slave;

    struct termios settings{};
    cfmakeraw(&settings);

    int error = openpty(&master, &slave, nullptr, &settings, nullptr); // TODO

    fcntl(slave, F_SETFL, FNDELAY);

    VtePty *pty = vte_pty_new_foreign_sync(master, nullptr, nullptr);
    vte_terminal_set_pty(VTE_TERMINAL(this->terminal), pty);


    EventLoopEntry entry1{};
    entry1.fdout = slave;
    entry1.entryType = EventLoopEntry::CHANNEL;
    entry1.channel = session.getChannel();

    eventLoop.registerFd(session.getSock(), entry1);


    EventLoopEntry entry2{};
    entry2.fdout = slave;
    entry2.entryType = EventLoopEntry::DESCRIPTOR;
    entry2.channel = session.getChannel();

    eventLoop.registerFd(slave, entry2);

    g_signal_connect(G_OBJECT(terminal), "child-exited", G_CALLBACK(got_child_exited), this);
}

Gtk::Widget *Vte::asGtkWidget() {
    return Glib::wrap(this->terminal);
}
