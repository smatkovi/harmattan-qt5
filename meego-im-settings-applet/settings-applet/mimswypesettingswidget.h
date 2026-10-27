/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIMSWYPESETTINGSWIDGET_H
#define MIMSWYPESETTINGSWIDGET_H

#include <DcpStylableWidget>

class QGraphicsLinearLayout;
class MLabel;
class MAbstractInputMethodSettings;

class MImSwypeSettingsWidget : public DcpStylableWidget
{
    Q_OBJECT
public:
    explicit MImSwypeSettingsWidget(MAbstractInputMethodSettings *settings,
                                    QGraphicsWidget *parent = 0);

    virtual ~MImSwypeSettingsWidget();

protected:
    void initWidget();

    //! reimp
    virtual void retranslateUi();
    virtual bool back();
    virtual QString title() const;
    //! reimp_end

signals:

private:
    QGraphicsLinearLayout *mainLayout;
    MLabel *headerLabel;
    MAbstractInputMethodSettings *settings;
};

#endif // MIMSWYPESETTINGSWIDGET_H
