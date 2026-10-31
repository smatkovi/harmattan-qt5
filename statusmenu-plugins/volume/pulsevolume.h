/*
 * The volume of PulseAudio's default output (the status menu's slider):
 * read, followed and set through libpulse on the GLib main loop Qt runs on.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 */
#ifndef PULSEVOLUME_H
#define PULSEVOLUME_H

#include <QObject>
#include <QByteArray>
#include <QTimer>

struct pa_context;
struct pa_glib_mainloop;
struct pa_sink_info;
struct pa_server_info;

class PulseVolume : public QObject
{
    Q_OBJECT

public:
    explicit PulseVolume(QObject *parent = 0);
    virtual ~PulseVolume();

    // 0 to 100 (percent of the normal volume), -1 before it is known
    int volume() const { return currentVolume; }
    bool muted() const { return currentMute; }

public slots:
    void setVolume(int percent);
    /*!
     * Follows PulseAudio's volume only while @active (the menu shows the
     * row): otherwise it asks PulseAudio to tell nothing, and does not
     * try to reach it when it is not there
     */
    void setActive(bool active);

signals:
    void volumeChanged(int percent, bool muted);

public:
    // for libpulse's callbacks
    void contextChanged();
    void serverInfo(const pa_server_info *info);
    void sinkInfo(const pa_sink_info *info);
    void sinkChanged();

private slots:
    void connectPulse();

private:
    void dropContext();
    void subscribe();

    pa_glib_mainloop *mainloop;
    pa_context *context;
    bool active;
    // another try to reach PulseAudio, later each time
    QTimer retryTimer;
    int retryDelay;
    QByteArray sinkName;
    int channels;
    int currentVolume;
    bool currentMute;
};

#endif
