/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of mhome.
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
#ifndef EXTRADIRWATCHER_STUB
#define EXTRADIRWATCHER_STUB

#include "extradirwatcher.h"
#include <stubbase.h>


// 1. DECLARE STUB
// FIXME - stubgen is not yet finished
class ExtraDirWatcherStub : public StubBase {
  public:
  virtual void ExtraDirWatcherConstructor(MDataStore *dataStore, const QStringList &directories);
  virtual void ExtraDirWatcherDestructor();
  virtual bool isDesktopEntryValid(const MDesktopEntry &entry, const QStringList &acceptedTypes);
}; 

// 2. IMPLEMENT STUB
void ExtraDirWatcherStub::ExtraDirWatcherConstructor(MDataStore *dataStore, const QStringList &directories) {
  Q_UNUSED(dataStore);
  Q_UNUSED(directories);

}
void ExtraDirWatcherStub::ExtraDirWatcherDestructor() {

}
bool ExtraDirWatcherStub::isDesktopEntryValid(const MDesktopEntry &entry, const QStringList &acceptedTypes) {
  QList<ParameterBase*> params;
  params.append( new Parameter<const MDesktopEntry & >(entry));
  params.append( new Parameter<const QStringList & >(acceptedTypes));
  stubMethodEntered("isDesktopEntryValid",params);
  return stubReturnValue<bool>("isDesktopEntryValid");
}



// 3. CREATE A STUB INSTANCE
ExtraDirWatcherStub gDefaultExtraDirWatcherStub;
ExtraDirWatcherStub* gExtraDirWatcherStub = &gDefaultExtraDirWatcherStub;


// 4. CREATE A PROXY WHICH CALLS THE STUB
const QString ExtraDirWatcher::DESKTOP_ENTRY_KEY_PACKAGE_STATE = "PackageState";
const QString ExtraDirWatcher::DESKTOP_ENTRY_KEY_PACKAGE_HAD_ERROR = "PackageHadError";
const QString ExtraDirWatcher::DESKTOP_ENTRY_KEY_PACKAGE_NAME = "Package";
const QString ExtraDirWatcher::DESKTOP_ENTRY_KEY_PACKAGE_REMOVABLE = "PackageRemovable";
const QString ExtraDirWatcher::DESKTOP_ENTRY_GROUP_MEEGO = "X-MeeGo";

ExtraDirWatcher::ExtraDirWatcher(MDataStore *dataStore, const QStringList &directories) : LauncherDataStore(dataStore, directories) {
  gExtraDirWatcherStub->ExtraDirWatcherConstructor(dataStore, directories);
}

ExtraDirWatcher::~ExtraDirWatcher() {
  gExtraDirWatcherStub->ExtraDirWatcherDestructor();
}

bool ExtraDirWatcher::isDesktopEntryValid(const MDesktopEntry &entry, const QStringList &acceptedTypes) {
  return gExtraDirWatcherStub->isDesktopEntryValid(entry, acceptedTypes);
}


#endif
