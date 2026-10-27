#include "security.h"
#include "pagefactory.h"
#include "dcpappletmanager.h"

#ifndef DCP_WITHOUT_CREDS
#include <sys/creds.h>
#endif
#include <QString>
#include <DcpAppletMetadata>
#include <dcpdebug.h>

void Security::restrictTo (DcpAppletMetadata* metadata)
{
#ifndef DCP_WITHOUT_CREDS
    if (!PageFactory::isInProcessApplets()) {
        creds_confine (qPrintable(metadata->fullBinary()));
    }
#else
    // no Aegis credentials to confine the applet with
    Q_UNUSED(metadata);
#endif
}

void Security::loadAppletRestricted (const QString& appletName)
{
    DcpAppletManager* db = DcpAppletManager::instance();
    dcp_failfunc_unless (db);
    DcpAppletMetadata* metadata = db->metadata (appletName);
    dcp_failfunc_unless (metadata);

    // restrict:
    restrictTo (metadata);

    // load:
    db->applet (appletName);
}

