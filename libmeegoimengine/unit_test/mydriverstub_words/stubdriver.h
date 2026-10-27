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

#ifndef STUBDRIVER_H
#define STUBDRIVER_H

#include <stubbase.h>
#include <mimenginetypes.h>
#include <QVector>
#include <QObject>
#include <mimenginewordsinterface.h>

class MImEngineWords;
class QStringList;
class QtPlugin;
class QtDebug;
class QLibrary;
class QSettings;
class QFile;

/*!
  \class StubDriver

  \brief StubDriver inherit from MImEngineWordsInterface(Abstract driver class) and QObject,
  it implement the input method engine, provide the functions: e.g. new error correction/prediction.

 */
class StubDriver: public QObject, public MImEngineWordsInterface, public StubBase
{
    Q_OBJECT
    Q_DISABLE_COPY(StubDriver)
    Q_INTERFACES(MImEngineWordsInterface)


public:
    /*!
     * StubDriver construtor
     *\param N/A
     *\return StubDriver
     */
    StubDriver();

    /*!
     * StubDriver destructor
     *\param  N/A
     *\return N/A
     */
    ~StubDriver();

    /*!
     *\reimp
    */

    virtual bool init();

    virtual bool addDictionaryWord(const QString &word, MImEngine::DictionaryType);

    virtual bool removeDictionaryWord(const QString &word, MImEngine::DictionaryType);

    virtual bool disableDictionary(MImEngine::DictionaryType);

    virtual bool enableDictionary(MImEngine::DictionaryType);

    virtual bool removeDictionary(MImEngine::DictionaryType);

    virtual void appendString(const QString &candidate);

    virtual void appendCharacter(const QChar &candidate);

    virtual void tapKeyboard(const QPoint &position, bool shift, QChar symbol);

    virtual void reselectString(const QString &s);

    virtual void setContext(const QString &s, int cursor);

    virtual void disablePrediction();

    virtual void enablePrediction();

    virtual bool predictionEnabled();

    virtual void disableCorrection();

    virtual void enableCorrection();

    virtual bool correctionEnabled();

    virtual void disableCompletion();

    virtual void enableCompletion();

    virtual bool completionEnabled();

    virtual QStringList candidates(unsigned int uStartIndex = 0, unsigned int uNum = 0);

    virtual int totalCandidates();

    virtual int matchedLength();

    virtual QStringList matchedSyllables();

    virtual int suggestedCandidateIndex();

    virtual void setExactWordPositionInList(MImEngine::ExactInListType setting);

    virtual MImEngine::DictionaryType candidateSource(int index);

    virtual void clearEngineBuffer();

    virtual QString  language();

    virtual bool setLanguage(const QString &language, MImEngine::LanguagePriority);

    virtual bool setKeyboardLayoutKeys(const QList<MImEngine::KeyboardLayoutKey> &keys);

    virtual QList<MImEngine::KeyboardLayoutKey> keyboardLayoutKeys();

    virtual bool exportAsNokiaDictionary(const QString &);

    virtual bool importNokiaUserDictionary(const QString &);

    virtual void setMaximumErrors(int);

    virtual int maximumErrors();

    virtual void setMaximumCandidates(int maxCandidates);

    virtual int maximumCandidates();

    virtual QString driver();

    virtual QString error();

    virtual bool setScript(const QString &s);

    virtual QString script();

    virtual QString transliterate(const QString &targetScript, unsigned int index);

    virtual void insertCharacters(const QString &text, int index = -1);

    virtual void removeCharacters(int count = 1, int index = -1);

    virtual void commitWord(int index = -1, bool ignoreNewWord = true);

    /*!
     *\reimp_end
    */

private:
    int m_noCandidates;
};
#endif //STUBDRIVER_H
