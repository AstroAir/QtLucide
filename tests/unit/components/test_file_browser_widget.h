/**
 * File Browser Widget Tests
 */

#ifndef TEST_FILE_BROWSER_WIDGET_H
#define TEST_FILE_BROWSER_WIDGET_H

#include <QObject>
#include <QtTest/QtTest>
#include "ui/widgets/panels/FileBrowserWidget.h"

class TestFileBrowserWidget : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testWidgetCreation();
    void testNavigation();
    void testImageFiltering();
    void testFileSelection();

private:
    FileBrowserWidget* m_browserWidget;
};

#endif // TEST_FILE_BROWSER_WIDGET_H
