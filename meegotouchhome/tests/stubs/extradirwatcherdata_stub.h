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
#ifndef EXTRADIRWATCHERDATA_STUB
#define EXTRADIRWATCHERDATA_STUB

#include "extradirwatcher.h"
#include <stubbase.h>


// 1. DECLARE STUB
// FIXME - stubgen is not yet finished
class ExtraDirWatcherDataStub : public StubBase {
  public:
  virtual QVariant value(const QString &key) const;
  virtual bool setValue(const QString &key, const QVariant &value);
  virtual QStringList allKeys() const;
  virtual bool contains(const QString &) const;
  virtual bool createValue(const QString &, const QVariant &);
  virtual void remove(const QString &);
  virtual void clear();
  virtual QString key(const QString &value);
}; 

// 2. IMPLEMENT STUB
QVariant ExtraDirWatcherDataStub::value(const QString &key) const {
  QList<ParameterBase*> params;
  params.append( new Parameter<const QString & >(key));
  stubMethodEntered("value",params);
  return stubReturnValue<QVariant>("value");
}

bool ExtraDirWatcherDataStub::setValue(const QString &key, const QVariant &value) {
  QList<ParameterBase*> params;
  params.append( new Parameter<const QString & >(key));
  params.append( new Parameter<const QVariant & >(value));
  stubMethodEntered("setValue",params);
  return stubReturnValue<bool>("setValue");
}

QStringList ExtraDirWatcherDataStub::allKeys() const {
  stubMethodEntered("allKeys");
  return stubReturnValue<QStringList>("allKeys");
}

bool ExtraDirWatcherDataStub::contains(const QString &) const {
  QList<ParameterBase*> params;
  params.append( new Parameter<const QString & >((null)));
  stubMethodEntered("contains",params);
  return stubReturnValue<bool>("contains");
}

bool ExtraDirWatcherDataStub::createValue(const QString &, const QVariant &) {
  QList<ParameterBase*> params;
  params.append( new Parameter<const QString & >((null)));
  params.append( new Parameter<const QVariant & >((null)));
  stubMethodEntered("createValue",params);
  return stubReturnValue<bool>("createValue");
}

void ExtraDirWatcherDataStub::remove(const QString &) {
  QList<ParameterBase*> params;
  params.append( new Parameter<const QString & >((null)));
  stubMethodEntered("remove",params);
}

void ExtraDirWatcherDataStub::clear() {
  stubMethodEntered("clear");
}

QString ExtraDirWatcherDataStub::key(const QString &value) {
  QList<ParameterBase*> params;
  params.append( new Parameter<const QString & >(value));
  stubMethodEntered("key",params);
  return stubReturnValue<QString>("key");
}



// 3. CREATE A STUB INSTANCE
ExtraDirWatcherDataStub gDefaultExtraDirWatcherDataStub;
ExtraDirWatcherDataStub* gExtraDirWatcherDataStub = &gDefaultExtraDirWatcherDataStub;


// 4. CREATE A PROXY WHICH CALLS THE STUB
QVariant ExtraDirWatcherData::value(const QString &key) const {
  return gExtraDirWatcherDataStub->value(key);
}

bool ExtraDirWatcherData::setValue(const QString &key, const QVariant &value) {
  return gExtraDirWatcherDataStub->setValue(key, value);
}

QStringList ExtraDirWatcherData::allKeys() const {
  return gExtraDirWatcherDataStub->allKeys();
}

bool ExtraDirWatcherData::contains(const QString &) const {
  return gExtraDirWatcherDataStub->contains();
}

bool ExtraDirWatcherData::createValue(const QString &, const QVariant &) {
  return gExtraDirWatcherDataStub->createValue();
}

void ExtraDirWatcherData::remove(const QString &) {
  gExtraDirWatcherDataStub->remove();
}

void ExtraDirWatcherData::clear() {
  gExtraDirWatcherDataStub->clear();
}

QString ExtraDirWatcherData::key(const QString &value) {
  return gExtraDirWatcherDataStub->key(value);
}


#endif
