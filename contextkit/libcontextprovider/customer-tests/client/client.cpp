#include "commandwatcher.h"

#include <QCoreApplication>
#include <QDebug>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    new CommandWatcher(STDIN_FILENO, QCoreApplication::instance());

    return app.exec();
}
