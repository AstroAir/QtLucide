/**
 * Icon Metadata Manager Tests
 */

#ifndef TEST_ICON_METADATA_MANAGER_H
#define TEST_ICON_METADATA_MANAGER_H

#include <QObject>
#include <QtTest/QtTest>
#include "core/managers/IconMetadataManager.h"

class TestIconMetadataManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testManagerCreation();
    void testMetadataRetrieval();
    void testMetadataStorage();

private:
    IconMetadataManager* m_metadataManager;
};

#endif // TEST_ICON_METADATA_MANAGER_H
