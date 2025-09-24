/**
 * Modern Sidebar Widget Tests
 */

#ifndef TEST_MODERN_SIDEBAR_WIDGET_H
#define TEST_MODERN_SIDEBAR_WIDGET_H

#include <QObject>
#include <QtTest/QtTest>
#include "ui/widgets/panels/ModernSidebarWidget.h"

class TestModernSidebarWidget : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testWidgetCreation();
    void testCategoryManagement();
    void testCollapseExpand();
    void testSignalEmission();

private:
    ModernSidebarWidget* m_sidebarWidget;
};

#endif // TEST_MODERN_SIDEBAR_WIDGET_H
