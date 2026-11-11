 /*
 * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // CopperSpice implementation of the application event loop
    QApplication app(argc, argv);

    // Set Global Identity - use these strings exactly everywhere
    app.setOrganizationName("TalkCalendarProject");
    app.setApplicationName("TalkCalendar CS");

    MainWindow window;
    window.show();

    return app.exec();
}
