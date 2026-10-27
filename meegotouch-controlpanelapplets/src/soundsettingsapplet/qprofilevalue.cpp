/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QDebug>
#include <QStringList>
#include <QFile>
#ifdef HAVE_LIBPROFILE
#include <profiled/libprofile.h>
#endif

#include "qprofilevalue.h"

//#define DEBUG
#define WARNING
#include "../debug.h"

#define TO_STRING(string) ((string).toUtf8().constData())
#define NEED_TO_WATCH(string) ((string).startsWith ("/home"))

int QProfileValue::nTrackedValues = 0;

/*!
 * \param key The key identifies the profile entry.
 * \param setAllProfiles If true the changing of the value will be reflected in
 *   every profile.
 *
 * The constructor the class sets the key identifies the profile entry. The key
 * will not be changed for the full lifetime of the object.
 */
QProfileValue::QProfileValue(
        const QString  &key,
        bool            setAllProfiles) :
    QTrackedVariant (key),
    m_setAllProfiles (setAllProfiles),
    m_MissingFile (false)
{
    SYS_DEBUG ("*** key = %s", SYS_STR (key));
    addNotify ();
}

/*!
 * QProfileValue destructor
 */
QProfileValue::~QProfileValue ()
{
    SYS_DEBUG ("%sdestruct%s", TERM_BLUE, TERM_NORMAL);

    delNotify ();
    stopWatchFiles ();
}

/*!
 * A method for following changes in the profiled
 */
void
QProfileValue::notifyValue (
        const char      *profile,
        const char      *key,
        const char      *val,
        const char      *type,
        QProfileValue   *self)
{
    QString compareString;
    Q_UNUSED(val)
    Q_UNUSED(type)

    if (self->key ().contains('@'))
        compareString = self->key ();
    else
    {
#ifdef HAVE_LIBPROFILE
        char *currentProfile = profile_get_profile ();
        compareString = self->key () + "@" + currentProfile;
        free (currentProfile);
#endif
    }

    if (compareString == (QString (key) + "@" + profile))
    {
        self->m_val.clear ();
        self->emit_changed ();
    }
}

/*!
 * A method for query notifications from profiled about profile changes
 */
void
QProfileValue::addNotify()
{
#ifdef HAVE_LIBPROFILE
    if (0 == nTrackedValues)
        profile_tracker_init ();
    nTrackedValues++;

    profile_track_add_active_cb (
        (profile_track_value_fn_data) notifyValue, this, NULL);
    profile_track_add_change_cb (
        (profile_track_value_fn_data) notifyValue, this, NULL);
#endif
}

/*!
 * A method for remove notifications from profiled
 */
void
QProfileValue::delNotify ()
{
#ifdef HAVE_LIBPROFILE
    profile_track_remove_active_cb (
        (profile_track_value_fn_data) notifyValue, this);
    profile_track_remove_change_cb (
        (profile_track_value_fn_data) notifyValue, this);

    nTrackedValues--;
    if (0 == nTrackedValues)
        profile_tracker_quit ();
#endif
}

/*!
 * This method sets the new value in the profiled
 *
 * \param newValue the new value
 */
void
QProfileValue::realSetValue (const QVariant &newValue)
{
    SYS_DEBUG ("****************************************");
    SYS_DEBUG ("*** m_setAllProfiles = %s", SYS_BOOL(m_setAllProfiles));
    SYS_DEBUG ("*** key()            = %s", SYS_STR(key()));
    /*
     * Not doing anything if the value is the same.
     */
    if (m_val == newValue)
        return;

    m_val.clear ();

    /*
     * The empty key shows that this value is handled manually by the caller.
     * This special case is used for custom settings.
     */
    SYS_DEBUG ("*** key() = %s", SYS_STR (key ()));
    if (key () == "custom.alert.tone")
    {
        SYS_DEBUG ("The key is 'custom.alert.tone', not in backend.");
        SYS_DEBUG ("*** Storing '%s'", SYS_STR (newValue.toString ()));
        m_val = newValue;
        emit_changed ();
        return;
    }

#ifdef HAVE_LIBPROFILE
    QVariant convertedValue = newValue;
    QVariant::Type neededType = QVariant::Invalid;
    QString theKey, theProfile;
    QStringList lsType = getType (theKey, theProfile);

    char *currentProfile = profile_get_profile ();
    if (theProfile.isNull ())
        theProfile = QString (currentProfile);
    free (currentProfile);

    /*
     *
     */
    stopWatchFiles ();

    SYS_DEBUG ("*** lsType[0] = %s", SYS_STR (lsType[0]));
    if ("SOUNDFILE" == lsType[0])
    {
        QString filename = newValue.toString ();
        SYS_DEBUG ("*** filename  = %s", SYS_STR (filename));

        if (!filename.isEmpty ())
        {
            bool missing;
            missing = startWatchFile (filename);
            SYS_DEBUG ("*** missing = %s", SYS_BOOL (missing));
        }
        neededType = QVariant::String;

    }
    else if ("STRING" == lsType[0])
    {
        neededType = QVariant::String;
    }
    else if ("INTEGER" == lsType[0])
    {
        neededType = QVariant::Int;
    }
    else if ("BOOLEAN" == lsType[0])
    {
        neededType = QVariant::Bool;
    }
    else if ("DOUBLE" == lsType[0])
    {
        neededType = QVariant::Double;
    }

    if (neededType != QVariant::Invalid &&
        convertedValue.convert (neededType))
    {
        switch (neededType)
        {
            case QVariant::Bool:
                profile_set_value_as_bool (
                    TO_STRING (theProfile), TO_STRING (theKey),
                    (int) convertedValue.toBool ());
                break;
            case QVariant::Int:
                profile_set_value_as_int (
                    TO_STRING (theProfile), TO_STRING (theKey),
                    (int) convertedValue.toInt ());
                break;
            case QVariant::Double:
                profile_set_value_as_double (
                    TO_STRING (theProfile), TO_STRING (theKey),
                    (double) convertedValue.toDouble ());
                break;
            case QVariant::String:
                profile_set_value (
                    TO_STRING (theProfile), TO_STRING (theKey),
                    (char *) TO_STRING (convertedValue.toString ()));
                break;
            default:
                break;
        }
    }

    if (m_setAllProfiles) {
        SYS_DEBUG ("SETTING ALL THE PROFILES");
        /*
         * Alert Tone should be the same for all profiles...
         * Fixes: NB#258344
         *
         * Will remove the shole exception stuff because of this bug:
         * NB#288423
         */
        //bool isAlertTone = theKey.contains ("alert.tone");

        char **profiles = profile_get_profiles ();
        if (profiles)
        {
            for (int i = 0 ; profiles[i] != NULL ; i++)
            {
                SYS_DEBUG ("------------------------------------------");
                SYS_DEBUG ("theProfile     = %s", SYS_STR(theProfile));
                SYS_DEBUG ("profiles[%03d] = %s", i, profiles[i]);
                SYS_DEBUG ("theKey         = %s", SYS_STR(theKey));
                //SYS_DEBUG ("isAlertTone    = %s", SYS_BOOL(isAlertTone));
                if (theProfile == QString (profiles[i]))
                    continue; // the current one already set...

                /*
                 * Do not set meeting and silent values if it is
                 * not about the alert tone...
                 */
                #if 0
                if ((!isAlertTone) &&
                    (profiles[i] == QString ("meeting") ||
                     profiles[i] == QString ("silent"))) {
                    SYS_WARNING ("Except this.");
                    continue;
                }
                #endif

                SYS_DEBUG ("Setting '%s'",
                        SYS_STR((theKey + "@" + QString (profiles[i]))));
                QProfileValue other (
                        theKey + "@" + QString (profiles[i]), false);
                other.set (newValue);
            }
            profile_free_profiles (profiles);
        }
    }
#endif
}

/*!
 * Method for fetching the backend (profiled) current values
 */
void
QProfileValue::fetchFromBackend ()
{
#ifdef HAVE_LIBPROFILE
    QString      theKey, theProfile;
    QStringList  lsType = getType(theKey, theProfile);
    QVariant     var;

    SYS_DEBUG ("*** lsType[0] = %s", SYS_STR(lsType[0]));
    SYS_DEBUG ("*** theKey = '%s'", SYS_STR(theKey));
    SYS_DEBUG ("*** key()  = '%s'", SYS_STR(key()));
    if (theKey == "custom.alert.tone")
        return;

    if ("SOUNDFILE" == lsType[0])
    {
        char *filename;
        bool  needReread;

        filename = profile_get_value (
                theProfile.isNull() ? NULL : TO_STRING(theProfile),
                TO_STRING(theKey));
        needReread = startWatchFile (QString::fromUtf8(filename));
        SYS_DEBUG ("*** needReread = %s", SYS_BOOL(needReread));

        if (needReread && !m_MissingFile) {
            SYS_DEBUG ("Need re-read, was not missing before: %s",
                    filename);
            m_MissingFile = true;
            realSetValue (QVariant (""));
        } else if (needReread && m_MissingFile) {
            SYS_WARNING ("Need re-read but already missing: giving up: %s",
                    filename);
        } else if (!needReread && m_MissingFile) {
            SYS_DEBUG ("Was missing before, but now found: %s",
                    filename);
            m_MissingFile = false;
        }

        var = QVariant(QString::fromUtf8(filename));
        free(filename);
    }
    else if ("STRING" == lsType[0])
    {
        char *the_value = profile_get_value (
                theProfile.isNull() ? NULL : TO_STRING(theProfile),
                TO_STRING(theKey));

        var = QVariant(QString::fromUtf8(the_value));
        free(the_value);
    }
    else if ("BOOLEAN" == lsType[0])
    {
        var = QVariant((bool)profile_get_value_as_bool(
                theProfile.isNull() ? NULL : TO_STRING(theProfile),
                TO_STRING(theKey)));
    }
    else if ("INTEGER" == lsType[0])
    {
        var = QVariant((int)profile_get_value_as_int(
                theProfile.isNull() ? NULL : TO_STRING(theProfile),
                TO_STRING(theKey)));
    }

    if (!var.isNull())
        m_val = var;
#endif
}

/*!
 * An internal method for get the type of the key
 */
QStringList
QProfileValue::getType (
        QString &theKey,
        QString &theProfile)
{
    QStringList    lsType;
    QStringList    lsKey;
    char          *the_type;

    /*
     * The key looks like this: custom.alert.tone@general
     *   where @general is optional and
     *     o custom.alert.tone = theKey
     *     o general = theprofile
     */
    lsKey = key().split('@');
    theKey = lsKey[0];

    if (lsKey.size() > 1)
        theProfile = lsKey[1];

    SYS_DEBUG ("*** theKey = '%s'", SYS_STR(theKey));
    SYS_DEBUG ("*** key()  = '%s'", SYS_STR(key()));
    if (theKey == "custom.alert.tone") {
        lsType << "SOUNDFILE";
        goto finalize;
    }

#ifdef HAVE_LIBPROFILE
    the_type = profile_get_type(lsKey[0].toUtf8().constData());
    SYS_DEBUG ("*** %s = %s", SYS_STR(theKey), the_type);

    lsType = QString(the_type).split(' ');
    free(the_type);
    the_type = NULL;
#endif

finalize:
    SYS_DEBUG ("Returning %s",
            lsType.size() == 0 ? "<nothing>" : SYS_STR(lsType[0]));
    return lsType;
}

/*!
 * \param p_rangeType Textual representation of the range or NULL to retrieve
 *   the range from the profile database.
 * \returns The list of the acceptable values of the profile key.
 */
QList<QVariant>
QProfileValue::possibleValues(RangeType *p_rangeType)
{
    QList<QVariant> ret;
    QString theKey, theProfile;
    QStringList lsType = getType(theKey, theProfile);

    SYS_DEBUG ("*** theKey = '%s'", SYS_STR(theKey));
    SYS_DEBUG ("*** key()  = '%s'", SYS_STR(key()));
    SYS_DEBUG ("*** p_rangeType = %p");
    SYS_DEBUG ("*** lsType[0]   = %s", SYS_STR(lsType[0]));

    if (p_rangeType)
        *p_rangeType = Invalid;

    if ("SOUNDFILE" == lsType[0] ||
        "STRING"    == lsType[0]) {

    if (p_rangeType)
        *p_rangeType = List;

        for (int i = 1; i < lsType.size () ; i++)
        {
            ret.append (QVariant (lsType[i].remove ('"')));
        }
    }
    else
    if ("BOOLEAN" == lsType[0]) {
        if (p_rangeType) (*p_rangeType) = List;
            ret.append(QVariant(false));
            ret.append(QVariant(true));
    }
    else
    if ("INTEGER" == lsType[0]) {
        QVariant lower = QVariant(INT_MIN);
        QVariant upper = QVariant(INT_MAX);

        if (p_rangeType) (*p_rangeType) = Interval;

        if (lsType.size() > 1) {
            QStringList lsInterval = lsType[1].split('-');
            if (lsInterval.size() > 0)
                if (QVariant(lsInterval[0]).canConvert(QVariant::Int))
                    lower.setValue(lsInterval[0]);
            if (lsInterval.size() > 1)
                if (QVariant(lsInterval[1]).canConvert(QVariant::Int))
                    upper.setValue(lsInterval[1]);
        }

        ret.append(lower);
        ret.append(upper);
    }

    return ret;
}

/*!
 * This slot will be activated when the file system watcher reports a change in
 * the file state. If the file is removed we are setting the value to the empty
 * string here and then the profile backend will report back the default value
 * for th egiven key.
 */
void
QProfileValue::fileChanged (
        const QString &filename)
{
    QFile thisFile (filename);
    bool  exists = thisFile.exists (filename);

    SYS_DEBUG ("*** key()  = '%s'", SYS_STR(key()));
    SYS_DEBUG ("*** path   = %s", SYS_STR(filename));
    SYS_DEBUG ("*** exists = %s", SYS_BOOL(exists));
    if (!exists) {
        realSetValue (QVariant (""));
    }
}

/*!
 * \returns true if a file was watched before the method is called.
 *
 * Stops the file watching.
 */
bool
QProfileValue::stopWatchFiles ()
{
    SYS_DEBUG ("");

    if (m_FileWatcher)
    {
        delete m_FileWatcher;
        return true;
    }

    return false;
}

/*!
 * \param filename The full path of the file to watch.
 * \returns true if the file exists, false otherwise.
 *
 * The QProfileValue is able to watch the file system to sense when the sound file
 * is removed/deleted. This virtual method will start watching the file if the
 * file exists. If the file is watched and removed the class will set the value
 * to "" in order to get the file name of the default sound file from the
 * profile daemon and emit the changed() signal when the default file is
 * returned. Setting the file name to the empty string is necessary because
 * every event has its own unique default file name that can be retrieved only
 * when the value set to "".
 */
bool
QProfileValue::startWatchFile (
        const QString &filename)
{
    QFile thisFile (filename);
    bool  exists = thisFile.exists (filename);

    SYS_DEBUG ("filename = %s", SYS_STR(filename));
    SYS_DEBUG ("exists   = %s", SYS_BOOL(exists));

    /*
     * We stop watching if we did before.
     */
    stopWatchFiles ();

    /*
     * If the file does not exists to begin with we don't need to watch. We will
     * return true, that means the file name must be fixed.
     */
    if (!exists) {
        goto finalize;
    }

    /*
     * There is no sense to watch the files in the rootfs, as those are
     * only modified by SSU, and after that reboot is initiated.
     */
    if (NEED_TO_WATCH(filename))
    {
        m_FileWatcher = new QFileSystemWatcher (this);
        m_FileWatcher->addPath (filename);
        connect (m_FileWatcher, SIGNAL (fileChanged (const QString &)),
                 SLOT (fileChanged (const QString &)));
    }

finalize:
    SYS_DEBUG ("returning %s", SYS_BOOL(!exists));
    return !exists;
}

