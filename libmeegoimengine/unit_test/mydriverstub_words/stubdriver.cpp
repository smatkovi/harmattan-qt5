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

#include "stubdriver.h"
#include <iostream>
#include <string>
#include <QObject>
#include <mimenginefactory.h>
#include <QStringList>
#include <QtPlugin>
#include <QtDebug>
#include <QLibrary>
#include <QSettings>
#include <QFile>
#include <QDataStream>


StubDriver::StubDriver()
{
    m_noCandidates = 0;
}

StubDriver::~StubDriver()
{
}

bool StubDriver::init()
{
    return true;
}

bool StubDriver::addDictionaryWord(const QString &w, MImEngine::DictionaryType type)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(w));
    params.append(new Parameter<MImEngine::DictionaryType>(type));
    stubMethodEntered("addDictionaryWord", params);
    Q_UNUSED(w);
    return stubReturnValue<bool>("addDictionaryWord");
}

bool StubDriver::removeDictionaryWord(const QString &w, MImEngine::DictionaryType type)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(w));
    params.append(new Parameter<MImEngine::DictionaryType>(type));
    stubMethodEntered("removeDictionaryWord", params);
    Q_UNUSED(w);
    return stubReturnValue<bool>("removeDictionaryWord");
}

bool StubDriver::disableDictionary(MImEngine::DictionaryType type)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<MImEngine::DictionaryType>(type));
    stubMethodEntered("disableDictionary", params);
    return stubReturnValue<bool>("disableDictionary");
}
bool StubDriver::enableDictionary(MImEngine::DictionaryType type)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<MImEngine::DictionaryType>(type));
    stubMethodEntered("enableDictionary", params);
    return stubReturnValue<bool>("enableDictionary");
}
bool StubDriver::removeDictionary(MImEngine::DictionaryType type)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<MImEngine::DictionaryType>(type));
    stubMethodEntered("removeDictionary", params);
    return stubReturnValue<bool>("removeDictionary");
}

void StubDriver::appendString(const QString &s)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(s));
    stubMethodEntered("appendString", params);
    Q_UNUSED(s);
}
void StubDriver::appendCharacter(const QChar &c)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QChar &>(c));
    stubMethodEntered("appendCharacter", params);
    Q_UNUSED(c);
}

void StubDriver::tapKeyboard(const QPoint &position, bool shift, QChar symbol)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QPoint &>(position));
    params.append(new Parameter<bool>(shift));
    params.append(new Parameter<QChar>(symbol));
    stubMethodEntered("tapKeyboard", params);
    Q_UNUSED(position);
    Q_UNUSED(shift);
}

void StubDriver::reselectString(const QString &s)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(s));
    stubMethodEntered("reselectString", params);
    Q_UNUSED(s);
}

void StubDriver::setContext(const QString &s, int cursor)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(s));
    params.append(new Parameter<int>(cursor));
    stubMethodEntered("setContext", params);
    Q_UNUSED(s);
}

void StubDriver::disablePrediction()
{
    stubMethodEntered("disablePrediction");
}
void StubDriver::enablePrediction()
{
    stubMethodEntered("enablePrediction");
}
bool StubDriver::predictionEnabled()
{
    stubMethodEntered("predictionEnabled");
    return stubReturnValue<bool>("predictionEnabled");
}

void StubDriver::disableCorrection()
{
    stubMethodEntered("disableCorrection");
}
void StubDriver::enableCorrection()
{
    stubMethodEntered("enableCorrection");
}
bool StubDriver::correctionEnabled()
{
    stubMethodEntered("correctionEnabled");
    return stubReturnValue<bool>("correctionEnabled");
}


void StubDriver::disableCompletion()
{
    stubMethodEntered("disableCompletion");
}
void StubDriver::enableCompletion()
{
    stubMethodEntered("enableCompletion");
}
bool StubDriver::completionEnabled()
{
    stubMethodEntered("completionEnabled");
    return stubReturnValue<bool>("completionEnabled");
}

QStringList StubDriver::candidates(unsigned int uStartIndex, unsigned int uNum)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<unsigned int &>(uStartIndex));
    params.append(new Parameter<unsigned int &>(uNum));
    stubMethodEntered("candidates", params);
    return stubReturnValue<QStringList>("candidates");
}

int StubDriver::totalCandidates()
{
    stubMethodEntered("totalCandidates");
    return stubReturnValue<int>("totalCandidates");
}

int StubDriver::matchedLength()
{
    stubMethodEntered("matchedLength");
    return stubReturnValue<int>("matchedLength");
}

QStringList StubDriver::matchedSyllables()
{
    stubMethodEntered("matchedSyllables");
    return stubReturnValue<QStringList>("matchedSyllables");
}

int StubDriver::suggestedCandidateIndex()
{
    stubMethodEntered("suggestedCandidateIndex");
    return stubReturnValue<int>("suggestedCandidateIndex");
}

void StubDriver::setExactWordPositionInList(MImEngine::ExactInListType setting)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<MImEngine::ExactInListType &>(setting));
    stubMethodEntered("setExactWordPositionInList", params);

}

MImEngine::DictionaryType StubDriver::candidateSource(int index)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int &>(index));
    stubMethodEntered("candidateSource", params);
    return stubReturnValue<MImEngine::DictionaryType>("candidateSource");
}

void StubDriver::clearEngineBuffer()
{
    stubMethodEntered("clearEngineBuffer");
}

QString StubDriver::language()
{
    stubMethodEntered("language");
    return stubReturnValue<QString>("language");
}

bool StubDriver::setLanguage(const QString  &lan , MImEngine::LanguagePriority p)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(lan));
    params.append(new Parameter<MImEngine::LanguagePriority>(p));
    stubMethodEntered("setLanguage", params);
    return stubReturnValue<bool>("setLanguage");
}

bool StubDriver::setKeyboardLayoutKeys(const QList<MImEngine::KeyboardLayoutKey> &keys)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QList<MImEngine::KeyboardLayoutKey> >(keys));
    stubMethodEntered("setKeyboardLayoutKeys", params);
    return stubReturnValue<bool>("setKeyboardLayoutKeys");
}

QList<MImEngine::KeyboardLayoutKey> StubDriver::keyboardLayoutKeys()
{
    stubMethodEntered("keyboardLayoutKeys");
    return stubReturnValue<QList<MImEngine::KeyboardLayoutKey> >("keyboardLayoutKeys");
}

bool StubDriver::exportAsNokiaDictionary(const QString &fileName)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(fileName));
    stubMethodEntered("exportAsNokiaDictionary", params);
    return stubReturnValue<bool>("exportAsNokiaDictionary");
}

bool StubDriver::importNokiaUserDictionary(const QString &fileName)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(fileName));
    stubMethodEntered("importNokiaUserDictionary", params);
    return stubReturnValue<bool>("importNokiaUserDictionary");
}

void StubDriver::setMaximumErrors(int maxErrors)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int>(maxErrors));
    stubMethodEntered("setMaximumErrors", params);
}

int StubDriver::maximumErrors()
{
    stubMethodEntered("maximumErrors");
    return stubReturnValue<int>("maximumErrors");
}

QString StubDriver::driver()
{
    stubMethodEntered("driver");
    qDebug() << "stub driver 1!";
    return "stubdriver1_words";
}

void StubDriver::setMaximumCandidates(int maxCandidates)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int>(maxCandidates));
    stubMethodEntered("setMaximumCandidates", params);
}

int StubDriver::maximumCandidates()
{
    stubMethodEntered("maximumCandidates");
    return stubReturnValue<int>("maximumCandidates");
}

QString StubDriver::error()
{
    stubMethodEntered("error");
    return QString();
}

bool StubDriver::setScript(const QString &s)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(s));
    stubMethodEntered("setScript", params);
    return stubReturnValue<bool>("setScript");
}

QString StubDriver::script()
{
    stubMethodEntered("script");
    return stubReturnValue<QString>("script");
}

QString StubDriver::transliterate(const QString &targetScript, unsigned int index)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(targetScript));
    params.append((new Parameter<unsigned int>(index)));
    stubMethodEntered("transliterate", params);
    return stubReturnValue<QString>("transliterate");
}

void StubDriver::insertCharacters(const QString &text, int index)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(text));
    params.append(new Parameter<int>(index));
    stubMethodEntered("insertCharacters", params);
}

void StubDriver::removeCharacters(int count, int index)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int>(count));
    params.append(new Parameter<int>(index));
    stubMethodEntered("removeCharacters", params);
}

void StubDriver::commitWord(int index, bool ignoreNewWord)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int>(index));
    params.append(new Parameter<bool>(ignoreNewWord));
    stubMethodEntered("commitWord", params);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(stubdriver1_words, StubDriver);
#endif
