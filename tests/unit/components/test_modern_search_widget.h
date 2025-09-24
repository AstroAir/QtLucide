/**
 * Modern Search Widget Tests
 * Tests for search functionality and UI interactions
 */

#ifndef TEST_MODERN_SEARCH_WIDGET_H
#define TEST_MODERN_SEARCH_WIDGET_H

#include <QObject>
#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QLineEdit>
#include <QPushButton>

// Gallery includes
#include "ui/widgets/search/ModernSearchWidget.h"

class TestModernSearchWidget : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic widget tests
    void testWidgetCreation();
    void testWidgetInitialization();
    void testWidgetDestruction();
    
    // Search functionality tests
    void testSearchInput();
    void testSearchExecution();
    void testSearchClear();
    void testSearchHistory();
    
    // Filter tests
    void testFilterPanel();
    void testCategoryFilter();
    void testTagFilter();
    void testSizeFilter();
    
    // Signal tests
    void testSearchSignals();
    void testFilterSignals();
    void testHistorySignals();
    
    // UI interaction tests
    void testKeyboardNavigation();
    void testButtonClicks();
    void testContextMenu();
    
    // Performance tests
    void testSearchPerformance();
    void testLargeResultSet();
    
    // Error handling tests
    void testInvalidSearchTerms();
    void testEmptyResults();

private:
    ModernSearchWidget* m_searchWidget;
    QString m_testDataDir;
    
    // Helper methods
    void simulateUserInput(const QString& text);
    void simulateKeyPress(Qt::Key key);
    bool verifySearchResults(const QStringList& expectedResults);
};

#endif // TEST_MODERN_SEARCH_WIDGET_H
