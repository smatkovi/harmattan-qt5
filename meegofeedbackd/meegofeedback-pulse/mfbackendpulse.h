/* This file is part of meegofeedbackd
 *
 * The audio backend for PulseAudio, written anew for the Qt 5 port: Nokia
 * released only the dummy backends. Each feedback's audio.wav is uploaded
 * to PulseAudio's sample cache when it is loaded, and played from there,
 * which keeps a tap's sound within the latency limit.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 */

#ifndef MFBACKENDPULSE_H
#define MFBACKENDPULSE_H

#include <mfbackendbase.h>

#include <QByteArray>
#include <QDir>
#include <QObject>

struct pa_threaded_mainloop;
struct pa_context;

class MfBackendPulse : public MfBackendBase {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "meego.MfBackendInterface/1.1")
    Q_INTERFACES(MfBackendInterface)

public:
    MfBackendPulse();
    virtual ~MfBackendPulse();

    QString name() const;
    bool detectHardware() const;
    bool init(int durationLimit);
    bool loadHandle(const QDir &feedbackDir, void **feedbackHandle);
    void unloadHandle(void **feedbackHandle);
    void play(void *feedbackHandle, playbackVolume volume);
    bool isReady();

    // for PulseAudio's callbacks
    void contextChanged();

public slots:
    void deviceStateChanged(const QMap<QString, QString> &newState);

private:
    void disconnectPulse();
    bool upload(const QByteArray &sampleName, const QByteArray &pcm,
                int format, int rate, int channels);

    pa_threaded_mainloop *mainloop;
    pa_context *context;
    int durationLimit;
    unsigned sampleCount;
};

#endif
