/*
 * This file is part of meego-keyboard 
 *
 * Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 *
 * Contact: Mohammad Anwari <Mohammad.Anwari@nokia.com>
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list 
 * of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list 
 * of conditions and the following disclaimer in the documentation and/or other materials 
 * provided with the distribution.
 * Neither the name of Nokia Corporation nor the names of its contributors may be 
 * used to endorse or promote products derived from this software without specific 
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
 * THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "ut_enginehandlertonal.h"
#include <mkeyboardhost.h>
#include <enginehandler.h>
#include <enginemanager.h>
#include <enginehandlertonal.h>
#include <MApplication>
#include "mkeyboardhost.h"
#include "minputmethodhoststub.h"

void Ut_EngineHandlerTonal::initTestCase()
{
    static int argc = 1;
    static char *app_name[1] = { (char *) "ut_enginehandlertonal" };

    // Avoid waiting if im server is not responding
    MApplication::setLoadMInputContext(false);
    app = new MApplication(argc, app_name);

    inputMethodHost = new MInputMethodHostStub;
    mainWindow = new QWidget;
    keyboardHost = MKeyboardHost::create(inputMethodHost, mainWindow);
}

void Ut_EngineHandlerTonal::cleanupTestCase()
{
    delete mainWindow;
    mainWindow = 0;
    delete inputMethodHost;
    inputMethodHost = 0;
    delete app;
    app = 0;
}

void Ut_EngineHandlerTonal::init()
{
    subject = new EngineHandlerTonal(*keyboardHost);
}

void Ut_EngineHandlerTonal::cleanup()
{
    delete subject;
    subject = 0;
}

void Ut_EngineHandlerTonal::testCreate()
{
    QVERIFY(subject != 0);
}

void Ut_EngineHandlerTonal::testPlaceVietnameseTone()
{
    EngineManager::instance().updateLanguage("vi");
    QChar tone(QChar(0x0301));
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("hoa"), 3, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("hóa"));
    QCOMPARE(subject->preeditCursorPosition(), 3);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("quy"), 3, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("quý"));
    QCOMPARE(subject->preeditCursorPosition(), 3);
    tone = QChar(0x0323);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("hoat"), 4, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("hoạt"));
    QCOMPARE(subject->preeditCursorPosition(), 4);
    tone = QChar(0x0300);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("ngươi"), 5, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("người"));
    QCOMPARE(subject->preeditCursorPosition(), 5);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("thương"), 5, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("thường"));
    QCOMPARE(subject->preeditCursorPosition(), 5);
    tone = QChar(0x0309);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("biêu"), 4, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("biểu"));
    QCOMPARE(subject->preeditCursorPosition(), 4);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("ơ"), 1, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("ở"));
    QCOMPARE(subject->preeditCursorPosition(), 1);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("anh"), 3, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("ảnh"));
    QCOMPARE(subject->preeditCursorPosition(), 3);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("gia"), 3, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("giả"));
    QCOMPARE(subject->preeditCursorPosition(), 3);

    // Trying replacements also
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("hoạt"), 4, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("hoảt"));
    QCOMPARE(subject->preeditCursorPosition(), 4);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("thường"), 5, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("thưởng"));
    QCOMPARE(subject->preeditCursorPosition(), 5);
    tone = QChar(0x0301);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("người"), 5, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("ngưới"));
    QCOMPARE(subject->preeditCursorPosition(), 5);
    tone = QChar(0x0323);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("biểu"), 4, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("biệu"));
    QCOMPARE(subject->preeditCursorPosition(), 4);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("ảnh"), 3, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("ạnh"));
    QCOMPARE(subject->preeditCursorPosition(), 3);
    tone = QChar(0x0300);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("ở"), 1, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("ờ"));
    QCOMPARE(subject->preeditCursorPosition(), 1);

    // And some cursor position tests when not at the end of a word
    tone = QChar(0x0323);
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("biểu"), 8, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("biệu"));
    QCOMPARE(subject->preeditCursorPosition(), 4); // the real length
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("ảnh"), 1, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("ạnh"));
    QCOMPARE(subject->preeditCursorPosition(), 1); // remains after the tone
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("ảnh"), -1, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("ạnh"));
    QCOMPARE(subject->preeditCursorPosition(), 3); // end of the string if asking for a negative
    QVERIFY(subject->placeVietnameseTone(QString::fromUtf8("biểu"), 3, tone));
    QCOMPARE(subject->preeditString(), QString::fromUtf8("biệu"));
    QCOMPARE(subject->preeditCursorPosition(), 3);

    // And some broken things
    QVERIFY(!subject->placeVietnameseTone(QString::fromUtf8(""), 1, tone));
    QVERIFY(!subject->placeVietnameseTone(QString::fromUtf8("nh"), 2, tone));
    QVERIFY(!subject->placeVietnameseTone(QString(QChar(0x0301)), 1, tone));
}

QTEST_APPLESS_MAIN(Ut_EngineHandlerTonal);

