#ifndef _APPLETTESTER_MAIN_H
#define _APPLETTESTER_MAIN_H

#include <QObject>
#include <QString>
#include <QPluginLoader>

class DcpAppletIf;
class AppletTesterWindow;

class AppletTester : public QObject
{
Q_OBJECT

public:
    AppletTester (QObject *parent = 0);
    ~AppletTester ();

    bool loadAndRun (QString &plugin);

private slots:
    void showPluginWidget (int partId);

private:
    AppletTesterWindow  *mainWindow;
    QPluginLoader       *loader;
    DcpAppletIf         *applet;
};

#endif
