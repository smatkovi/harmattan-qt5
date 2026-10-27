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

#ifndef MIMENGINETYPES_H
#define MIMENGINETYPES_H

#include <QChar>
#include <QList>
#include <QRect>

namespace MImEngine
{
//!Language Priority
    enum LanguagePriority {
        LanguagePriorityPrimary,     /// Language is used as primary, all lookups will first happen in this language
        LanguagePrioritySecondary    /// Language is used as secondary
    };

//!Dictionary Type
    enum DictionaryType {
        DictionaryTypeInvalid,        //< Invalid dictionary type.
        DictionaryTypeLanguage,       //< Dictionary is the vendor supplied dictionary
        DictionaryTypeOEM,            //< Dictionary is OEM/Operator/Nokia supplied dictionary
        DictionaryTypeSessionContext, //< Dictionary is used in short term (e.g. dictionary created from an incoming SMS message)
        DictionaryTypeBlacklist,      //< Dictionary containing words which shall not appear in the UI
        DictionaryTypePersonal,       //< Dictionary auto-created by user
        DictionaryTypeDomainContext,  //< Dictionary is used by applications which fall into same domain (e.g. Chat, web browsing)
    };

    enum ExactInListType {
        ExactInListOff,     //< The exact word is deleted from the candidate list
        ExactInListFirst,   //< The exact word will appear in first position
        ExactInListLast     //< The exact word will appear in last position
    }; 

    //!Key Type
    enum KeyboardLayoutKeyType
    {
        KeyboardLayoutKeyRegional,      //< Key is a regional key (alphabets etc.)
        KeyboardLayoutKeyNonRegional    //< Key is a non regional key (punctuations etc.)
    };

    //!Keyboard Layout Key
    struct KeyboardLayoutKey
    {
        QRect rect;                     //< Rect of the key
        QList<QChar> symbols;           //< List of symbols associated with the key
        KeyboardLayoutKeyType type;     //< Type of the key
    };
}
#endif //MEEGOIMENGINETYPES_H
