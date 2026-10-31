/*
 * The volume of PulseAudio's default output (see pulsevolume.h).
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 */
#include "pulsevolume.h"

#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>

namespace {

void contextStateCallback(pa_context *, void *self)
{
    static_cast<PulseVolume *>(self)->contextChanged();
}

void serverInfoCallback(pa_context *, const pa_server_info *info, void *self)
{
    static_cast<PulseVolume *>(self)->serverInfo(info);
}

void sinkInfoCallback(pa_context *, const pa_sink_info *info, int eol, void *self)
{
    if (eol == 0 && info)
        static_cast<PulseVolume *>(self)->sinkInfo(info);
}

void subscribeCallback(pa_context *, pa_subscription_event_type_t type, uint32_t, void *self)
{
    int facility = type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
    if (facility == PA_SUBSCRIPTION_EVENT_SINK || facility == PA_SUBSCRIPTION_EVENT_SERVER)
        static_cast<PulseVolume *>(self)->sinkChanged();
}

}

// the first retry, and the longest wait between two
static const int FirstRetry = 3000;
static const int LongestRetry = 60000;

PulseVolume::PulseVolume(QObject *parent) : QObject(parent),
    mainloop(pa_glib_mainloop_new(NULL)), context(0), active(false),
    retryDelay(FirstRetry), channels(2), currentVolume(-1), currentMute(false)
{
    retryTimer.setSingleShot(true);
    retryTimer.setTimerType(Qt::VeryCoarseTimer);
    connect(&retryTimer, SIGNAL(timeout()), SLOT(connectPulse()));
    // (connected when the menu first shows the row)
}

PulseVolume::~PulseVolume()
{
    dropContext();
    pa_glib_mainloop_free(mainloop);
}

void PulseVolume::dropContext()
{
    if (context) {
        pa_context_set_state_callback(context, NULL, NULL);
        pa_context_set_subscribe_callback(context, NULL, NULL);
        pa_context_disconnect(context);
        pa_context_unref(context);
        context = 0;
    }
}

void PulseVolume::setActive(bool on)
{
    if (on == active)
        return;
    active = on;
    if (active) {
        retryDelay = FirstRetry;
        if (context && pa_context_get_state(context) == PA_CONTEXT_READY)
            subscribe();
        else
            connectPulse();
        return;
    }
    retryTimer.stop();
    if (context && pa_context_get_state(context) == PA_CONTEXT_READY) {
        // nothing to tell while the menu is closed
        pa_operation *op = pa_context_subscribe(context, PA_SUBSCRIPTION_MASK_NULL, NULL, NULL);
        if (op)
            pa_operation_unref(op);
    } else {
        // not there: no waiting for it either
        dropContext();
    }
}

void PulseVolume::subscribe()
{
    pa_operation *op = pa_context_subscribe(context,
        pa_subscription_mask_t(PA_SUBSCRIPTION_MASK_SINK | PA_SUBSCRIPTION_MASK_SERVER),
        NULL, NULL);
    if (op)
        pa_operation_unref(op);
    // what changed while the menu was closed
    sinkChanged();
}

void PulseVolume::connectPulse()
{
    dropContext();
    if (!active)
        return;
    pa_proplist *props = pa_proplist_new();
    pa_proplist_sets(props, PA_PROP_APPLICATION_NAME, "status indicator menu");
    context = pa_context_new_with_proplist(pa_glib_mainloop_get_api(mainloop),
                                           "sysuid volume", props);
    pa_proplist_free(props);
    pa_context_set_state_callback(context, contextStateCallback, this);
    pa_context_connect(context, NULL, PA_CONTEXT_NOFAIL, NULL);
}

void PulseVolume::contextChanged()
{
    switch (pa_context_get_state(context)) {
    case PA_CONTEXT_READY:
        retryDelay = FirstRetry;
        pa_context_set_subscribe_callback(context, subscribeCallback, this);
        if (active)
            subscribe();
        break;
    case PA_CONTEXT_FAILED:
    case PA_CONTEXT_TERMINATED:
        // PulseAudio went away (or is not there yet): try again while the
        // menu is open, less often each time; else when it opens again
        if (active) {
            retryTimer.start(retryDelay);
            retryDelay = qMin(retryDelay * 2, LongestRetry);
        } else {
            // (dropped outside of its own callback)
            QTimer::singleShot(0, this, SLOT(connectPulse()));
        }
        break;
    default:
        break;
    }
}

void PulseVolume::sinkChanged()
{
    // the default output may have changed: ask which one it is first
    pa_operation *op = pa_context_get_server_info(context, serverInfoCallback, this);
    if (op)
        pa_operation_unref(op);
}

void PulseVolume::serverInfo(const pa_server_info *info)
{
    if (!info || !info->default_sink_name)
        return;
    sinkName = info->default_sink_name;
    pa_operation *op = pa_context_get_sink_info_by_name(context, sinkName.constData(),
                                                        sinkInfoCallback, this);
    if (op)
        pa_operation_unref(op);
}

void PulseVolume::sinkInfo(const pa_sink_info *info)
{
    channels = info->volume.channels;
    int percent = qRound(100.0 * pa_cvolume_avg(&info->volume) / PA_VOLUME_NORM);
    bool mute = info->mute;
    if (percent != currentVolume || mute != currentMute) {
        currentVolume = qBound(0, percent, 100);
        currentMute = mute;
        emit volumeChanged(currentVolume, currentMute);
    }
}

void PulseVolume::setVolume(int percent)
{
    if (sinkName.isEmpty() || pa_context_get_state(context) != PA_CONTEXT_READY)
        return;
    pa_cvolume volume;
    pa_cvolume_set(&volume, channels, pa_volume_t(qBound(0, percent, 100) * PA_VOLUME_NORM / 100));
    pa_operation *op = pa_context_set_sink_volume_by_name(context, sinkName.constData(),
                                                          &volume, NULL, NULL);
    if (op)
        pa_operation_unref(op);
    // moving the slider unmutes, as the N9's did
    if (currentMute && percent > 0) {
        op = pa_context_set_sink_mute_by_name(context, sinkName.constData(), 0, NULL, NULL);
        if (op)
            pa_operation_unref(op);
    }
    currentVolume = percent;
}
