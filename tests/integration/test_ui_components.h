/**
 * QtLucide UI Component Tests - Header
 *
 * Comprehensive tests for Gallery application UI components including
 * SearchWidget, CategoryFilterWidget, IconGridWidget, IconDetailsPanel, and PreferencesDialog.
 */

#ifndef TEST_UI_COMPONENTS_H
#define TEST_UI_COMPONENTS_H

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMouseEvent>
#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QTimer>
#include <QWidget>
#include <QtTest/QtTest>

// Forward declarations
class SearchWidget;
class CategoryFilterWidget;
class IconGridWidget;
class IconDetailsPanel;
class PreferencesDialog;
class IconMetadataManager;

namespace lucide {
class QtLucide;
}

class TestUIComponents : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // SearchWidget tests
    void testSearchWidgetCreation();
    void testSearchWidgetBasicSearch();
    void testSearchWidgetAdvancedSearch();
    void testSearchWidgetRegexSearch();
    void testSearchWidgetCaseSensitivity();
    void testSearchWidgetSearchHistory();
    void testSearchWidgetClearSearch();
    void testSearchWidgetKeyboardShortcuts();
    void testSearchWidgetEdgeCases();

    // CategoryFilterWidget tests
    void testCategoryFilterCreation();
    void testCategoryFilterBasicFiltering();
    void testCategoryFilterMultipleCategories();
    void testCategoryFilterStateManagement();
    void testCategoryFilterPersistence();
    void testCategoryFilterClearAll();
    void testCategoryFilterUserInteraction();

    // IconGridWidget tests
    void testIconGridCreation();
    void testIconGridLayout();
    void testIconGridSizeConfiguration();
    void testIconGridViewModes();
    void testIconGridSelection();
    void testIconGridScrolling();
    void testIconGridKeyboardNavigation();
    void testIconGridMouseInteraction();
    void testIconGridPerformance();
    void testIconGridResponsiveness();

    // IconDetailsPanel tests
    void testIconDetailsPanelCreation();
    void testIconDetailsPanelDataDisplay();
    void testIconDetailsPanelIconPreview();
    void testIconDetailsPanelMetadataDisplay();
    void testIconDetailsPanelCopyFunctions();
    void testIconDetailsPanelExportOptions();
    void testIconDetailsPanelUserInteractions();

    // PreferencesDialog tests
    void testPreferencesDialogCreation();
    void testPreferencesDialogSettingsValidation();
    void testPreferencesDialogSettingsPersistence();
    void testPreferencesDialogThemeSettings();
    void testPreferencesDialogPerformanceSettings();
    void testPreferencesDialogExportSettings();
    void testPreferencesDialogRestoreDefaults();
    void testPreferencesDialogApplyCancel();

    // Cross-component interaction tests
    void testSearchFilterIntegration();
    void testGridDetailsIntegration();
    void testPreferencesGridIntegration();
    void testComponentStateSync();

private:
    // Helper methods
    void createTestComponents();
    void destroyTestComponents();
    void simulateUserInput(QWidget* widget, const QString& text);
    void simulateKeyPress(QWidget* widget, Qt::Key key,
                          Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    void simulateMouseClick(QWidget* widget, const QPoint& position);
    bool waitForSignal(QObject* sender, const char* signal, int timeout = 1000);
    void verifyComponentState(QWidget* component, const QString& expectedState);

    // Test components
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    SearchWidget* m_searchWidget;
    CategoryFilterWidget* m_categoryFilter;
    IconGridWidget* m_iconGrid;
    IconDetailsPanel* m_detailsPanel;
    PreferencesDialog* m_preferencesDialog;

    // Test data
    QStringList m_testIconNames;
    QStringList m_testCategories;
    QWidget* m_testParent;

    // Performance tracking
    QElapsedTimer m_performanceTimer;

    // Test configuration
    static constexpr int UI_RESPONSE_TIMEOUT_MS = 1000;
    static constexpr int ANIMATION_TIMEOUT_MS = 500;
    static constexpr int SEARCH_DELAY_MS = 100;
};

#endif // TEST_UI_COMPONENTS_H
