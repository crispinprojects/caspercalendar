/*
 * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "preferences.h"
#include <QMessageBox>
#include <QColorDialog>

/**
 * @brief Constructs a new Preferences dialog.
 * @param parent The parent widget.
 */
Preferences::Preferences(QWidget *parent)
: QDialog(parent),
checkTalk(new QCheckBox("Talk", this)),
checkTalkStartup(new QCheckBox("Talk At Startup", this)),
checkUpcoming(new QCheckBox("Talk Upcoming", this)),
spinUpcomingDays(new QSpinBox(this)),
spinTempo(new QSpinBox(this)),
btnEventColour(new QPushButton("Event Colour", this)),
btnPriorityColour(new QPushButton("Priority Colour", this)),
spinFontSize(new QSpinBox(this)),
btnReset(new QPushButton("Reset", this)),
btnSave(new QPushButton("Save", this))
{
    setWindowTitle("Preferences");

    // Set layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->addWidget(checkTalk);
    layout1->addWidget(checkTalkStartup);
    mainLayout->addLayout(layout1);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(checkUpcoming);
    layout2->addWidget(new QLabel("Days", this));
    spinUpcomingDays->setRange(1, 365); // Adjust range as needed
    layout2->addWidget(spinUpcomingDays);
    mainLayout->addLayout(layout2);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(new QLabel("Speak Tempo", this));
    spinTempo->setRange(5, 25); // Minimum 5, Maximum 25
    layout3->addWidget(spinTempo);
    mainLayout->addLayout(layout3);

    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->addWidget(btnEventColour);
    layout4->addWidget(btnPriorityColour);
    mainLayout->addLayout(layout4);

    QHBoxLayout *layout5 = new QHBoxLayout();
    layout5->addWidget(new QLabel("Font Size", this));
    spinFontSize->setRange(10, 24); // Minimum 10, Maximum 24
    layout5->addWidget(spinFontSize);
    mainLayout->addLayout(layout5);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(btnReset);
    buttonLayout->addWidget(btnSave);
    mainLayout->addLayout(buttonLayout);

    // Connect signals and slots
    connect(btnSave, &QPushButton::clicked, this, &Preferences::onSaveClicked);
    connect(btnReset, &QPushButton::clicked, this, &Preferences::onResetClicked);

    // Connect the color buttons to their respective methods
    connect(btnEventColour, &QPushButton::clicked, this, &Preferences::on_btnEventColor_clicked);
    connect(btnPriorityColour, &QPushButton::clicked, this, &Preferences::on_btnPriorityColor_clicked);
}

/**
 * @brief Destroys the Preferences dialog.
 */
Preferences::~Preferences()
{
    // Cleanup if necessary
}

// Setter Methods

/**
 * @brief Sets whether speech functionality is enabled.
 * @param talk true to enable speech, false to disable.
 */
void Preferences::setTalk(bool talk) {
    checkTalk->setChecked(talk);
}

/**
 * @brief Sets whether speech should start automatically with the application.
 * @param startup true to enable startup speech, false to disable.
 */
void Preferences::setTalkStartup(bool startup) {
    checkTalkStartup->setChecked(startup);
}

/**
 * @brief Sets whether to announce upcoming events.
 * @param upcoming true to enable announcements, false to disable.
 */
void Preferences::setUpcoming(bool upcoming) {
    checkUpcoming->setChecked(upcoming);
}

/**
 * @brief Sets the number of days ahead for upcoming event announcements.
 * @param days The number of days (1-365).
 */
void Preferences::setUpcomingDays(int days) {
    spinUpcomingDays->setValue(days);
}

/**
 * @brief Sets the speech tempo setting.
 * @param tempo The tempo value (5-25).
 */
void Preferences::setTempo(int tempo) {
    spinTempo->setValue(tempo);
}

/**
 * @brief Sets the event color.
 * @param color The QColor to use for events.
 */
void Preferences::setEventColour(const QColor &color) {
    m_currentEventColor = color;
    QString styleSheetTemplate = "background-color: %1;";
    btnEventColour->setStyleSheet(styleSheetTemplate.replace("%1", color.name()));
}

/**
 * @brief Sets the priority color.
 * @param color The QColor to use for priorities.
 */
void Preferences::setPriorityColour(const QColor &color) {
    m_currentPriorityColor = color;
    QString styleSheetTemplate = "background-color: %1;";
    btnPriorityColour->setStyleSheet(styleSheetTemplate.replace("%1", color.name()));
}

/**
 * @brief Sets the font size setting.
 * @param size The font size value (10-24).
 */
void Preferences::setFontSize(int size) {
    spinFontSize->setValue(size);
}

// Other Methods

/**
 * @brief Gets whether speech functionality is enabled.
 * @return true if speech is enabled, false otherwise.
 */
bool Preferences::getTalk() const {
    return checkTalk->isChecked();
}

/**
 * @brief Gets whether speech should start automatically with the application.
 * @return true if startup speech is enabled, false otherwise.
 */
bool Preferences::getTalkStartup() const {
    return checkTalkStartup->isChecked();
}

/**
 * @brief Gets whether to announce upcoming events.
 * @return true if upcoming events should be announced, false otherwise.
 */
bool Preferences::getUpcoming() const {
    return checkUpcoming->isChecked();
}

/**
 * @brief Gets the number of days ahead for upcoming event announcements.
 * @return The number of days (1-365).
 */
int Preferences::getUpcomingDays() const {
    return spinUpcomingDays->value();
}

/**
 * @brief Gets the speech tempo setting.
 * @return The tempo value (5-25).
 */
int Preferences::getTempo() const {
    return spinTempo->value();
}

/**
 * @brief Gets the current event color.
 * @return The QColor representing the event color.
 */
QColor Preferences::getEventColour() const {
    return m_currentEventColor;
}

/**
 * @brief Gets the current priority color.
 * @return The QColor representing the priority color.
 */
QColor Preferences::getPriorityColour() const {
    return m_currentPriorityColor;
}

/**
 * @brief Gets the current font size setting.
 * @return The font size value (10-24).
 */
int Preferences::getFontSize() const {
    return spinFontSize->value();
}

/**
 * @brief Handles the save button click event.
 * Saves current preferences and closes the dialog.
 */
void Preferences::onSaveClicked()
{
    qDebug() << "Preferences saved";

    // Close dialog
    accept();
}

/**
 * @brief Handles the reset button click event.
 * Resets all settings to their default values.
 */
void Preferences::onResetClicked()
{
    // Reset all settings to default values
    checkTalk->setChecked(true);
    checkTalkStartup->setChecked(false);
    checkUpcoming->setChecked(false);
    spinUpcomingDays->setValue(7); // Default 7 days
    spinTempo->setValue(17); // Default tempo
    spinFontSize->setValue(14); // Default font size

    // Reset colors to default values
    setEventColour(Qt::yellow);
    setPriorityColour(Qt::darkMagenta);
}

/**
 * @brief Handles the event color button click event.
 * Opens a color dialog for selecting the event color.
 */
void Preferences::on_btnEventColor_clicked()
{
    QColor color = QColorDialog::getColor(m_currentEventColor, this, "Select Event Color");
    if (color.isValid()) {
        m_currentEventColor = color;
        // Set the button's background to the chosen color as a preview
        QString styleSheetTemplate = "background-color: %1;";
        btnEventColour->setStyleSheet(styleSheetTemplate.replace("%1", color.name()));
    }
}

/**
 * @brief Handles the priority color button click event.
 * Opens a color dialog for selecting the priority color.
 */
void Preferences::on_btnPriorityColor_clicked()
{
    QColor color = QColorDialog::getColor(m_currentPriorityColor, this, "Select Priority Color");
    if (color.isValid()) {
        m_currentPriorityColor = color;
        // Set the button's background to the chosen color as a preview
        QString styleSheetTemplate = "background-color: %1;";
        btnPriorityColour->setStyleSheet(styleSheetTemplate.replace("%1", color.name()));
    }
}
