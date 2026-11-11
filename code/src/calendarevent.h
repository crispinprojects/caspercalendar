/**
 * @file calendarevent.h
 *
 * This file contains the CalendarEvent structure definition.
 *
 * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CALENDAREVENT_H
#define CALENDAREVENT_H

#include <QDate>
#include <QString>
#include <QTime>

/**
 * @struct CalendarEvent
 *
 * This structure represents an event in a calendar.
 */
struct CalendarEvent
{
  int m_id = 0; /**< Unique identifier for the event */

  /**
   * Date when the event occurs.
   */
  QDate m_eventDate;

  /**
   * Summary of the event.
   */
  QString m_summary;

  /**
   * Detailed description of the event.
   */
  QString m_description;

  /**
   * Location where the event will take place.
   */
  QString m_location;

  /**
   * Start time for the event.
   */
  QTime m_startTime;

  /**
   * End time for the event.
   */
  QTime m_endTime;

  bool m_isAllDay = false; /**< Whether the event is all day long */

  bool m_isPriority = false; /**< Whether the event has high priority */

  int m_recurrenceType = 0; // 0 = none, 4 = yearly
};

#endif // CALENDAREVENT_H

