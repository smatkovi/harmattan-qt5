/* This file is part of meegoimengine *
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

#include "dummydriver.h"
#include "mimenginefactory.h"
#include <QtPlugin>
#include <QLibrary>

DummyDriver::DummyDriver()
    : predictionEnabledFlag(false),
      correctionEnabledFlag(false),
      completionEnabledFlag(false),
      maximumErrorsCount(0),
      maximumCandidatesCount(0)
{
}

DummyDriver::~DummyDriver()
{
}

bool DummyDriver::init()
{
    return true;
}

bool DummyDriver::addDictionaryWord(const QString &w, MImEngine::DictionaryType type)
{
    Q_UNUSED(w);
    Q_UNUSED(type);
    return false;
}

bool DummyDriver::removeDictionaryWord(const QString &w, MImEngine::DictionaryType type)
{
    Q_UNUSED(w);
    Q_UNUSED(type);
    return false;
}

bool DummyDriver::disableDictionary(MImEngine::DictionaryType type)
{
    Q_UNUSED(type);
    return false;
}
bool DummyDriver::enableDictionary(MImEngine::DictionaryType type)
{
    Q_UNUSED(type);
    return false;
}
bool DummyDriver::removeDictionary(MImEngine::DictionaryType type)
{
    Q_UNUSED(type);
    return false;
}

void DummyDriver::appendString(const QString &s)
{
    Q_UNUSED(s);
}
void DummyDriver::appendCharacter(const QChar &c)
{
    Q_UNUSED(c);
}

void DummyDriver::tapKeyboard(const QPoint &position, bool shift, QChar symbol)
{
    Q_UNUSED(position);
    Q_UNUSED(shift);
    Q_UNUSED(symbol);
}

void DummyDriver::reselectString(const QString &s)
{
    Q_UNUSED(s);
}

void DummyDriver::setContext(const QString &s, int cursor)
{
    Q_UNUSED(s);
    Q_UNUSED(cursor);
}

void DummyDriver::disablePrediction()
{
    predictionEnabledFlag = false;
}

void DummyDriver::enablePrediction()
{
    predictionEnabledFlag = true;
}

bool DummyDriver::predictionEnabled()
{
    return predictionEnabledFlag;
}

void DummyDriver::disableCorrection()
{
    correctionEnabledFlag = false;
}

void DummyDriver::enableCorrection()
{
    correctionEnabledFlag = true;
}

bool DummyDriver::correctionEnabled()
{
    return correctionEnabledFlag;
}

void DummyDriver::disableCompletion()
{
    completionEnabledFlag = false;
}

void DummyDriver::enableCompletion()
{
    completionEnabledFlag = true;
}

bool DummyDriver::completionEnabled()
{
    return completionEnabledFlag;
}

QStringList DummyDriver::candidates(unsigned int uStartIndex, unsigned int uNum)
{
    Q_UNUSED(uStartIndex);
    Q_UNUSED(uNum);
    return QStringList();
}

int DummyDriver::totalCandidates()
{
    return 0;
}

int DummyDriver::matchedLength()
{
    return 0;
}

QStringList DummyDriver::matchedSyllables()
{
    return QStringList();
}

int DummyDriver::suggestedCandidateIndex()
{
    return 0;
}

void DummyDriver::setExactWordPositionInList(MImEngine::ExactInListType setting)
{
    Q_UNUSED(setting);
}

MImEngine::DictionaryType DummyDriver::candidateSource(int index)
{
    Q_UNUSED(index);
    return  MImEngine::DictionaryTypeInvalid;
}

void DummyDriver::clearEngineBuffer()
{
}

QString  DummyDriver::language()
{
    return driverLanguage;
}

bool DummyDriver::setLanguage(const QString  &l, MImEngine::LanguagePriority p)
{
    Q_UNUSED(p);
    driverLanguage = l;
    return true;
}

bool DummyDriver::setKeyboardLayoutKeys(const QList<MImEngine::KeyboardLayoutKey> &keys)
{
    layoutKeys = keys;
    return true;
}

QList<MImEngine::KeyboardLayoutKey> DummyDriver::keyboardLayoutKeys()
{
    return layoutKeys;
}

bool DummyDriver::exportAsNokiaDictionary(const QString &fileName)
{
    Q_UNUSED(fileName);
    return false;
}

bool DummyDriver::importNokiaUserDictionary(const QString &fileName)
{
    Q_UNUSED(fileName);
    return false;
}

void DummyDriver::setMaximumErrors(int maxErrors)
{
    maximumErrorsCount = maxErrors;
}

int DummyDriver::maximumErrors()
{
    return maximumErrorsCount;
}

QString DummyDriver::driver()
{
    return "dummyimdriver";
}

void DummyDriver::setMaximumCandidates(int maxCandidates)
{
    maximumCandidatesCount = maxCandidates;
}

int DummyDriver::maximumCandidates()
{
    return maximumCandidatesCount;
}

QString DummyDriver::error()
{
    return QString();
}

bool DummyDriver::setScript(const QString &s)
{
    Q_UNUSED(s);
    return false;
}

QString DummyDriver::script()
{
    return QString();
}

QString DummyDriver::transliterate(const QString &targetScript, unsigned int index)
{
    Q_UNUSED(targetScript);
    Q_UNUSED(index);
    return QString();
}

void DummyDriver::insertCharacters(const QString &text, int index)
{
    Q_UNUSED(text);
    Q_UNUSED(index);
}

void DummyDriver::removeCharacters(int count, int index)
{
    Q_UNUSED(count);
    Q_UNUSED(index);
}

void DummyDriver::commitWord(int index, bool ignoreNewWord)
{
    Q_UNUSED(index);
    Q_UNUSED(ignoreNewWord);
}


Q_EXPORT_PLUGIN2(dummyimdriver, DummyDriver);
