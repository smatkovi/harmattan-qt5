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

#ifndef UT_MIMENGINE
#define UT_MIMENGINE

#include <QtTest/QtTest>
#include <QObject>

#include "mimenginefactory.h"
Q_DECLARE_METATYPE(MImEngineFactory *);
class Ut_MImEngineWordsInterfaceFactory : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void setLanguage();

    void setKeyboardLayoutKeys();

    void enableDictionary();
    void disableDictionary();

    void enablePrediction();
    void disablePrediction();

    void enableCorrection();
    void disableCorrection();

    void enableCompletion();
    void disableCompletion();

    void appendString();
    void appendCharacter();
    void tapKeyboard();
    void reselectString();
    void setContext();

    void addDictionaryWord();
    void removeDictionaryWord();

    void removeDictionary();

    void predictionEnabled();
    void correctionEnabled();
    void completionEnabled();

    void language();

    void keyboardLayoutKeys();

    void exportAsNokiaDictionary();
    void importNokiaUserDictionary();

    void setMaximumErrors();
    void maximumErrors();

    void setMaximumCandidates();
    void maximumCandidates();

    void suggestedCandidateIndex();

    void clearEngineBuffer();

    void totalCandidates();

    void matchedLength();

    void matchedSyllables();

    void drivers();

    void setScript();
    void script();

    void transliterate();

    void insertCharacters();
    void removeCharacters();

    void commitWord();

    //! can only be tested with a real engine
    void checkExactWordposition();

    //! test candidates can't be checked with stub (needs to be checked in engine)
    void checkErrorCorrectionCandidates();

    //! Test checks if multiple drivers can be loaded and changed on runtime
    void checkMultipleDriverLoad();
private:
    MImEngineFactory *m_subject;
};

#endif //MYTEST_H
