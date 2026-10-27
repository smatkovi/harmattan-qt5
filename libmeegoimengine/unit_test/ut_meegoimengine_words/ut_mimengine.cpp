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

#include "ut_mimengine.h"

#include "stubdriver.h"

#include <QStringList>
#include <QtDebug>
#include <QFile>
#include <QProcess>
#include <QRegExp>


//namespace
//{
//    const QString defaultDriverDir = "/usr/lib/meego-imengines/drivers/words";
//}

StubDriver *gInterface = NULL;
namespace
{
    const QString MyStubDriverPathKey = "MYSTUBDRIVER_PATH";
}

void Ut_MImEngineWordsInterfaceFactory::initTestCase()
{
    QStringList envs = QProcess::systemEnvironment();
    QString exp = QString("^%1=.*").arg(MyStubDriverPathKey);
    int index = envs.indexOf(QRegExp(exp, Qt::CaseInsensitive));
    QString stubDriverPath = 0;
    if (index != -1) {
        stubDriverPath = envs.at(index);
        exp = QString("^%1=").arg(MyStubDriverPathKey);
        stubDriverPath = stubDriverPath.remove(QRegExp(exp,Qt::CaseInsensitive));
        if (!stubDriverPath.isEmpty()) {
            qDebug() << "manual set driver path:" << stubDriverPath;
        }
    }

    m_subject = MImEngineFactory::instance();
    if (m_subject) {
        QStringList driverList = m_subject->drivers();
        qDebug()<<driverList;
        QVERIFY(driverList.count() > 0);
        QVERIFY(driverList.indexOf("stubdriver1_words") != -1);
        gInterface = (StubDriver *)m_subject->createEngineWords("stubdriver1_words");
        QVERIFY(gInterface != NULL);
        gInterface->enableDictionary(MImEngine::DictionaryTypeLanguage);
    }
}


void Ut_MImEngineWordsInterfaceFactory::cleanupTestCase()
{

}

void Ut_MImEngineWordsInterfaceFactory::init()
{
    QStringList driverList = m_subject->drivers();
    QVERIFY(driverList.indexOf("stubdriver1_words") != -1);
}

void Ut_MImEngineWordsInterfaceFactory::cleanup()
{
}

void Ut_MImEngineWordsInterfaceFactory::setLanguage()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("setLanguage", false);
    QCOMPARE(gInterface->setLanguage("TEST", MImEngine::LanguagePriorityPrimary), false);
    QVERIFY(gInterface->stubCallCount("setLanguage") == 1);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("setLanguage", true);
    QCOMPARE(gInterface->setLanguage("EN", MImEngine::LanguagePriorityPrimary), true);
    QVERIFY(gInterface->stubCallCount("setLanguage") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::setKeyboardLayoutKeys()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("setKeyboardLayoutKeys", true);
    QCOMPARE(gInterface->setKeyboardLayoutKeys(QList<MImEngine::KeyboardLayoutKey>()), true);
    QVERIFY(gInterface->stubCallCount("setKeyboardLayoutKeys") == 1);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("setKeyboardLayoutKeys", false);
    QCOMPARE(gInterface->setKeyboardLayoutKeys(QList<MImEngine::KeyboardLayoutKey>()), false);
    QVERIFY(gInterface->stubCallCount("setKeyboardLayoutKeys") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::disableDictionary()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("disableDictionary", true);
    QCOMPARE(gInterface->disableDictionary(MImEngine::DictionaryTypeLanguage), true);
    QVERIFY(gInterface->stubCallCount("disableDictionary") == 1);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("disableDictionary", false);
    QCOMPARE(gInterface->disableDictionary(MImEngine::DictionaryTypeLanguage), false);
    QVERIFY(gInterface->stubCallCount("disableDictionary") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::enableDictionary()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("enableDictionary", true);
    QCOMPARE(gInterface->enableDictionary(MImEngine::DictionaryTypeLanguage), true);
    QVERIFY(gInterface->stubCallCount("enableDictionary") == 1);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("enableDictionary", false);
    QCOMPARE(gInterface->enableDictionary(MImEngine::DictionaryTypeLanguage), false);
    QVERIFY(gInterface->stubCallCount("enableDictionary") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::disablePrediction()
{
    gInterface->stubReset();
    gInterface->disablePrediction();
    QVERIFY(gInterface->stubCallCount("disablePrediction") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::enablePrediction()
{
    gInterface->stubReset();
    gInterface->enablePrediction();
    QVERIFY(gInterface->stubCallCount("enablePrediction") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::disableCompletion()
{
    gInterface->stubReset();
    gInterface->disableCompletion();
    QVERIFY(gInterface->stubCallCount("disableCompletion") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::enableCompletion()
{
    gInterface->stubReset();
    gInterface->enableCompletion();
    QVERIFY(gInterface->stubCallCount("enableCompletion") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::disableCorrection()
{
    gInterface->stubReset();
    gInterface->disableCorrection();
    QVERIFY(gInterface->stubCallCount("disableCorrection") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::enableCorrection()
{
    gInterface->stubReset();
    gInterface->enableCorrection();
    QVERIFY(gInterface->stubCallCount("enableCorrection") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::appendString()
{
    gInterface->stubReset();
    gInterface->appendString("aak");
    QVERIFY(gInterface->stubCallCount("appendString") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::appendCharacter()
{
    gInterface->stubReset();
    QString teststr = "aak";
    for (int i = 0; i < teststr.count(); i++)
        gInterface->appendCharacter(teststr.at(i));
    QVERIFY(gInterface->stubCallCount("appendCharacter") == teststr.count());
}

void Ut_MImEngineWordsInterfaceFactory::tapKeyboard()
{
    gInterface->stubReset();
    gInterface->tapKeyboard(QPoint(), false, QChar());
    QVERIFY(gInterface->stubCallCount("tapKeyboard") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::reselectString()
{
    gInterface->stubReset();
    gInterface->reselectString("aak");
    QVERIFY(gInterface->stubCallCount("reselectString") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::setContext()
{
    gInterface->stubReset();
    gInterface->setContext("aak", 0);
    QVERIFY(gInterface->stubCallCount("setContext") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::addDictionaryWord()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("addDictionaryWord", true);
    QCOMPARE(gInterface->addDictionaryWord("test", MImEngine::DictionaryTypeLanguage), true);
    QVERIFY(gInterface->stubCallCount("addDictionaryWord") == 1);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("addDictionaryWord", false);
    QCOMPARE(gInterface->addDictionaryWord("test", MImEngine::DictionaryTypeLanguage), false);
    QVERIFY(gInterface->stubCallCount("addDictionaryWord") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::removeDictionaryWord()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("removeDictionaryWord", false);
    QCOMPARE(gInterface->removeDictionaryWord("test", MImEngine::DictionaryTypeLanguage), false);
    QVERIFY(gInterface->stubCallCount("removeDictionaryWord") == 1);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("removeDictionaryWord", true);
    QCOMPARE(gInterface->removeDictionaryWord("test", MImEngine::DictionaryTypeLanguage), true);
    QVERIFY(gInterface->stubCallCount("removeDictionaryWord") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::removeDictionary()
{
    gInterface->stubReset();
    gInterface->removeDictionary(MImEngine::DictionaryTypeLanguage);
    QVERIFY(gInterface->stubCallCount("removeDictionary") == 1);
}


void Ut_MImEngineWordsInterfaceFactory::predictionEnabled()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("predictionEnabled", true);
    QCOMPARE(gInterface->predictionEnabled(), true);
    QVERIFY(gInterface->stubCallCount("predictionEnabled") == 1);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("predictionEnabled", false);
    QCOMPARE(gInterface->predictionEnabled(), false);
    QVERIFY(gInterface->stubCallCount("predictionEnabled") == 1);
}
void Ut_MImEngineWordsInterfaceFactory::correctionEnabled()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("correctionEnabled", true);
    QCOMPARE(gInterface->correctionEnabled(), true);
    QVERIFY(gInterface->stubCallCount("correctionEnabled") == 1);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("correctionEnabled", false);
    QCOMPARE(gInterface->correctionEnabled(), false);
    QVERIFY(gInterface->stubCallCount("correctionEnabled") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::completionEnabled()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("completionEnabled", true);
    QCOMPARE(gInterface->completionEnabled(), true);
    QVERIFY(gInterface->stubCallCount("completionEnabled") == 1);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("completionEnabled", false);
    QCOMPARE(gInterface->completionEnabled(), false);
    QVERIFY(gInterface->stubCallCount("completionEnabled") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::language()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<QString>("language", QString("EN"));
    QVERIFY(gInterface->language() == "EN");
    QVERIFY(gInterface->stubCallCount("language") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::keyboardLayoutKeys()
{
    const QRect rect(1, 1, 2, 2);
    const QChar symbol('a');
    const MImEngine::KeyboardLayoutKeyType type = MImEngine::KeyboardLayoutKeyRegional;

    MImEngine::KeyboardLayoutKey key;
    key.rect = rect;
    key.type = type;
    key.symbols.append(symbol);
    QList<MImEngine::KeyboardLayoutKey> keys;
    keys.append(key);

    gInterface->stubReset();
    gInterface->stubSetReturnValue<QList<MImEngine::KeyboardLayoutKey> >("keyboardLayoutKeys", keys);
    QList<MImEngine::KeyboardLayoutKey> keys2 = gInterface->keyboardLayoutKeys();
    QVERIFY(keys2.size() == 1);
    QVERIFY(keys2[0].rect == rect);
    QVERIFY(keys2[0].type == type);
    QVERIFY(keys2[0].symbols.size() == 1);
    QVERIFY(keys2[0].symbols[0] == symbol);
    QVERIFY(gInterface->stubCallCount("keyboardLayoutKeys") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::exportAsNokiaDictionary()
{
    QString fileName = "test.xml";
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("exportAsNokiaDictionary", true);
    QCOMPARE(gInterface->exportAsNokiaDictionary(fileName), true);
    QVERIFY(gInterface->stubCallCount("exportAsNokiaDictionary") == 1);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("exportAsNokiaDictionary", false);
    QCOMPARE(gInterface->exportAsNokiaDictionary(fileName), false);
    QVERIFY(gInterface->stubCallCount("exportAsNokiaDictionary") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::importNokiaUserDictionary()
{
    QString fileName = "test.xml";
    if (QFile::exists(fileName))
        QFile::remove(fileName);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("importNokiaUserDictionary", true);
    QCOMPARE(gInterface->importNokiaUserDictionary(fileName), true);
    QCOMPARE(gInterface->error().isEmpty(), true);
    gInterface->stubReset();
    QFile testFile(fileName);
    testFile.open(QIODevice::WriteOnly);
    testFile.close();
    QCOMPARE(gInterface->importNokiaUserDictionary(fileName), false);
    QVERIFY(gInterface->stubCallCount("importNokiaUserDictionary") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::setMaximumErrors()
{
    gInterface->stubReset();
    gInterface->setMaximumErrors(5);
    QVERIFY(gInterface->stubCallCount("setMaximumErrors") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::maximumErrors()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<int>("maximumErrors", 5);
    QVERIFY(gInterface->maximumErrors() == 5);
    QVERIFY(gInterface->stubCallCount("maximumErrors") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::setMaximumCandidates()
{
    gInterface->stubReset();
    gInterface->setMaximumCandidates(8);
    QVERIFY(gInterface->stubCallCount("setMaximumCandidates") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::maximumCandidates()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<int>("maximumCandidates", 8);
    QVERIFY(gInterface->maximumCandidates() == 8);
    QVERIFY(gInterface->stubCallCount("maximumCandidates") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::suggestedCandidateIndex()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<int>("suggestedCandidateIndex", 2);
    QVERIFY(gInterface->suggestedCandidateIndex() == 2);
    QVERIFY(gInterface->stubCallCount("suggestedCandidateIndex") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::clearEngineBuffer()
{
    gInterface->stubReset();
    gInterface->clearEngineBuffer();
    QVERIFY(gInterface->stubCallCount("clearEngineBuffer") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::drivers()
{
    qDebug() << m_subject->drivers().count();
    QVERIFY(m_subject->drivers().count() >= 1);
}

void Ut_MImEngineWordsInterfaceFactory::checkExactWordposition()
{
    gInterface->setExactWordPositionInList(MImEngine::ExactInListOff);
    QVERIFY(gInterface->stubCallCount("setExactWordPositionInList") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::checkErrorCorrectionCandidates()
{
    //this can only be checked with a 'real' engine
    QStringList list;
    list << "candidates";
    gInterface->stubSetReturnValue<QStringList>("candidates", list);
    QVERIFY(gInterface->candidates() == list);
    QVERIFY(gInterface->stubCallCount("candidates") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::checkMultipleDriverLoad()
{
    QString driver1, driver2;

    driver1 = "stubdriver1_words";
    driver2 = "stubdriver2_words";

    QStringList driverList = m_subject->drivers();
    //check that driver exists
    QVERIFY(driverList.indexOf(driver1) != -1);
    QVERIFY(driverList.indexOf(driver2) != -1);
    QVERIFY(gInterface->driver() == driver1);
    QVERIFY(driverList.count() >= 2);

    // set second driver
    gInterface = (StubDriver *)m_subject->createEngineWords(driver2);
    QVERIFY(gInterface->driver() == driver2);

    //check that all driver exists
    QVERIFY(driverList.indexOf(driver1) != -1);
    QVERIFY(driverList.indexOf(driver2) != -1);

    gInterface = (StubDriver *)m_subject->createEngineWords(driver1);
    QVERIFY(gInterface->driver() == driver1);
    QVERIFY(driverList.indexOf(driver1) != -1);
    QVERIFY(driverList.indexOf(driver2) != -1);

    driverList = m_subject->drivers();
    QVERIFY(driverList.count() >= 2);
}

void Ut_MImEngineWordsInterfaceFactory::matchedLength()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<int>("matchedLength", 2);
    QVERIFY(gInterface->matchedLength() == 2);
    QVERIFY(gInterface->stubCallCount("matchedLength") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::matchedSyllables()
{
    QStringList sValue;
    gInterface->stubSetReturnValue<QStringList>("matchedSyllables", sValue);
    QVERIFY(gInterface->matchedSyllables() == sValue);
    QVERIFY(gInterface->stubCallCount("matchedSyllables") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::totalCandidates()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<int>("totalCandidates", 5);
    QVERIFY(gInterface->totalCandidates() == 5);
    QVERIFY(gInterface->stubCallCount("totalCandidates") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::setScript()
{
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("setScript", true);
    QCOMPARE(gInterface->setScript("testValue"), true);
    QVERIFY(gInterface->stubCallCount("setScript") == 1);
    gInterface->stubReset();
    gInterface->stubSetReturnValue<bool>("setScript", false);
    QCOMPARE(gInterface->setScript("testValue"), false);
    QVERIFY(gInterface->stubCallCount("setScript") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::script()
{
    QString sValue("testValue");
    gInterface->stubSetReturnValue<QString>("script", sValue);
    QVERIFY(gInterface->script() == sValue);
    QVERIFY(gInterface->stubCallCount("script") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::transliterate()
{
    QString sValue("FakeReturnValue");
    gInterface->stubReset();
    gInterface->stubSetReturnValue<QString>("transliterate", sValue);
    QVERIFY(gInterface->transliterate(QString("FakeTargetScript"), 0) == sValue);
    QVERIFY(gInterface->stubCallCount("transliterate") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::insertCharacters()
{
    gInterface->stubReset();
    gInterface->insertCharacters("a");
    QVERIFY(gInterface->stubCallCount("insertCharacters") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::removeCharacters()
{
    gInterface->stubReset();
    gInterface->removeCharacters();
    QVERIFY(gInterface->stubCallCount("removeCharacters") == 1);
}

void Ut_MImEngineWordsInterfaceFactory::commitWord()
{
    gInterface->stubReset();
    gInterface->commitWord();
    QVERIFY(gInterface->stubCallCount("commitWord") == 1);
}

QTEST_MAIN(Ut_MImEngineWordsInterfaceFactory)
