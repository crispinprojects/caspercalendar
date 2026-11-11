/*
 * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef EVENTEDITOR_H
#define EVENTEDITOR_H

#include <QDialog>
#include <QLineEdit>
#include <QTimeEdit>
#include <QCheckBox>
#include <QPushButton>

/**
 * @class EventEditorDialog
 * @brief A dialog for creating and editing calendar events.
 *
 * This class provides a user interface for entering event details including summary,
 * description, location, timing, and various event options like all-day, yearly repetition,
 * and priority status.
 */
class EventEditorDialog : public QDialog
{
  CS_OBJECT(EventEditorDialog)

public:
  /**
   * @brief Constructs a new Event Editor dialog.
   * @param parent The parent widget.
   */
  explicit EventEditorDialog(QWidget *parent = nullptr);

  /** @brief Destroys the Event Editor dialog. */
  virtual ~EventEditorDialog() = default;

  // UI Elements - exposed for external access
  /**
   * @brief Pointer to the summary line edit field.
   * Allows external code to read or modify the event summary text.
   */
  QLineEdit *lineSummary;

  /**
   * @brief Pointer to the description line edit field.
   * Allows external code to read or modify the event description text.
   */
  QLineEdit *lineDescription;

  /**
   * @brief Pointer to the location line edit field.
   * Allows external code to read or modify the event location text.
   */
  QLineEdit *lineLocation;

  /**
   * @brief Pointer to the start time edit field.
   * Allows external code to read or modify the event start time.
   */
  QTimeEdit *timeStart;

  /**
   * @brief Pointer to the end time edit field.
   * Allows external code to read or modify the event end time.
   */
  QTimeEdit *timeEnd;

  /**
   * @brief Pointer to the all-day checkbox.
   * Indicates whether this is an all-day event.
   */
  QCheckBox *checkAllDay;

  /**
   * @brief Pointer to the yearly repetition checkbox.
   * Indicates whether this event repeats yearly.
   */
  QCheckBox *chkYearly;

  /**
   * @brief Pointer to the high priority checkbox.
   * Indicates whether this is a high priority event.
   */
  QCheckBox *chkPriority;

  /**
   * @brief Pointer to the save button.
   * Triggers event saving when clicked.
   */
  QPushButton *btnSave;
};

#endif // EVENTEDITOR_H
