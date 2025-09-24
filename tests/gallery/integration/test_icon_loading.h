/**
 * Gallery Icon Loading Tests - Header
 */

#ifndef TEST_ICON_LOADING_H
#define TEST_ICON_LOADING_H

#include <QObject>
#include <QtTest/QtTest>

namespace lucide {
class QtLucide;
}

class IconMetadataManager;

class TestGalleryIconLoading : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testMetadataManagerInitialization();
    void testMetadataLoading();
    void testIconItemCreation();
    void testIconItemRendering();
    void testGalleryIconAccess();
    void testResourceIntegrity();

private:
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
};

#endif // TEST_GALLERY_ICON_LOADING_H
