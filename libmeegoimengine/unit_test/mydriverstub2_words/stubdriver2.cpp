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

#include "stubdriver2.h"
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


StubDriver2::StubDriver2()
{
    m_noCandidates = 0;
}

StubDriver2::~StubDriver2()
{
}

bool StubDriver2::init()
{
    return true;
}

bool StubDriver2::addDictionaryWord(const QString &w, MImEngine::DictionaryType type)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(w));
    params.append(new Parameter<MImEngine::DictionaryType>(type));
    stubMethodEntered("addDictionaryWord", params);
    Q_UNUSED(w);
    return stubReturnValue<bool>("addDictionaryWord");
}

bool StubDriver2::removeDictionaryWord(const QString &w, MImEngine::DictionaryType type)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(w));
    params.append(new Parameter<MImEngine::DictionaryType>(type));
    stubMethodEntered("removeDictionaryWord", params);
    Q_UNUSED(w);
    return stubReturnValue<bool>("removeDictionaryWord");
}

bool StubDriver2::disableDictionary(MImEngine::DictionaryType type)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<MImEngine::DictionaryType>(type));
    stubMethodEntered("disableDictionary", params);
    return stubReturnValue<bool>("disableDictionary");
}
bool StubDriver2::enableDictionary(MImEngine::DictionaryType type)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<MImEngine::DictionaryType>(type));
    stubMethodEntered("enableDictionary", params);
    return stubReturnValue<bool>("enableDictionary");
}
bool StubDriver2::removeDictionary(MImEngine::DictionaryType type)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<MImEngine::DictionaryType>(type));
    stubMethodEntered("removeDictionary", params);
    return stubReturnValue<bool>("removeDictionary");
}

void StubDriver2::appendString(const QString &s)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(s));
    stubMethodEntered("appendString", params);
    Q_UNUSED(s);
}
void StubDriver2::appendCharacter(const QChar &c)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QChar &>(c));
    stubMethodEntered("appendCharacter", params);
    Q_UNUSED(c);
}

void StubDriver2::tapKeyboard(const QPoint &position, bool shift, QChar symbol)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QPoint &>(position));
    params.append(new Parameter<bool>(shift));
    params.append(new Parameter<QChar>(symbol));
    stubMethodEntered("tapKeyboard", params);
    Q_UNUSED(position);
    Q_UNUSED(shift);
}

void StubDriver2::reselectString(const QString &s)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(s));
    stubMethodEntered("reselectString", params);
    Q_UNUSED(s);
}

void StubDriver2::setContext(const QString &s, int cursor)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(s));
    params.append(new Parameter<int>(cursor));
    stubMethodEntered("setContext", params);
    Q_UNUSED(s);
}

void StubDriver2::disablePrediction()
{
    stubMethodEntered("disablePrediction");
}
void StubDriver2::enablePrediction()
{
    stubMethodEntered("enablePrediction");
}
bool StubDriver2::predictionEnabled()
{
    stubMethodEntered("predictionEnabled");
    return stubReturnValue<bool>("predictionEnabled");
}

void StubDriver2::disableCorrection()
{
    stubMethodEntered("disableCorrection");
}
void StubDriver2::enableCorrection()
{
    stubMethodEntered("enableCorrection");
}
bool StubDriver2::correctionEnabled()
{
    stubMethodEntered("correctionEnabled");
    return stubReturnValue<bool>("correctionEnabled");
}


void StubDriver2::disableCompletion()
{
    stubMethodEntered("disableCompletion");
}
void StubDriver2::enableCompletion()
{
    stubMethodEntered("enableCompletion");
}
bool StubDriver2::completionEnabled()
{
    stubMethodEntered("completionEnabled");
    return stubReturnValue<bool>("completionEnabled");
}

QStringList StubDriver2::candidates(unsigned int uStartIndex, unsigned int uNum)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<unsigned int &>(uStartIndex));
    params.append(new Parameter<unsigned int &>(uNum));
    stubMethodEntered("candidates", params);
    return stubReturnValue<QStringList>("candidates");
}

int StubDriver2::totalCandidates()
{
    stubMethodEntered("totalCandidates");
    return stubReturnValue<int>("totalCandidates");
}

int StubDriver2::matchedLength()
{
    stubMethodEntered("matchedLength");
    return stubReturnValue<int>("matchedLength");
}

QStringList StubDriver2::matchedSyllables()
{
    stubMethodEntered("matchedSyllables");
    return stubReturnValue<QStringList>("matchedSyllables");
}

int StubDriver2::suggestedCandidateIndex()
{
    stubMethodEntered("suggestedCandidateIndex");
    return stubReturnValue<int>("suggestedCandidateIndex");
}

void StubDriver2::setExactWordPositionInList(MImEngine::ExactInListType setting)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<MImEngine::ExactInListType &>(setting));
    stubMethodEntered("setExactWordPositionInList", params);

}

MImEngine::DictionaryType StubDriver2::candidateSource(int index)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int &>(index));
    stubMethodEntered("candidateSource", params);
    return stubReturnValue<MImEngine::DictionaryType>("candidateSource");
}

void StubDriver2::clearEngineBuffer()
{
    stubMethodEntered("clearEngineBuffer");
}

QString  StubDriver2::language()
{
    stubMethodEntered("language");
    return stubReturnValue<QString>("language");
}
bool StubDriver2::setLanguage(const QString  &lan , MImEngine::LanguagePriority p)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(lan));
    params.append(new Parameter<MImEngine::LanguagePriority>(p));
    stubMethodEntered("setLanguage", params);
    return stubReturnValue<bool>("setLanguage");
}

bool StubDriver2::setKeyboardLayoutKeys(const QList<MImEngine::KeyboardLayoutKey> &keys)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QList<MImEngine::KeyboardLayoutKey> >(keys));
    stubMethodEntered("setKeyboardLayoutKeys", params);
    return stubReturnValue<bool>("setKeyboardLayoutKeys");
}

QList<MImEngine::KeyboardLayoutKey> StubDriver2::keyboardLayoutKeys()
{
    stubMethodEntered("keyboardLayoutKeys");
    return stubReturnValue<QList<MImEngine::KeyboardLayoutKey> >("keyboardLayoutKeys");
}

bool StubDriver2::exportAsNokiaDictionary(const QString &fileName)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(fileName));
    stubMethodEntered("exportAsNokiaDictionary", params);
    return stubReturnValue<bool>("exportAsNokiaDictionary");
}

bool StubDriver2::importNokiaUserDictionary(const QString &fileName)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(fileName));
    stubMethodEntered("importNokiaUserDictionary", params);
    return stubReturnValue<bool>("importNokiaUserDictionary");
}

void StubDriver2::setMaximumErrors(int maxErrors)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int>(maxErrors));
    stubMethodEntered("setMaximumErrors", params);
}

int StubDriver2::maximumErrors()
{
    stubMethodEntered("maximumErrors");
    return stubReturnValue<int>("maximumErrors");
}

QString StubDriver2::driver()
{
    stubMethodEntered("driver");
    qDebug() << "stub driver 2!";
    return "stubdriver2_words";
}
void StubDriver2::setMaximumCandidates(int maxCandidates)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int>(maxCandidates));
    stubMethodEntered("setMaximumCandidates", params);
}

int StubDriver2::maximumCandidates()
{
    stubMethodEntered("maximumCandidates");
    return stubReturnValue<int>("maximumCandidates");
}

QString StubDriver2::error()
{
    stubMethodEntered("error");
    return QString();
}

bool StubDriver2::setScript(const QString &s)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(s));
    stubMethodEntered("setScript", params);
    return stubReturnValue<bool>("setScript");
}

QString StubDriver2::script()
{
    stubMethodEntered("script");
    return stubReturnValue<QString>("script");
}

QString StubDriver2::transliterate(const QString &targetScript, unsigned int index)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(targetScript));
    params.append(new Parameter<unsigned int>(index));
    stubMethodEntered("transliterate", params);
    return stubReturnValue<QString>("transliterate");
}

void StubDriver2::insertCharacters(const QString &text, int index)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QString &>(text));
    params.append(new Parameter<int>(index));
    stubMethodEntered("insertCharacters", params);
}

void StubDriver2::removeCharacters(int count, int index)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int>(count));
    params.append(new Parameter<int>(index));
    stubMethodEntered("removeCharacters", params);
}

void StubDriver2::commitWord(int index, bool ignoreNewWord)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int>(index));
    params.append(new Parameter<bool>(ignoreNewWord));
    stubMethodEntered("commitWord", params);
}

Q_EXPORT_PLUGIN2(stubdriver2_words, StubDriver2);
