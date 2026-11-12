/*
 * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/**
 * @file mainwindow.cpp
 * @brief Casper Calendar
 *
 */

#include "mainwindow.h"
#include "eventeditor.h"
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QListWidget>
#include <QApplication>
// CopperSpice SQL Interfaces
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <iostream> //back to using raw C++

/**
 * @brief MainWindow constructor.
 * @param parent The parent widget
 */
MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent),
m_engine(new Synthesizer()),
m_voiceDict(new Dictionary())
{
    // sSetup window title and size
    setWindowTitle("Casper Calendar");
    resize(600, 450);
    //setWindowIcon(QIcon(":/icons/casper.png"));
    setWindowIcon(QIcon(":/icons/casper.png"));


    // Configure base font size
    QFont globalFont("Sans Serif", 14);
    //Force the application engine itself to default to font size 14
    qApp->setFont(globalFont);

    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);
    central->setFont(globalFont);

    // Initialize mainCalendar and eventList as class members
    this->mainCalendar = new QCalendarWidget(central);
    mainCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    mainCalendar->setSelectedDate(QDate::currentDate());

    this->eventList = new QListWidget(central);
    this->mainCalendar->setGridVisible(true);
    mainLayout->addWidget(this->mainCalendar);
    mainLayout->addWidget(this->eventList);
    setCentralWidget(central);

    // Programmatic Menubar Generation
    auto *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // --- File Menu ---
    auto *fileMenu = menuBar->addMenu("&File");
    auto *exportAction = fileMenu->addAction("&Export iCal...");
    auto *importAction = fileMenu->addAction("&Import iCal...");
    fileMenu->addSeparator();
    auto *quitAction = fileMenu->addAction("&Quit");

    // --- Event Menu (NEW) ---
    auto *eventMenu = menuBar->addMenu("&Event");
    auto *newEventAction = eventMenu->addAction("&New Event");
    auto *editEventAction = eventMenu->addAction("&Edit Selected Event");
    auto *deleteEventAction = eventMenu->addAction("&Delete Event");
    auto *deleteAllEventsAction = eventMenu->addAction("Delete All Events");

    // --- Calendar Menu (NEW) ---
    auto *calendarMenu = menuBar->addMenu("&Calendar");
    auto *gotoTodayAction = calendarMenu->addAction("Go to &Today");
    calendarMenu->addSeparator();
    auto *preferencesAction = calendarMenu->addAction("&Preferences");
    calendarMenu->addSeparator();
    auto *speakTimeAction = calendarMenu->addAction("Speak &Time");
    auto *speakDayEventsAction = calendarMenu->addAction("Speak Events");

    // --- Help Menu ---
    auto *helpMenu = menuBar->addMenu("&Help");
    auto *infoAction = helpMenu->addAction("&Information");
    helpMenu->addSeparator();
    auto *aboutAction = helpMenu->addAction("&About");

    // Set keyboard shortcuts
    newEventAction->setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_N));
    editEventAction->setShortcut(QKeySequence(Qt::ControlModifier | Qt::Key_E));
    speakTimeAction->setShortcut(QKeySequence(Qt::Key_T));
    speakDayEventsAction->setShortcut( QKeySequence(Qt::Key_Space));
    gotoTodayAction->setShortcut(QKeySequence(Qt::Key_Home));

    // Connect signals and methods
    connect(quitAction, &QAction::triggered, this, &MainWindow::close);
    // Connect the export action
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportiCal);
    // Connect the import action
    connect(importAction, &QAction::triggered, this, &MainWindow::importiCal);

    connect(infoAction, &QAction::triggered, this, &MainWindow::information);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);

    //Connect claendar selection changed to update eventList
    connect(this->mainCalendar, &QCalendarWidget::selectionChanged, this, &MainWindow::updateEventList);
    //calendar connections
    connect(gotoTodayAction, &QAction::triggered, this, &MainWindow::goToToday);
    connect(speakTimeAction, &QAction::triggered, this, &MainWindow::speakTime);
    connect(speakDayEventsAction, &QAction::triggered, this, &MainWindow::speakEvents);
    connect(mainCalendar, &QCalendarWidget::currentPageChanged,
            this, &MainWindow::onCalendarPageChanged);
    connect(preferencesAction, &QAction::triggered, this, &MainWindow::setPreferences);

    //event connections
    connect(newEventAction, &QAction::triggered, this, &MainWindow::newEvent);
    connect(editEventAction, &QAction::triggered, this, &MainWindow::editEvent);
    connect(deleteEventAction, &QAction::triggered, this, &MainWindow::deleteEvent);
    connect(deleteAllEventsAction, &QAction::triggered, this, &MainWindow::deleteAllEvents);

    //  Setup SQLite database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("calendar.db");

    if (!db.open()) {
        qDebug() << "Database Connection Error:" << db.lastError().text();
    } else {
        //qDebug() << "Connected to SQLite successfully!";

        QSqlQuery query;
        bool success = query.exec(
            "CREATE TABLE IF NOT EXISTS events ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "event_date TEXT, "
            "summary TEXT, "
            "description TEXT, "
            "location TEXT, "
            "start_time TEXT, "
            "end_time TEXT, "
            "is_allday INTEGER DEFAULT 0, "
            "is_priority INTEGER DEFAULT 0, "
            "recurrence_type INTEGER DEFAULT 0"
            ")"
        );

        if (!success) {
            qDebug() << "Failed to create table:" << query.lastError().text();
        }
    }
    //Use QSettings to get user preferences
    QSettings settings; // automatically uses organization and app name from main.cpp
    m_talk = settings.value("talk", true).toBool(); // 'true' is the default if not found
    m_talk_startup = settings.value("talk_startup", true).toBool();
    m_upcoming = settings.value("upcoming", true).toBool();
    m_upcoming_days = settings.value("upcoming_days", 3).toInt();
    m_tempo = settings.value("tempo", 17).toInt();
    m_font_size = settings.value("font_size", 14).toInt();
    // Load the colors. If they do not exist yet, use defaults.
    m_eventColor = settings.value("colors/event", QColor(Qt::yellow)).value<QColor>(); // Default Yellow
    m_priorityColor = settings.value("colors/priority", QColor(Qt::darkMagenta)).value<QColor>(); // Default Purple
    //mark up calendar
    markEventsOnCalendar();

    //Setup font
    QFont defaultFont = QApplication::font();
    // qDebug() << "Default Font:" << defaultFont.family() << "Size:" << defaultFont.pointSize();
    // Set the global font for the application using m_font_size
    QFont appFont(defaultFont.family(), m_font_size); // Specify font family and size
    QApplication::setFont(appFont);
    this->mainCalendar->setFont(appFont);
    this->eventList->setFont(appFont);

    //Get today date
    QDate today = QDate::currentDate();
    mainCalendar->setSelectedDate(today);
    QList<CalendarEvent> events = getEventsForDate(today);
    updateEventList(); //put today events in the event list box

    //Welcome message
    if (m_talk && m_talk_startup) {
        std::string welcome = "Casper Calendar "; //Casper not in dictionary so invoke (test) G2P engine
        QString datePhrase = getDatePhrase(today) + " ";
        welcome.append(datePhrase.toStdString());

        for (const CalendarEvent &evt : events) {
            const bool isAllDay = evt.m_isAllDay;
            welcome += evt.m_summary.toStdString() + " ";

            if (!isAllDay) {
                QString timePhrase = getTimePhrase(evt.m_startTime.hour(), evt.m_startTime.minute(), isAllDay);
                welcome.append(timePhrase.toStdString());
                welcome.append(" ");
            }
        }

        // Add Upcoming Events
        if(m_upcoming){
            QString upcomingEvents = getUpcomingEventsPhrase();
            if (!upcomingEvents.isEmpty()) {
                welcome.append(upcomingEvents.toStdString());
            }
        }

        //std::cout<< "welcome = " <<welcome<<std::endl;

        m_engine->speak(welcome, m_voiceDict, static_cast<float>(m_tempo) / 10.0f);
    }

}

/**
 * @brief MainWindow destructor.
 */
MainWindow::~MainWindow()
{
    delete m_engine;
    delete m_voiceDict;
}

/**
 * @brief Retrieves all calendar events for a given date.
 * @param date The QDate to query.
 * @return List of CalendarEvent objects matching the date.
 */
QList<CalendarEvent> MainWindow::getEventsForDate(const QDate &date)
{
    QList<CalendarEvent> events;
    QSqlQuery query;

    const QString sql =
    "SELECT id, event_date, summary, description, location, "
    "start_time, end_time, is_allday, is_priority, recurrence_type "
    "FROM events "
    "WHERE event_date = ? "
    "OR (recurrence_type = 4 AND substr(event_date, 6, 5) = ?) "
    "ORDER BY is_allday DESC, start_time ASC";

    if (!query.prepare(sql)) {
        qDebug() << "Could not prepare events query:"
        << query.lastError().text();
        return events;
    }

    query.addBindValue(date.toString("yyyy-MM-dd"));
    query.addBindValue(date.toString("MM-dd"));

    if (!query.exec()) {
        qDebug() << "Could not execute events query:"
        << query.lastError().text();
        return events;
    }

    while (query.next()) {
        CalendarEvent event;

        event.m_id = query.value("id").toInt();

        const QDate storedDate = QDate::fromString(
            query.value("event_date").toString(), "yyyy-MM-dd");

        event.m_eventDate = storedDate;
        event.m_summary = query.value("summary").toString();
        event.m_description = query.value("description").toString();
        event.m_location = query.value("location").toString();

        event.m_startTime = QTime::fromString(
            query.value("start_time").toString(), "HH:mm");
        event.m_endTime = QTime::fromString(
            query.value("end_time").toString(), "HH:mm");

        event.m_isAllDay = query.value("is_allday").toInt() != 0;
        event.m_isPriority = query.value("is_priority").toInt() != 0;
        event.m_recurrenceType = query.value("recurrence_type").toInt();

        events.append(event);
    }

    return events;
}


/**
 * @brief Marks dates on the calendar that have events.
 */
void MainWindow::markEventsOnCalendar()
{
    QCalendarWidget *calendar = this->mainCalendar;

    // Clear date-specific formats left from the previous calendar page.
    calendar->setDateTextFormat(QDate(), QTextCharFormat());

    const int month = calendar->monthShown();
    const int year = calendar->yearShown();

    QSqlQuery query;
    const QString sql =
    "SELECT event_date, is_priority "
    "FROM events "
    "WHERE substr(event_date, 1, 7) = ? "
    "OR (recurrence_type = 4 AND substr(event_date, 6, 2) = ?)";

    if (!query.prepare(sql)) {
        qDebug() << "Could not prepare calendar-marking query:"
        << query.lastError().text();
        return;
    }

    QString monthText = QString::number(month);
    if (month < 10) {
        monthText = "0" + monthText;
    }

    QString yearMonth = QString::number(year) + "-" + monthText;

    query.addBindValue(yearMonth);  // Matches yyyy-MM in event_date
    query.addBindValue(monthText);  // Matches MM in event_date


    QTextCharFormat standardFormat;
    standardFormat.setFontWeight(QFont::Bold);
    standardFormat.setForeground(Qt::black);
    standardFormat.setBackground(m_eventColor);

    QTextCharFormat priorityFormat;
    priorityFormat.setFontWeight(QFont::Bold);
    priorityFormat.setForeground(Qt::white);
    priorityFormat.setBackground(m_priorityColor);

    if (!query.exec()) {
        qDebug() << "Could not execute calendar-marking query:"
        << query.lastError().text();
        return;
    }

    while (query.next()) {
        const QDate storedDate = QDate::fromString(
            query.value("event_date").toString(), "yyyy-MM-dd");

        if (!storedDate.isValid()) {
            continue;
        }

        // Use the displayed year for yearly occurrences.
        const QDate date(year, month, storedDate.day());

        // For example, Feb 29 is not valid in a non-leap year.
        if (!date.isValid()) {
            continue;
        }

        const bool isPriority = query.value("is_priority").toInt() != 0;
        calendar->setDateTextFormat(
            date, isPriority ? priorityFormat : standardFormat);
    }

    const QDate today = QDate::currentDate();

    QTextCharFormat todayFormat = calendar->dateTextFormat(today);
    todayFormat.setFontUnderline(true);
    calendar->setDateTextFormat(today, todayFormat);
}


/**
 * @brief Clears the event list and updates date marks on the calendar that have events
 * when the calendar page changes. Set date to first day of month
 */
void MainWindow::onCalendarPageChanged()
{
    eventList->clear();
    markEventsOnCalendar();
}


/**
 * @brief  About Casper Calendar
 */
void MainWindow::about()
{
    QMessageBox::about(this, "About Casper Calendar",
                       "Casper Calendar CopperSpice 2.1 & Wayland.");
}

/**
 * @brief System information
 */
void MainWindow::information()
{
    //qDebug() << "Info Triggered";
    QMessageBox box(this);
    box.setWindowTitle("System Info");
    box.setTextFormat(Qt::RichText);

    //QString csVersion="0.2.1";

    QString msg = "<b>Casper Calendar (CS) Version: </b>";
    msg.append(PROJECT_VERSION);
    msg.append("<br>");

    msg.append("CopperSpice Version: ");
    msg.append(CS_VERSION_STR);
    msg.append("<br>");

    msg.append("System Font: ");
    QFont defaultFont = QApplication::font();
    msg.append(defaultFont.family().toHtmlEscaped());
    msg.append("<br>");

    msg.append("Capser Calendar Font Size: ");
    msg.append(QString::number(m_font_size));
    msg.append("<br>");

    msg.append("<b>Shortcuts</b><br>");
    msg.append("Ctrl+N = New Event<br>");
    msg.append("Ctrl+E = Edit Selected Event<br>");
    msg.append("T Key = Speak Time<br>");
    msg.append("Spacebar = Speak Selected Day Events<br>");

    box.setText(msg);
    box.exec();

}

/**
 * @brief Updates the event list with events for the selected date.
 *
 * Fetches events from the database that match the currently selected date and populates
 * the event list widget with these events. If fetching fails, an error message is logged.
 */
void MainWindow::updateEventList()
{

    const QString selectedDate = this->mainCalendar->selectedDate().toString("yyyy-MM-dd");
    this->eventList->clear();

    const QString sql =
    "SELECT id, event_date, start_time, summary, description, location, is_allday, is_priority "
    "FROM events "
    "WHERE event_date = ? "
    "OR (recurrence_type = 4 "
    "    AND substr(event_date, 6, 5) = substr(?, 6, 5))";

    QSqlQuery readQuery;
    readQuery.prepare(sql);
    readQuery.addBindValue(selectedDate);
    readQuery.addBindValue(selectedDate);


    if (readQuery.exec()) {
        while (readQuery.next()) {
            const QString eventId   = readQuery.value(0).toString();
            const QString startTime = readQuery.value(2).toString();
            const QString summary   = readQuery.value(3).toString();
            const QString description  = readQuery.value(4).toString();
            const QString location   = readQuery.value(5).toString();
            const bool isAllDay     = readQuery.value(6).toInt() != 0;

            QString displayText="";
            if (isAllDay) displayText.append("All day -");
            else displayText.append(startTime);
            displayText.append(" ");
            displayText.append(summary);
            if (!description.isEmpty())
            {
                displayText.append(", ");
                displayText.append(description);
            }
            if (!location.isEmpty()) {
                displayText.append(", ");
                displayText.append(location);
            }

            auto *listItem = new QListWidgetItem(displayText);
            listItem->setData(Qt::UserRole, eventId);
            this->eventList->addItem(listItem);
        }
    } else {
        qDebug() << "Failed to fetch events:"
        << readQuery.lastError().text();
    }
}

/**
 * @brief Goes to today's date in the calendar.
 */
void MainWindow::goToToday()
{
    this->mainCalendar->setSelectedDate(QDate::currentDate());
}

/**
 * @brief Speaks the current time
 */
void MainWindow::speakTime()
{
    int hour = QTime::currentTime().hour();
    int minute = QTime::currentTime().minute();
    QString timePhrase =getTimePhrase(hour, minute, false);

    QString speakTimePhrase ="The time is ";
    speakTimePhrase.append(timePhrase);
    //qDebug()<<"speak time phrase "<<speakTimePhrase;
    m_engine->speak(speakTimePhrase.toStdString(),m_voiceDict,static_cast<float>(m_tempo) / 10.0f);
}

/**
 * @brief Normalizes an integer day into its ordinal word form.
 * @param day The day of the month (1-31).
 * @return A string such as "first", "second", etc.
 */
QString MainWindow::getOrdinalStr(int day)
{
    static const QStringList ordinals = {
        "", // 0 (ignored)
        "first", "second", "third", "fourth", "fifth",
        "sixth", "seventh", "eighth", "ninth", "tenth",
        "eleventh", "twelfth", "thirteenth", "fourteenth", "fifteenth",
        "sixteenth", "seventeenth", "eighteenth", "nineteenth", "twentieth",
        "twenty first", "twenty second", "twenty third", "twenty fourth", "twenty fifth",
        "twenty sixth", "twenty seventh", "twenty eighth", "twenty ninth", "thirtieth",
        "thirty first"
    };

    if (day >= 1 && day <= 31) {
        return ordinals[day];
    }

    return ""; // Return empty if date is invalid
}

/**
 * @brief Converts an integer day into its cardinal word form for speech
 * @param number The number (1-59) as 59 minutes
 * @return A string such as "one", "two", etc.
 */
QString MainWindow::getCardinalStr(int number)
{
    static const QStringList cardinals = {
        "", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten",
        "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen",
        "eighteen", "nineteen", "twenty", "twenty one", "twenty two", "twenty three",
        "twenty four", "twenty five", "twenty six", "twenty seven", "twenty eight",
        "twenty nine", "thirty", "thirty one", "thirty two", "thirty three",
        "thirty four", "thirty five", "thirty six", "thirty seven", "thirty eight",
        "thirty nine", "forty", "forty one", "forty two", "forty three", "forty four",
        "forty five", "forty six", "forty seven", "forty eight", "forty nine", "fifty",
        "fifty one", "fifty two", "fifty three", "fifty four", "fifty five",
        "fifty six", "fifty seven", "fifty eight", "fifty nine"
    };

    if (number >= 1 && number <= 59) {
        return cardinals[number];
    }

    return ""; // Return empty if date is invalid
}

/**
 * @brief Converts a QDate.
 * @return A formatted string like "tuesday twenty fourth february"
 */
QString MainWindow::getDatePhrase(const QDate &date)
{
    QString dayName = date.toString("dddd").toLower();
    QString monthName = date.toString("MMMM").toLower();
    int day = date.day();

    QString ordinalStr = getOrdinalStr(day);

    QString datePhrase="";
    datePhrase.append(dayName);
    datePhrase.append(" ");
    datePhrase.append(ordinalStr);
    datePhrase.append(" ");
    datePhrase.append(monthName);
    datePhrase.append(" ");

    return datePhrase;
}

/**
 * @brief Converts a 24-hour time into a natural language phrase.
 * @param hour The hour (0-23).
 * @param minute The minute (0-59).
 * @param isAllDay Flag to bypass time generation for all-day events.
 * @return A formatted string like "ten zero five am".
 */
QString MainWindow::getTimePhrase(int hour, int minute, bool isAllDay)
{
    //if (isAllDay) return "all day";
    if (isAllDay) return "";

    QString timePhrase = "";

    int hour12 = (hour == 0 || hour == 12) ? 12 : hour % 12;
    QString ampm = (hour < 12) ? "amtime" : "pmtime";
    // add hour words
    timePhrase.append(getCardinalStr(hour12));
    timePhrase.append(" ");

    if (minute == 0) {
        timePhrase.append(ampm);
        timePhrase.append(" ");
    } else if (minute < 10) {
        timePhrase.append(" zero ");
        timePhrase.append(getCardinalStr(minute));
        timePhrase.append(" ");
        timePhrase.append(ampm);
        timePhrase.append(" ");

    } else {
        timePhrase.append(getCardinalStr(minute));
        timePhrase.append(" ");
        timePhrase.append(ampm);
        timePhrase.append(" ");
    }

    return timePhrase;
}

/**
 * @brief Speaks the events for the selected calendar day
 * using the text-to-speech engine
 */
void MainWindow::speakEvents()
{
    QDate dayDate = this->mainCalendar->selectedDate();
    QList<CalendarEvent> events = getEventsForDate(dayDate);

    if (m_talk) {
        QString phrase = getDatePhrase(dayDate)+ " ";
        for (const CalendarEvent &ev : events) {
            //QString startTime =ev.m_startTime;

            QString timePhrase = getTimePhrase(ev.m_startTime.hour(), ev.m_startTime.minute(), ev.m_isAllDay)+ " ";
            if (ev.m_isAllDay) {
                phrase.append(ev.m_summary);
                phrase.append(" ");
            } else {
                phrase.append(timePhrase);
                phrase.append(" ");
                phrase.append(ev.m_summary);
                phrase.append(" ");
            }
        }

        m_engine->speak(phrase.toStdString(),m_voiceDict,static_cast<float>(m_tempo) / 10.0f);
    }
}
/**
 * @brief Opens the editor editor for  a new event.
 */
void MainWindow::newEvent()
{
    EventEditorDialog dialog(this);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    const QString summaryText = dialog.lineSummary->text().trimmed();
    if (summaryText.isEmpty()) {
        return;
    }

    const QString descriptionText = dialog.lineDescription->text().trimmed();
    const QString locationText = dialog.lineLocation->text().trimmed();
    const QString selectedDate =
    mainCalendar->selectedDate().toString("yyyy-MM-dd");

    const bool isAllDay = dialog.checkAllDay->isChecked();
    const bool isPriority = dialog.chkPriority->isChecked();
    const int recurrenceType = dialog.chkYearly->isChecked() ? 4 : 0;

    // Store blank times for all-day events. Alternatively, retain the
    // selected times if the rest of your code expects them.
    const QString startTime = isAllDay
    ? QString()
    : dialog.timeStart->time().toString("HH:mm");
    const QString endTime = isAllDay
    ? QString()
    : dialog.timeEnd->time().toString("HH:mm");

    QSqlQuery query;
    const QString sql =
    "INSERT INTO events "
    "(event_date, summary, description, location, start_time, end_time, "
    "is_allday, is_priority, recurrence_type) "
    "VALUES (:date, :summary, :description, :location, :start, :end, "
    ":all_day, :priority, :recurrence)";

    if (!query.prepare(sql)) {
        qDebug() << "Could not prepare event insert:"
        << query.lastError().text();
        return;
    }

    query.bindValue(":date", selectedDate);
    query.bindValue(":summary", summaryText);
    query.bindValue(":description", descriptionText);
    query.bindValue(":location", locationText);
    query.bindValue(":start", startTime);
    query.bindValue(":end", endTime);
    query.bindValue(":all_day", isAllDay ? 1 : 0);
    query.bindValue(":priority", isPriority ? 1 : 0);
    query.bindValue(":recurrence", recurrenceType);

    if (!query.exec()) {
        qDebug() << "Could not insert event:" << query.lastError().text();
        return;
    }

    updateEventList();
    markEventsOnCalendar();
}

/**
 * @brief Edits an existing event.
 *
 * Opens the event editor dialog pre-filled with the details of the selected event. If the user
 * accepts the dialog (i.e., clicks "Save Event"), the updated event details are retrieved,
 * validated, and saved back to the database.
 */
void MainWindow::editEvent()
{
    QListWidgetItem *selectedItem = eventList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(
            this, "Edit Event", "Please select an event from the list to edit.");
        return;
    }

    const QVariant eventId = selectedItem->data(Qt::UserRole);
    if (!eventId.isValid()) {
        return;
    }

    QSqlQuery selectQuery;
    selectQuery.prepare(
        "SELECT summary, description, location, start_time, end_time, "
        "is_allday, is_priority, recurrence_type "
        "FROM events WHERE id = :id");
    selectQuery.bindValue(":id", eventId);

    if (!selectQuery.exec() || !selectQuery.next()) {
        qDebug() << "Could not load event for editing:"
        << selectQuery.lastError().text();
        return;
    }

    EventEditorDialog dialog(this);
    dialog.setWindowTitle("Edit Event");

    dialog.lineSummary->setText(selectQuery.value("summary").toString());
    dialog.lineDescription->setText(selectQuery.value("description").toString());
    dialog.lineLocation->setText(selectQuery.value("location").toString());

    const bool isAllDay = selectQuery.value("is_allday").toInt() != 0;
    dialog.checkAllDay->setChecked(isAllDay);
    dialog.chkPriority->setChecked(
        selectQuery.value("is_priority").toInt() != 0);
    dialog.chkYearly->setChecked(
        selectQuery.value("recurrence_type").toInt() == 4);

    const QTime startTime = QTime::fromString(
        selectQuery.value("start_time").toString(), "HH:mm");
    const QTime endTime = QTime::fromString(
        selectQuery.value("end_time").toString(), "HH:mm");

    if (startTime.isValid()) {
        dialog.timeStart->setTime(startTime);
    }
    if (endTime.isValid()) {
        dialog.timeEnd->setTime(endTime);
    }

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    const QString summary = dialog.lineSummary->text().trimmed();
    if (summary.isEmpty()) {
        return;
    }

    const bool updatedAllDay = dialog.checkAllDay->isChecked();
    const bool updatedPriority = dialog.chkPriority->isChecked();
    const int updatedRecurrence = dialog.chkYearly->isChecked() ? 4 : 0;

    const QString start = updatedAllDay
    ? QString()
    : dialog.timeStart->time().toString("HH:mm");
    const QString end = updatedAllDay
    ? QString()
    : dialog.timeEnd->time().toString("HH:mm");

    QSqlQuery updateQuery;
    updateQuery.prepare(
        "UPDATE events SET "
        "summary = :summary, "
        "description = :description, "
        "location = :location, "
        "start_time = :start, "
        "end_time = :end, "
        "is_allday = :all_day, "
        "is_priority = :priority, "
        "recurrence_type = :recurrence "
        "WHERE id = :id");

    updateQuery.bindValue(":summary", summary);
    updateQuery.bindValue(":description", dialog.lineDescription->text().trimmed());
    updateQuery.bindValue(":location", dialog.lineLocation->text().trimmed());
    updateQuery.bindValue(":start", start);
    updateQuery.bindValue(":end", end);
    updateQuery.bindValue(":all_day", updatedAllDay ? 1 : 0);
    updateQuery.bindValue(":priority", updatedPriority ? 1 : 0);
    updateQuery.bindValue(":recurrence", updatedRecurrence);
    updateQuery.bindValue(":id", eventId);

    if (!updateQuery.exec()) {
        qDebug() << "Could not update event:"
        << updateQuery.lastError().text();
        return;
    }

    updateEventList();
    markEventsOnCalendar();
}

/**
 * @brief Delete (Selected) Event
 */
void MainWindow::deleteEvent()
{
    QListWidgetItem *selectedItem = this->eventList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Delete Event", "Please select an event from the list to delete.");
        return;
    }

    QVariant eventId = selectedItem->data(Qt::UserRole);
    if (!eventId.isValid()) return;

    auto reply = QMessageBox::question(this, "Delete Event",
                                       "Are you sure you want to permanently delete this event?",
                                       QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    QSqlQuery deleteQuery;
    deleteQuery.prepare("DELETE FROM events WHERE id = :id");
    deleteQuery.bindValue(":id", eventId);

    if (!deleteQuery.exec()) {
        qDebug() << "Delete database entry error:" << deleteQuery.lastError().text();
        QMessageBox::critical(this, "Error", "Failed to delete the event from the database.");
    } else {
        qDebug() << "Successfully deleted entry index:" << eventId.toInt();

        int row = this->eventList->row(selectedItem);
        delete this->eventList->takeItem(row);

        if (this->eventList->count() == 0) {
            this->eventList->addItem("No events scheduled for this day.");
        }
    }
    markEventsOnCalendar();
}

/**
 * @brief Delete All Events  (Danger zone)
 */
void MainWindow::deleteAllEvents()
{
    //qDebug()<<"Delete ALL Events";
    // confirmation Dialog
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "DANGER ZONE: Database",
                                 "This will permanently delete ALL events. Are you sure?",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        if (query.exec("DELETE FROM events")) {
            //mainCalendar->setSelectedDate(QDate::currentDate());
            markEventsOnCalendar();; // Refresh highlights after deleting
            QMessageBox::information(this, "Success", "Database cleared.");
        } else {
            qDebug() << "Delete All Error:" << query.lastError().text();
        }
    }
}

/**
 * @brief Set Preferences
 */
void MainWindow::setPreferences()
{
    Preferences dialog(this);

    // Set initial values in the dialog
    dialog.setTalk(m_talk);
    dialog.setTalkStartup(m_talk_startup);
    dialog.setUpcoming(m_upcoming);
    dialog.setUpcomingDays(m_upcoming_days);
    dialog.setTempo(m_tempo);
    dialog.setFontSize(m_font_size);
    dialog.setEventColour(m_eventColor);
    dialog.setPriorityColour(m_priorityColor);

    if (dialog.exec() == QDialog::Accepted) {
        //qDebug() << "Preferences Window Triggered";

        // Get the updated values from the dialog
        m_talk = dialog.getTalk();
        m_talk_startup = dialog.getTalkStartup();
        m_upcoming = dialog.getUpcoming();
        m_upcoming_days = dialog.getUpcomingDays();
        m_tempo = dialog.getTempo();
        m_font_size = dialog.getFontSize();
        m_eventColor = dialog.getEventColour();
        m_priorityColor = dialog.getPriorityColour();

        // Save the updated values to QSettings
        QSettings settings; // automatically uses the names from main.cpp
        settings.setValue("talk", m_talk);
        settings.setValue("talk_startup", m_talk_startup);
        settings.setValue("upcoming", m_upcoming);
        settings.setValue("upcoming_days", m_upcoming_days);
        settings.setValue("font_size", m_font_size);
        settings.setValue("tempo", m_tempo);

        // Save the color values
        settings.setValue("colors/event", QVariant::fromValue(m_eventColor));
        settings.setValue("colors/priority", QVariant::fromValue(m_priorityColor));
        //Refresh font Size
        QFont defaultFont = QApplication::font();
        // Set the global font for the application
        QFont appFont(defaultFont.family(), m_font_size); // Specify font family and size
        QApplication::setFont(appFont);
        this->mainCalendar->setFont(appFont);
        this->eventList->setFont(appFont);


        // Refresh the calendar to show the new colors immediately
        markEventsOnCalendar();
    }
}

/**
 * @brief Builds a phrase summarising upcoming events.
 * @return A string containing information about upcoming events, or an empty string if none.
 */
QString MainWindow::getUpcomingEventsPhrase()
{
    if (!m_upcoming) return ""; // Respect the config setting

    QString upcomingPhrase = "";
    QDate today = QDate::currentDate();
    QDate endDate = today.addDays(m_upcoming_days);
    int count = 0;

    QSqlQuery query;
    QString queryString =
    "SELECT summary, event_date FROM events "
    "WHERE event_date BETWEEN '" + today.toString("yyyy-MM-dd") + "' AND '" + endDate.toString("yyyy-MM-dd") + "' "
    "ORDER BY event_date ASC";

        if (query.exec(queryString)) {
            while (query.next()) {
                QString summary = query.value("summary").toString();
                QDate evDate = QDate::fromString(query.value("event_date").toString(), "yyyy-MM-dd");

                if (evDate > today && evDate <= endDate) {
                    QString datePhrase = getDatePhrase(evDate);
                    upcomingPhrase += summary + " on " + datePhrase+ " ";
                    count++;
                }
            }
        } else {
            qDebug() << "Query Error:" << query.lastError().text();
        }

        if (count > 0) {
            //return "You have " + getCardinalStr(count) + " upcoming events " + upcomingPhrase;

            return "you have " + upcomingPhrase;
            //return  upcomingPhrase.toLower();
        }
        return "";
}

/**
 * @brief Exports all events from the database to an iCalendar (.ics/.ical) file.
 */
void MainWindow::exportiCal()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export iCal", "", "iCalendar Files (*.ics *.ical)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open file for writing.");
        return;
    }

    QTextStream out(&file);
    out << "BEGIN:VCALENDAR\n";
    out << "VERSION:2.0\n";
    out << "PRODID:-//TalkCalendar//CopperSpice 4.x//EN\n";

    QSqlQuery query("SELECT * FROM events");
    if (!query.exec()) {
        qDebug() << "Database Query Error:" << query.lastError().text();
        QMessageBox::warning(this, "Error", "Failed to retrieve events from the database.");
        file.close();
        return;
    }

    while (query.next()) {
        out << "BEGIN:VEVENT\n";

        // Summary
        out << "SUMMARY:" << query.value("summary").toString() << "\n";

        // DTSTART
        QDate startDate = QDate::fromString(query.value("event_date").toString(), "yyyy-MM-dd");
        QTime startTime = QTime::fromString(query.value("start_time").toString(), "HH:mm");

        if (query.value("is_allday").toInt() == 1) {
            out << "DTSTART;VALUE=DATE:" << startDate.toString(Qt::ISODate) << "\n";
        } else {
            QDateTime startDateTime = QDateTime(startDate, startTime);
            out << "DTSTART:" << startDateTime.toUTC().toString(Qt::ISODateWithMs) << "Z\n"; // ISO 8601 format
        }

        // DTEND (if not all-day)
        if (query.value("is_allday").toInt() != 1) {
            QTime endTime = QTime::fromString(query.value("end_time").toString(), "HH:mm");
            QDateTime endDateTime = QDateTime(startDate, endTime);
            out << "DTEND:" << endDateTime.toUTC().toString(Qt::ISODateWithMs) << "Z\n"; // ISO 8601 format
        }

        // RRULE (recurrence)
        int recType = query.value("recurrence_type").toInt();
        if (recType == 4) { // Assuming '4' is internal code for Yearly
            out << "RRULE:FREQ=YEARLY\n";
        } else if (recType == 2) { // Assuming '2' is internal code for Monthly
            out << "RRULE:FREQ=MONTHLY\n";
        } else if (recType == 1) { // Assuming '1' is internal code for Weekly
            out << "RRULE:FREQ=WEEKLY\n";
        }
        // Add more recurrence types as needed

        // PRIORITY
        if (query.value("is_priority").toInt() == 1) {
            out << "PRIORITY:1\n"; // Highest priority
        } else {
            out << "PRIORITY:9\n"; // Lowest priority by default
        }

        // LOCATION
        out << "LOCATION:" << query.value("location").toString() << "\n";

        // DESCRIPTION
        out << "DESCRIPTION:" << query.value("description").toString().replace("\n", "\\n") << "\n";

        out << "END:VEVENT\n";
    }

    out << "END:VCALENDAR\n";
    file.close();

    QMessageBox::information(this, "Export", "Calendar exported successfully.");
}

/**
 * @brief structure for ical importer
 */
struct ParsedDate {
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    bool hasTime = false;
    bool isValid = false;
};

/**
 * @brief Import parser helper method.
 */
static bool parseInt(const QString &text, int &result)
{
    bool ok = false;
    result = text.toInteger<int>(&ok);
    return ok;
}
/**
 * @brief Import parser helper method
 * @param  date-time text string
 * @return A ParsedDate struture
 */
static ParsedDate parseICalDate(const QString &text)
{
    ParsedDate result;

    const int tPosition = text.indexOf('T');
    const QString datePart =
    (tPosition < 0) ? text : text.left(tPosition);

    // Accept YYYY-MM-DD and YYYYMMDD.
    if (datePart.contains('-')) {
        const QStringList parts = datePart.split('-');
        if (parts.size() != 3 ||
            !parseInt(parts[0], result.year) ||
            !parseInt(parts[1], result.month) ||
            !parseInt(parts[2], result.day)) {
            return result;
            }
    } else {
        if (datePart.size() != 8 ||
            !parseInt(datePart.mid(0, 4), result.year) ||
            !parseInt(datePart.mid(4, 2), result.month) ||
            !parseInt(datePart.mid(6, 2), result.day)) {
            return result;
            }
    }

    if (tPosition >= 0) {
        result.hasTime = true;

        QString timePart = text.mid(tPosition + 1);
        const int dotPosition = timePart.indexOf('.');
        if (dotPosition >= 0) {
            timePart = timePart.left(dotPosition);
        }

        // Remove trailing UTC marker(s), including the sample's "ZZ".
        while (timePart.endsWith('Z')) {
            timePart.chop(1);
        }

        // Accept HH:mm:ss or HHmmss.
        if (timePart.contains(':')) {
            const QStringList parts = timePart.split(':');
            if (parts.size() < 2 ||
                !parseInt(parts[0], result.hour) ||
                !parseInt(parts[1], result.minute)) {
                return ParsedDate();
                }
        } else {
            if (timePart.size() < 4 ||
                !parseInt(timePart.mid(0, 2), result.hour) ||
                !parseInt(timePart.mid(2, 2), result.minute)) {
                return ParsedDate();
                }
        }
    }

    result.isValid =
    QDate(result.year, result.month, result.day).isValid() &&
    (!result.hasTime ||
    QTime(result.hour, result.minute).isValid());

    return result;
}

/**
 * @brief Imports events from an iCalendar (.ics/.ical) file into the database.
 */
void MainWindow::importiCal()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this,
        "Import iCal",
        QString(),
                                                          "iCalendar Files (*.ics *.ical);;All Files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Import iCal",
                             "Cannot open file for reading.");
        return;
    }

    QTextStream inStream(&file);

    bool inEvent = false;
    QString summary;
    QString dtStart;
    QString dtEnd;
    QString description;
    QString location;
    int isPriority = 0;
    int recurrenceType = 0;
    int importedCount = 0;

    while (!inStream.atEnd()) {
        const QString line = inStream.readLine().trimmed();

        if (line == "BEGIN:VEVENT") {
            inEvent = true;

            // Reset values for each event.
            summary.clear();
            dtStart.clear();
            dtEnd.clear();
            description.clear();
            location.clear();
            isPriority = 0;
            recurrenceType = 0;
            continue;
        }

        if (!inEvent) {
            continue;
        }

        if (line == "END:VEVENT") {

            //const ParsedDate start = parseICalDate(dtStart);
            const ParsedDate start = parseICalDate(dtStart);

            if (!start.isValid) {
                qDebug() << "Skipping event with invalid DTSTART:" << summary << dtStart;
                inEvent = false;
                continue;
            }

            ParsedDate end;
            if (dtEnd.isEmpty() && !start.hasTime) {
                // All-day event with no DTEND: treat it as a single-day event.
                end = start;
            } else {
                end = parseICalDate(dtEnd);
            }

            if (!end.isValid) {
                qDebug() << "Skipping event with invalid DTEND:" << summary << dtEnd;
                inEvent = false;
                continue;
            }

            const bool isAllDay = !start.hasTime;
            const QString eventDate =
            QDate(start.year, start.month, start.day)
            .toString("yyyy-MM-dd");

            const QString startTime = isAllDay
            ? QString()
            : QTime(start.hour, start.minute).toString("HH:mm");

            const QString endTime = (isAllDay || !end.hasTime)
            ? QString()
            : QTime(end.hour, end.minute).toString("HH:mm");

            QSqlQuery query;
            if (!query.prepare(
                "INSERT INTO events "
                "(event_date, summary, description, location, "
                "start_time, end_time, is_allday, is_priority, "
                "recurrence_type) "
                "VALUES (:date, :summary, :description, :location, "
                ":start, :end, :all_day, :priority, :recurrence)")) {
                qDebug() << "Could not prepare event insert:"
                << query.lastError().text();
            inEvent = false;
            continue;
                }

                query.bindValue(":date", eventDate);
                query.bindValue(":summary", summary);
                query.bindValue(":description", description);
                query.bindValue(":location", location);
                query.bindValue(":start", startTime);
                query.bindValue(":end", endTime);
                query.bindValue(":all_day", isAllDay ? 1 : 0);
                query.bindValue(":priority", isPriority);
                query.bindValue(":recurrence", recurrenceType);

                if (!query.exec()) {
                    qDebug() << "Could not import event" << summary << ":"
                    << query.lastError().text();
                } else {
                    ++importedCount;
                }

                inEvent = false;
                continue;
        }

        const int colon = line.indexOf(':');
        if (colon < 0) {
            continue;
        }

        const QString property = line.left(colon);
        const QString value = line.mid(colon + 1);

        if (property == "SUMMARY") {
            summary = value;
        }

        else if (property == "DTSTART" || property.startsWith("DTSTART;")) {
            dtStart = value;
        }
        else if (property == "DTEND" || property.startsWith("DTEND;")) {
            dtEnd = value;
        }

        else if (property == "DESCRIPTION") {
            description = value;
        } else if (property == "LOCATION") {
            location = value;
        } else if (property == "PRIORITY") {
            bool ok = false;
            const int icalPriority = value.toInteger<int>(&ok);

            if (ok) {
                isPriority = (icalPriority >= 1 && icalPriority <= 4) ? 1 : 0;
            }

        } else if (property == "RRULE") {
            // Basic yearly recurrence support.
            recurrenceType = value.contains("FREQ=YEARLY") ? 4 : 0;
        }
    }

    file.close();

    markEventsOnCalendar();
    goToToday();

    QMessageBox::information(
        this, "Import iCal",
        QString("Imported ") + QString::number(importedCount) + " event(s).");
}
