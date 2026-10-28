/* This file is part of meegofeedbackd
 *
 * The audio backend for PulseAudio, written anew for the Qt 5 port (see
 * mfbackendpulse.h).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 */

#include "mfbackendpulse.h"

#include <QFile>
#include <QMetaObject>
#include <QtDebug>

#include <pulse/pulseaudio.h>

namespace {

// The playback volumes of the theme's feedback volume levels
double linearVolume(MfBackendBase::playbackVolume volume)
{
    switch (volume) {
    case MfBackendBase::VolumeHigh:
        return 1.0;
    case MfBackendBase::VolumeMedium:
        return 0.6;
    case MfBackendBase::VolumeLow:
        return 0.3;
    default:
        return 0.0;
    }
}

quint16 le16(const char *p)
{
    return quint8(p[0]) | quint8(p[1]) << 8;
}

quint32 le32(const char *p)
{
    return quint32(le16(p)) | quint32(le16(p + 2)) << 16;
}

// The PCM data of a RIFF WAVE file and its sample spec
bool readWave(const QByteArray &file, QByteArray *pcm, pa_sample_spec *spec)
{
    if (file.size() < 12 || !file.startsWith("RIFF") || file.mid(8, 4) != "WAVE")
        return false;

    bool haveFormat = false;
    int pos = 12;
    while (pos + 8 <= file.size()) {
        const char *chunk = file.constData() + pos;
        quint32 size = le32(chunk + 4);
        int body = pos + 8;
        if (quint32(file.size() - body) < size)
            size = file.size() - body;

        if (!qstrncmp(chunk, "fmt ", 4) && size >= 16) {
            const char *f = file.constData() + body;
            quint16 tag = le16(f);
            quint16 bits = le16(f + 14);
            spec->channels = le16(f + 2);
            spec->rate = le32(f + 4);
            if (tag == 3 && bits == 32)
                spec->format = PA_SAMPLE_FLOAT32LE;
            else if (tag != 1)
                return false;
            else if (bits == 8)
                spec->format = PA_SAMPLE_U8;
            else if (bits == 16)
                spec->format = PA_SAMPLE_S16LE;
            else if (bits == 24)
                spec->format = PA_SAMPLE_S24LE;
            else if (bits == 32)
                spec->format = PA_SAMPLE_S32LE;
            else
                return false;
            haveFormat = true;
        } else if (!qstrncmp(chunk, "data", 4) && haveFormat) {
            *pcm = file.mid(body, size);
            return pa_sample_spec_valid(spec) && !pcm->isEmpty();
        }
        // chunks are padded to an even size
        pos = body + size + (size & 1);
    }
    return false;
}

void contextStateCallback(pa_context *, void *userdata)
{
    static_cast<MfBackendPulse *>(userdata)->contextChanged();
}

void streamStateCallback(pa_stream *, void *userdata)
{
    pa_threaded_mainloop_signal(static_cast<pa_threaded_mainloop *>(userdata), 0);
}

}

MfBackendPulse::MfBackendPulse() : MfBackendBase(),
    mainloop(0), context(0), durationLimit(0), sampleCount(0)
{
}

MfBackendPulse::~MfBackendPulse()
{
    disconnectPulse();
}

QString MfBackendPulse::name() const
{
    return QString("pulse");
}

bool MfBackendPulse::detectHardware() const
{
    return true;
}

bool MfBackendPulse::init(int durationLimit)
{
    this->durationLimit = durationLimit;
    disconnectPulse();

    mainloop = pa_threaded_mainloop_new();
    if (!mainloop)
        return false;

    pa_proplist *props = pa_proplist_new();
    pa_proplist_sets(props, PA_PROP_APPLICATION_NAME, "meegofeedbackd");
    pa_proplist_sets(props, PA_PROP_APPLICATION_ID, "com.nokia.meegofeedbackd");
    pa_proplist_sets(props, PA_PROP_MEDIA_ROLE, "event");
    context = pa_context_new_with_proplist(pa_threaded_mainloop_get_api(mainloop),
                                           "meegofeedbackd", props);
    pa_proplist_free(props);
    if (!context) {
        disconnectPulse();
        return false;
    }
    pa_context_set_state_callback(context, contextStateCallback, this);

    pa_threaded_mainloop_lock(mainloop);
    bool ok = pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL) >= 0 &&
              pa_threaded_mainloop_start(mainloop) >= 0;
    pa_context_state_t state = PA_CONTEXT_UNCONNECTED;
    while (ok) {
        state = pa_context_get_state(context);
        if (state == PA_CONTEXT_READY || !PA_CONTEXT_IS_GOOD(state))
            break;
        pa_threaded_mainloop_wait(mainloop);
    }
    pa_threaded_mainloop_unlock(mainloop);

    if (!ok || state != PA_CONTEXT_READY) {
        qWarning() << "MfBackendPulse: no PulseAudio:"
                   << pa_strerror(pa_context_errno(context));
        disconnectPulse();
        return false;
    }
    currentState = Connected;
    return true;
}

void MfBackendPulse::contextChanged()
{
    pa_context_state_t state = pa_context_get_state(context);

    pa_threaded_mainloop_signal(mainloop, 0);
    if (!PA_CONTEXT_IS_GOOD(state) && currentState == Connected) {
        // PulseAudio went away: reconnect from Qt's thread (MfBackendBase)
        currentState = Disconnected;
        QMetaObject::invokeMethod(this, "lostConnection", Qt::QueuedConnection);
    }
}

void MfBackendPulse::disconnectPulse()
{
    if (mainloop)
        pa_threaded_mainloop_stop(mainloop);
    if (context) {
        pa_context_set_state_callback(context, NULL, NULL);
        pa_context_disconnect(context);
        pa_context_unref(context);
        context = 0;
    }
    if (mainloop) {
        pa_threaded_mainloop_free(mainloop);
        mainloop = 0;
    }
}

bool MfBackendPulse::upload(const QByteArray &sampleName, const QByteArray &pcm,
                            int format, int rate, int channels)
{
    pa_sample_spec spec;
    spec.format = pa_sample_format_t(format);
    spec.rate = rate;
    spec.channels = channels;

    pa_threaded_mainloop_lock(mainloop);
    pa_stream *stream = pa_stream_new(context, sampleName.constData(), &spec, NULL);
    bool ok = stream != 0;
    if (ok) {
        pa_stream_set_state_callback(stream, streamStateCallback, mainloop);
        ok = pa_stream_connect_upload(stream, pcm.size()) >= 0;
    }
    while (ok) {
        pa_stream_state_t state = pa_stream_get_state(stream);
        if (state == PA_STREAM_READY)
            break;
        if (!PA_STREAM_IS_GOOD(state))
            ok = false;
        else
            pa_threaded_mainloop_wait(mainloop);
    }
    if (ok) {
        ok = pa_stream_write(stream, pcm.constData(), pcm.size(), NULL, 0,
                             PA_SEEK_RELATIVE) >= 0 &&
             pa_stream_finish_upload(stream) >= 0;
    }
    // the upload is done when the stream ends
    while (ok) {
        pa_stream_state_t state = pa_stream_get_state(stream);
        if (state == PA_STREAM_TERMINATED)
            break;
        if (state == PA_STREAM_FAILED)
            ok = false;
        else
            pa_threaded_mainloop_wait(mainloop);
    }
    if (stream) {
        pa_stream_set_state_callback(stream, NULL, NULL);
        pa_stream_unref(stream);
    }
    pa_threaded_mainloop_unlock(mainloop);
    return ok;
}

bool MfBackendPulse::loadHandle(const QDir &feedbackDir, void **feedbackHandle)
{
    (*feedbackHandle) = 0;

    QFile file(feedbackDir.filePath("audio.wav"));
    if (!file.exists())
        return false;
    // An empty file is a feedback without sound on purpose.
    if (file.size() == 0)
        return true;
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray pcm;
    pa_sample_spec spec;
    if (!readWave(file.readAll(), &pcm, &spec)) {
        qWarning() << "MfBackendPulse: not a PCM WAVE file:" << file.fileName();
        return false;
    }
    // no longer than the daemon's limit (feedback-duration-limit)
    if (durationLimit > 0) {
        size_t limit = pa_usec_to_bytes(pa_usec_t(durationLimit) * 1000, &spec);
        if (size_t(pcm.size()) > limit)
            pcm.truncate(limit);
    }

    QByteArray sampleName = "meegofeedback-" + QByteArray::number(++sampleCount);
    if (!upload(sampleName, pcm, spec.format, spec.rate, spec.channels)) {
        qWarning() << "MfBackendPulse: upload failed:" << file.fileName();
        return false;
    }
    (*feedbackHandle) = new QByteArray(sampleName);
    return true;
}

void MfBackendPulse::unloadHandle(void **feedbackHandle)
{
    QByteArray *sampleName = static_cast<QByteArray *>(*feedbackHandle);

    if (mainloop && context) {
        pa_threaded_mainloop_lock(mainloop);
        if (pa_context_get_state(context) == PA_CONTEXT_READY) {
            pa_operation *op = pa_context_remove_sample(context, sampleName->constData(),
                                                        NULL, NULL);
            if (op)
                pa_operation_unref(op);
        }
        pa_threaded_mainloop_unlock(mainloop);
    }
    delete sampleName;
    (*feedbackHandle) = 0;
}

void MfBackendPulse::play(void *feedbackHandle, playbackVolume volume)
{
    const QByteArray *sampleName = static_cast<const QByteArray *>(feedbackHandle);
    double linear = linearVolume(volume);

    if (linear <= 0.0 || !mainloop || !context)
        return;
    pa_threaded_mainloop_lock(mainloop);
    if (pa_context_get_state(context) == PA_CONTEXT_READY) {
        pa_operation *op = pa_context_play_sample(context, sampleName->constData(), NULL,
                                                  pa_sw_volume_from_linear(linear),
                                                  NULL, NULL);
        if (op)
            pa_operation_unref(op);
    }
    pa_threaded_mainloop_unlock(mainloop);
}

bool MfBackendPulse::isReady()
{
    return currentState == Connected;
}

void MfBackendPulse::deviceStateChanged(const QMap<QString, QString> &newState)
{
    Q_UNUSED(newState);
}
