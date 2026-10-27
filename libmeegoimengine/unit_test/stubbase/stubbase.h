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

#ifndef STUBBASE_H
#define STUBBASE_H

#include <QList>
#include <QVariant>
#include <QMap>
#include <QtDebug>

#include <QDebug>

class ParameterBase
{
public:
    virtual ~ParameterBase() {
        // Class needs to have at least one virtual function to be polymorphic
        // (and thus enable dynamic_cast)
    }
protected:
    ParameterBase() {
    }
};

template <typename T>
class Parameter : public ParameterBase
{
public:
    Parameter(T value) : ParameterBase(), data(value) {
    }
    T data;
};

class MethodCall
{
public:
    MethodCall(QString name, QList<ParameterBase *> params, ParameterBase *returnValue)
        : _name(name),
          _params(params),
          _returnValue(returnValue) {
    }

    virtual ~MethodCall() {
        foreach(ParameterBase * p, _params) {
            delete p;
        }
    }

    QString name() {
        return _name;
    }

    QList<ParameterBase *> params() {
        return _params;
    }

    template <typename T>
    T parameter(int number) {
        if (number >= _params.count()) {
            qDebug() << "MethodCall::" << __func__ << ": method " << _name << " does not have parameter #" << number
                     << ". Check your test code.";
        }
        Parameter<T>* param = dynamic_cast<Parameter<T>* >(_params[number]);
        if (!param) {
            qDebug() << "MethodCall::" << __func__ << ": failed dynamic_cast, check that parameter type matches parameter number";
        }
        return param->data;
    }

    template <typename T>
    T returnValue() {
        Parameter<T>* value = dynamic_cast<Parameter<T>*>(_returnValue);

        if (!value) {
            qDebug() << "MethodCall::" << __func__ << ": failed dynamic_cast, check that type matches return value";
        }
        return value->data;
    }

    bool returnValueExists() {
        return (_returnValue != NULL);
    }

private:
    QString _name;
    QList<ParameterBase *> _params;
    ParameterBase *_returnValue;

};

class StubBase
{
public:
    // Return a list of method calls
    QList<MethodCall *> stubCallHistory() const;

    // Return a reference to the last method call
    MethodCall &stubLastCall() const;

    // Access parameters of the last method call
    template <typename T>
    T stubLastParameters(int number) const;

    // Access parameters of the last method call
    MethodCall &stubLastCallTo(QString method) const;

    // Return the number of times a method has been called
    int stubCallCount(QString method) const;

    // Set the return value for next call of methodName
    template <typename T>
    void stubSetReturnValue(QString methodName, T value) const;

    // Return the return value set for methodName
    template <typename T>
    T &stubReturnValue(QString methodName) const;



    // For use by stubs
    virtual ~StubBase();
    void stubReset() const;
    ParameterBase *stubReturnValue(QString methodName) const;
    void stubMethodEntered(QString methodName, QList<ParameterBase *> params) const;
    void stubMethodEntered(QString methodName) const;

private:
    mutable QMap<QString, ParameterBase *> _stubReturnValues;
    mutable QMap<QString, int> _stubCallCounts;
    mutable QList<MethodCall *> _stubCallHistory;

};

template <typename T>
void StubBase::stubSetReturnValue(QString methodName, T value) const
{    
    Parameter<T>* param = new Parameter<T>(value);
    _stubReturnValues[methodName] = param;

    if (methodName == "screenSize") {
        qDebug() << "value =" << (*(QSize*)(&value))
                << " _stubReturnValues[methodName]  = " <<  _stubReturnValues[methodName];
    }
}

template <typename T>
T &StubBase::stubReturnValue(QString methodName) const
{
    if (! _stubReturnValues.contains(methodName)) {
        stubSetReturnValue<T>(methodName, T());
    }

    ParameterBase *base = _stubReturnValues[methodName];
    Parameter<T>* param = dynamic_cast<Parameter<T>*>(base);
    if (!param) {
        qDebug() << "StubBase"<< base << __func__ << ": failed dynamic_cast, check that return value type matches the method; check also that you have used stubSetReturnValue(" << methodName << ")";
    }
    return param->data;

}

template <typename T>
T StubBase::stubLastParameters(int number) const
{
    MethodCall &call = stubLastCall();
    return call.parameter<T>(number);
}

#endif
