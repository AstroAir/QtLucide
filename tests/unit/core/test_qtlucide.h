/**
 * QtLucide Core Functionality Tests - Header
 */

#ifndef TEST_QTLUCIDE_H
#define TEST_QTLUCIDE_H

#include <QObject>
#include <QtTest/QtTest>

namespace lucide {
class QtLucide;
}

class TestQtLucide : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInitialization();
    void testIconCreation();
    void testIconByName();
    void testIconByEnum();
    void testDefaultOptions();
    void testCustomOptions();
    void testAvailableIcons();
    void testInvalidIcon();

private:
    lucide::QtLucide* m_lucide;
};

#endif // TEST_QTLUCIDE_H
