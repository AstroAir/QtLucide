/**
 * Gallery UI Interactions Tests
 */

#ifndef TEST_GALLERY_UI_INTERACTIONS_H
#define TEST_GALLERY_UI_INTERACTIONS_H

#include <QObject>
#include <QtTest/QtTest>
#include "ui/windows/GalleryMainWindow.h"

class TestGalleryUIInteractions : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testButtonClicks();
    void testKeyboardShortcuts();
    void testMenuInteractions();
    void testDialogInteractions();

private:
    GalleryMainWindow* m_mainWindow;
};

#endif // TEST_GALLERY_UI_INTERACTIONS_H
