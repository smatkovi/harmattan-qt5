# Common file for tests, it can be included into tests.pro

if (!isEmpty(TEST_PKG)) {
    TEST_PKG_NAME = $$TEST_PKG
} else {
    TEST_PKG_NAME = meegotouch-controlpanelapplets-tests
}

include(../localconfig.pri)
include(coverage.pri)
