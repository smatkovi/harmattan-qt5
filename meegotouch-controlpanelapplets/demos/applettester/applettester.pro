TEMPLATE = app

CONFIG += \
    meegotouchcore \
    gui \
    debug

INCLUDEPATH += . \
    /usr/include/duicontrolpanel

HEADERS += \
    main.h \
    window.h

SOURCES += \
    main.cpp \
    window.cpp

