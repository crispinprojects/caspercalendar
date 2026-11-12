 /*
  * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
  * SPDX-License-Identifier: GPL-2.0-or-later
  */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QListWidget>
#include <QColor>
#include <QString>
#include <QDate>
#include <QList>
#include <QSettings>
#include <QIcon>
#include "synthesizer.h"
#include "dictionary.h"
#include "preferences.h"
#include "calendarevent.h"

#define PROJECT_VERSION "0.1.2"


class MainWindow : public QMainWindow
{
    CS_OBJECT(MainWindow)

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private:
    void about();
    void information();
    void updateEventList();
    void goToToday();
    void speakTime();
    void speakEvents();
    void newEvent();
    void editEvent();
    void deleteEvent();
    void deleteAllEvents();
    void setPreferences();
    QString getDatePhrase(const QDate &date);
    QString getTimePhrase(int hour, int minute, bool isAllDay);
    QString getOrdinalStr(int day);
    QString getCardinalStr(int number);
    QList<CalendarEvent> getEventsForDate(const QDate &date);
    void markEventsOnCalendar();
    void onCalendarPageChanged();
    QString getUpcomingEventsPhrase();
    void exportiCal();
    void importiCal();

private:
    Dictionary* m_voiceDict;
    Synthesizer* m_engine;

    //preferences
    bool m_talk;
    bool m_talk_startup;
    bool m_upcoming;
    int m_upcoming_days;
    int m_tempo;
    int m_font_size;
    QColor m_eventColor;
    QColor m_priorityColor;



    // Declare mainCalendar and eventList as member variables
    QCalendarWidget* mainCalendar;
    QListWidget* eventList;
};

#endif // MAINWINDOW_H
