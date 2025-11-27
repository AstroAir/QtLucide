/**
 * Responsive Layout Manager Tests
 */

#ifndef TEST_RESPONSIVE_LAYOUT_MANAGER_H
#define TEST_RESPONSIVE_LAYOUT_MANAGER_H

#include "ui/layouts/ResponsiveLayoutManager.h"
#include <QObject>
#include <QtTest/QtTest>

class TestResponsiveLayoutManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testManagerCreation();
    void testLayoutTransitions();
    void testAnimations();

private:
    ResponsiveLayoutManager* m_layoutManager;
};

#endif // TEST_RESPONSIVE_LAYOUT_MANAGER_H
