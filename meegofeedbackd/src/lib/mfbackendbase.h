/* This file is part of meegofeedbackd
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MFBACKENDBASE_H
#define MFBACKENDBASE_H

#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QTimer>
#include <QDir>
#include <QMap>
#include <QString>

#include "mfbackendinterface.h"

class MfBackendBase : public QObject, public MfBackendInterface {
    Q_OBJECT
    Q_INTERFACES(MfBackendInterface)
public:

    enum State {
        Disconnected = 0,   // Initial state or lost connection
        Connecting,         // The backend tries to connect
        Connected           // The backend is connected
    };

    typedef enum {
        VolumeHigh = 0,
        VolumeMedium,
        VolumeLow,
        VolumeOff
    } playbackVolume;

    static const QString volumeHighID;
    static const QString volumeMediumID;
    static const QString volumeLowID;
    static const QString volumeOffID;

    /*!
     * \brief Class ctor.
     *
     * Class ctor.
     *
     */
    MfBackendBase();

    /*!
     * \brief Class dtor.
     *
     * Stop a connection timer if present.
     *
     */
    virtual ~MfBackendBase();

    /*!
     * \brief Returns plugin's name
     *
     * Human-readable string that uniquely identifies the plugin.
     *
     * This method works regardless of whether the plugin was initialized or not.
     *
     * \return String with plugin's name
     */
    virtual QString name() const = 0;

    /*!
     * \brief Detects the hardware
     *
     * The backend checks if the target hardware is available.
     *
     * \return True if the hardware is present, otherwise false.
     */
    virtual bool detectHardware() const = 0;

    /*!
     * \brief Initializes plugin
     *
     * \param durationLimit duration limit for the playback in milliseconds
     *
     * \return Whether the initialization was successful.
     */
    virtual bool init(int durationLimit) = 0;

public slots:
    /*!
     * \brief Called when device state changes
     *
     * This slot is called every time any status changes in the device.
     * Status is presented as key and value pairs. Currently supported
     * keys:
     * KEY: "display" POSSIBLE VALUES: "on", "off"
     *
     * \note It is up to the backend whether the state has any effects for
     *       the backend.
     *
     * \param newState newState contains the new state of the device in key
     *                 value pairs.
     */
    virtual void deviceStateChanged(const QMap<QString, QString> &newState) = 0;

protected:

    /*!
     * \brief Loads a feedback handle from a given directory
     *
     * Returns true if feedback handle was loaded successfully. Returns false if no valid feedback
     * handle was found in the given directory.
     *
     * \note A file with a 0-byte length should be considered a valid feedback. In this
     *       case true should be returned and returned feedbackHandle should be set as NULL. It
     *       is not required to unload feedback for 0-byte feedbacks. This means that
     *       unloadHandle() is not necessarily called.
     *
     * \param feedbackDir location of the feedback file to be loaded
     * \param feedbackHandle pointer to the feedback data to be loaded
     *
     * \return Whether the loading was successful
     */
    virtual bool loadHandle(const QDir &feedbackDir, void **feedbackHandle) = 0;

    /*!
     * \brief Unloads a given feedback handle
     *
     * \note feedbackHandle can contain a NULL pointer if 0-byte feedback was loaded with
     *       loadHandle().
     *
     * \param feedbackHandle pointer to the feedback data to be unloaded
     */
    virtual void unloadHandle(void **feedbackHandle) = 0;

    /*!
     * \brief Plays a given feedback with a given volume.
     *
     * This is a time critical method call that returns as soon as possible
     * (i.e., immediately) after the handle is being played.
     *
     * \param feedbackHandle handle of the feedback to be played
     * \param volume volume of the feedback to be played
     */
    virtual void play(void *feedbackHandle, playbackVolume volume) = 0;


    // The following methods are implemented in MfBackendBase and should not
    // be overriden!

public:

    /*!
     * \brief Current state.
     *
     * Current state of the backend.
     *
     * \return State.
     */
    State state() const;

    /*!
     * \brief Wrapper to load handles in a safe way.
     *
     * Safety checks before loading feedbacks.
     *
     * \param feedbackDir location of the loaded feedback file
     * \param feedbackHandle pointer to the loaded feedback data
     *
     * \return True if successful otherwise false.
     */
    bool loadHandleWrapper(const QDir &feedbackDir, void **feedbackHandle);

    /*!
     * \brief Wrapper to unload handles in a safe way.
     *
     * Safety checks before unloading feedbacks.
     *
     * \param feedbackHandle pointer to the loaded feedback data
     *
     */
    void unloadHandleWrapper(void **feedbackHandle);

    /*!
     * \brief Wrapper to play handles in a safe way.
     *
     * Safety checks before playing feedbacks.
     *
     * \param feedbackHandle pointer to the loaded feedback data
     *
     */
    void playWrapper(void *feedbackHandle, playbackVolume volume);

    /*!
     * \brief Prepares class for deletion.
     *
     * Waits until any possible concurrent initialization has been
     * finished and disconnects all signals.
     */
    void cleanup();

signals:

    /*!
     * \brief Signal to notify the daemon about the lost connection.
     *
     * Signal to notify the daemon about the lost connection.
     *
     */
    void lostConnection();

    /*!
     * \brief Signal to notify the daemon about the established connection.
     *
     * Signal to notify the daemon about the established connection.
     *
     */
    void establishedConnection();

private slots:

    /*!
     * \brief Slot to schedule a connection attempt
     *
     * Slot to schedule a connection attempt.
     *
     */
    void tryConnect();

    /*!
     * \brief Slot to check the result of a connection attempt
     *
     * Slot to check the result of a connection attempt.
     *
     */
    void connectionDone();

public slots:

    /*!
     * \brief Slot to connect the backend
     *
     * Slot to connect the backend.
     *
     */
    void connecting();

protected:

    /*
     * Contains the intervals, in milliseconds, to wait before attempting
     * to connect the backend.
     * list[0] is the time to wait before trying to connect for the first time.
     * If the first attempt fails then it waits list[1] milliseconds before attempting
     * connecting for the second time and so on.
     *
     */
    QList<int> connectionIntervals;

    /*
     * Timer for connection
     */
    QTimer timer;

    /*
     * The number of connection attempts that have consecutively failed.
     * This value is zeroed when a connection is established.
     */
    int failedConnections;

    /*
     * The current state of the backend
     */
    State currentState;

    /*
     * Get the result of a connection attempt
     */
    QFutureWatcher<bool> resultConnection;

    /*
     * Run a connection attempt in a separate thread
     */
    QFuture<bool> runConnection;
};

#endif
