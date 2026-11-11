/*
 * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

/**
 * @class Preferences
 * @brief A dialog for managing application preferences.
 *
 * This class provides a user interface for configuring various application settings
 * including speech options, color schemes, and display preferences.
 */
class Preferences : public QDialog
{
    CS_OBJECT(Preferences)

public:
    /**
     * @brief Constructs a new Preferences dialog.
     * @param parent The parent widget.
     */
    explicit Preferences(QWidget *parent = nullptr);

    /** @brief Destroys the Preferences dialog. */
    virtual ~Preferences();

    // Getters
    /**
     * @brief Gets whether speech functionality is enabled.
     * @return true if speech is enabled, false otherwise.
     */
    bool getTalk() const;

    /**
     * @brief Gets whether speech should start automatically with the application.
     * @return true if startup speech is enabled, false otherwise.
     */
    bool getTalkStartup() const;

    /**
     * @brief Gets whether to announce upcoming events.
     * @return true if upcoming events should be announced, false otherwise.
     */
    bool getUpcoming() const;

    /**
     * @brief Gets the number of days ahead for upcoming event announcements.
     * @return The number of days (1-365).
     */
    int getUpcomingDays() const;

    /**
     * @brief Gets the speech tempo setting.
     * @return The tempo value (5-25).
     */
    int getTempo() const;

    /**
     * @brief Gets the current event color.
     * @return The QColor representing the event color.
     */
    QColor getEventColour() const;

    /**
     * @brief Gets the current priority color.
     * @return The QColor representing the priority color.
     */
    QColor getPriorityColour() const;

    /**
     * @brief Gets the current font size setting.
     * @return The font size value (10-24).
     */
    int getFontSize() const;

    // Setters
    /**
     * @brief Sets whether speech functionality is enabled.
     * @param talk true to enable speech, false to disable.
     */
    void setTalk(bool talk);

    /**
     * @brief Sets whether speech should start automatically with the application.
     * @param startup true to enable startup speech, false to disable.
     */
    void setTalkStartup(bool startup);

    /**
     * @brief Sets whether to announce upcoming events.
     * @param upcoming true to enable announcements, false to disable.
     */
    void setUpcoming(bool upcoming);

    /**
     * @brief Sets the number of days ahead for upcoming event announcements.
     * @param days The number of days (1-365).
     */
    void setUpcomingDays(int days);

    /**
     * @brief Sets the speech tempo setting.
     * @param tempo The tempo value (5-25).
     */
    void setTempo(int tempo);

    /**
     * @brief Sets the event color.
     * @param color The QColor to use for events.
     */
    void setEventColour(const QColor &color);

    /**
     * @brief Sets the priority color.
     * @param color The QColor to use for priorities.
     */
    void setPriorityColour(const QColor &color);

    /**
     * @brief Sets the font size setting.
     * @param size The font size value (10-24).
     */
    void setFontSize(int size);

private:
    // UI Elements
    QCheckBox *checkTalk;          /**< Checkbox for enabling/disabling speech */
    QCheckBox *checkTalkStartup;   /**< Checkbox for startup speech option */
    QCheckBox *checkUpcoming;      /**< Checkbox for upcoming events announcement */
    QSpinBox *spinUpcomingDays;    /**< Spin box for days ahead setting (1-365) */
    QSpinBox *spinTempo;           /**< Spin box for speech tempo setting (5-25) */
    QPushButton *btnEventColour;   /**< Button to select event color */
    QPushButton *btnPriorityColour;/**< Button to select priority color */
    QSpinBox *spinFontSize;        /**< Spin box for font size setting (10-24) */
    QPushButton *btnReset;         /**< Reset button to restore default settings */
    QPushButton *btnSave;          /**< Save button to apply changes */

private:
    // Private methods
    /**
     * @brief Handles the save button click event.
     * Saves current preferences and closes the dialog.
     */
    void onSaveClicked();

    /**
     * @brief Handles the reset button click event.
     * Resets all settings to their default values.
     */
    void onResetClicked();

    /**
     * @brief Handles the event color button click event.
     * Opens a color dialog for selecting the event color.
     */
    void on_btnEventColor_clicked();

    /**
     * @brief Handles the priority color button click event.
     * Opens a color dialog for selecting the priority color.
     */
    void on_btnPriorityColor_clicked();

    // Member variables
    QColor m_currentEventColor;    /**< Current event color setting */
    QColor m_currentPriorityColor; /**< Current priority color setting */

};

#endif // PREFERENCES_H
