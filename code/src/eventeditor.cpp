 /*
  * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
  * SPDX-License-Identifier: GPL-2.0-or-later
  */
#include "eventeditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

/**
 * @brief Constructs a new Event Editor dialog.
 *
 * Initializes the dialog with all necessary UI elements for creating/editing events,
 * including text fields, time selectors, checkboxes, and action buttons. Sets up
 * appropriate layouts and connects signals to slots.
 *
 * @param parent The parent widget.
 */
EventEditorDialog::EventEditorDialog(QWidget *parent)
   : QDialog(parent)
{
   setWindowTitle("Event Editor");
   resize(450, 400);

   // Configure matching high-visibility font size
   QFont globalFont("Sans Serif", 14);
   setFont(globalFont);

   // Core vertical layout block
   auto *mainVerticalLayout = new QVBoxLayout(this);

   // Summary Entry Row
   auto *rowSummary = new QHBoxLayout();
   auto *labelSummary = new QLabel("Summary", this);
   lineSummary = new QLineEdit(this);
   rowSummary->addWidget(labelSummary);
   rowSummary->addWidget(lineSummary);
   mainVerticalLayout->addLayout(rowSummary);

   // Description Entry Row
   auto *rowDescription = new QHBoxLayout();
   auto *labelDescription = new QLabel("Description", this);
   lineDescription = new QLineEdit(this);
   rowDescription->addWidget(labelDescription);
   rowDescription->addWidget(lineDescription);
   mainVerticalLayout->addLayout(rowDescription);

   // Location Entry Row
   auto *rowLocation = new QHBoxLayout();
   auto *labelLocation = new QLabel("Location", this);
   lineLocation = new QLineEdit(this);
   rowLocation->addWidget(labelLocation);
   rowLocation->addWidget(lineLocation);
   mainVerticalLayout->addLayout(rowLocation);

   // Timing Row (Start Time & End Time)
   auto *rowTime = new QHBoxLayout();
   auto *labelStartTime = new QLabel("Start Time", this);
   timeStart = new QTimeEdit(this);
   timeStart->setDisplayFormat("HH:mm");

   auto *labelEndTime = new QLabel("End Time", this);
   timeEnd = new QTimeEdit(this);
   timeEnd->setDisplayFormat("HH:mm");

   rowTime->addWidget(labelStartTime);
   rowTime->addWidget(timeStart);
   rowTime->addWidget(labelEndTime);
   rowTime->addWidget(timeEnd);
   mainVerticalLayout->addLayout(rowTime);

   //  Configuration Checkboxes Row
   auto *rowCheckBoxes = new QHBoxLayout();
   checkAllDay = new QCheckBox("All Day", this);
   chkYearly = new QCheckBox("Repeats Yearly", this);
   chkPriority = new QCheckBox("High Priority", this);

   rowCheckBoxes->addWidget(checkAllDay);
   rowCheckBoxes->addWidget(chkYearly);
   rowCheckBoxes->addWidget(chkPriority);
   mainVerticalLayout->addLayout(rowCheckBoxes);

   // Action Execution Button
   btnSave = new QPushButton("Save Event", this);
   mainVerticalLayout->addWidget(btnSave);

   // connect save button with event dialog with an "Accepted" return signal status to close
   connect(btnSave, &QPushButton::clicked, this, &EventEditorDialog::accept);
}
