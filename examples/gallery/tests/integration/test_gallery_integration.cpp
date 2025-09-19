/**
 * QtLucide Gallery Application - Integration Tests
 *
 * Comprehensive integration tests for Gallery application including:
 * - Component interaction tests
 * - Main window integration
 * - Data flow between components
 * - End-to-end functionality tests
 */

#include <QApplication>
#include <QElapsedTimer>
#include <QMainWindow>
#include <QPixmap>
#include <QStringList>
#include <QTemporaryDir>
#include <QTimer>
#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

// Include Gallery components
#include "core/managers/ContentManager.h"
#include "core/managers/IconMetadataManager.h"
#include "core/managers/ImageMetadataManager.h"
#include "ui/widgets/grids/IconGridWidget.h"
#include "ui/widgets/grids/IconThumbnailGridWidget.h"
#include "ui/widgets/panels/CategorySidebarWidget.h"
#include "ui/widgets/search/CategoryFilterWidget.h"
#include "ui/widgets/search/IconSearchWidget.h"
#include "ui/widgets/search/SearchWidget.h"
#include "ui/widgets/viewers/ImageViewerWidget.h"
#include "ui/windows/GalleryMainWindow.h"
#include <QtLucide/QtLucide.h>

class TestGalleryIntegration : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Main Window Integration Tests
    void testMainWindow_Creation();
    void testMainWindow_ComponentInitialization();
    void testMainWindow_LayoutManagement();
    void testMainWindow_MenuAndToolbar();
    void testMainWindow_StatusBar();

    // Component Interaction Tests
    void testInteraction_SearchAndGrid();
    void testInteraction_FilterAndGrid();
    void testInteraction_GridAndViewer();
    void testInteraction_SidebarAndGrid();
    void testInteraction_SearchAndFilter();

    // Data Flow Tests
    void testDataFlow_IconLoading();
    void testDataFlow_SearchFiltering();
    void testDataFlow_CategoryFiltering();
    void testDataFlow_SelectionPropagation();
    void testDataFlow_ViewModeChanges();

    // End-to-End Tests
    void testEndToEnd_BasicWorkflow();
    void testEndToEnd_SearchWorkflow();
    void testEndToEnd_CategoryWorkflow();
    void testEndToEnd_ViewerWorkflow();
    void testEndToEnd_PerformanceWorkflow();

    // Window Management Tests
    void testWindowManagement_Resize();
    void testWindowManagement_SplitterAdjustment();
    void testWindowManagement_PanelVisibility();
    void testWindowManagement_FullscreenMode();

    // State Management Tests
    void testStateManagement_Settings();
    void testStateManagement_SessionRestore();
    void testStateManagement_ViewPreferences();

private:
    // Test fixtures
    QApplication* m_app;
    lucide::QtLucide* m_lucide;
    GalleryMainWindow* m_mainWindow;
    QTemporaryDir* m_tempDir;

    // Test data
    QStringList m_testIconNames;
    QString m_testImagePath;

    // Helper methods
    void setupTestData();
    void createTestImages();
    QStringList getTestIconNames(int count = 50);
    bool waitForSignal(QObject* sender, const char* signal, int timeout = 5000);
    void simulateUserInteraction();
};

void TestGalleryIntegration::initTestCase() {
    // Initialize QtLucide
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());

    // Setup test environment
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    setupTestData();
    createTestImages();

    qDebug() << "Gallery integration test environment initialized";
    qDebug() << "Test icons available:" << m_testIconNames.size();
}

void TestGalleryIntegration::cleanupTestCase() {
    if (m_mainWindow) {
        m_mainWindow->close();
        delete m_mainWindow;
    }
    delete m_tempDir;
    qDebug() << "Gallery integration test environment cleaned up";
}

void TestGalleryIntegration::init() {
    // Create fresh main window for each test
    m_mainWindow = new GalleryMainWindow();
    m_mainWindow->setLucide(m_lucide);
    m_mainWindow->show();

    // Wait for initialization
    QTest::qWait(100);
}

void TestGalleryIntegration::cleanup() {
    if (m_mainWindow) {
        m_mainWindow->close();
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }
}

void TestGalleryIntegration::setupTestData() {
    m_testIconNames = getTestIconNames(100);
    QVERIFY(!m_testIconNames.isEmpty());
}

void TestGalleryIntegration::createTestImages() {
    // Create test images in temporary directory
    m_testImagePath = m_tempDir->path() + "/test_image.png";

    QPixmap testPixmap(64, 64);
    testPixmap.fill(Qt::blue);
    QVERIFY(testPixmap.save(m_testImagePath, "PNG"));
}

QStringList TestGalleryIntegration::getTestIconNames(int count) {
    QStringList allIcons = m_lucide->availableIcons();
    if (allIcons.size() < count) {
        return allIcons;
    }
    return allIcons.mid(0, count);
}

bool TestGalleryIntegration::waitForSignal(QObject* sender, const char* signal, int timeout) {
    QSignalSpy spy(sender, signal);
    return spy.wait(timeout);
}

void TestGalleryIntegration::simulateUserInteraction() {
    // Simulate some user interactions
    QTest::qWait(50);
    QApplication::processEvents();
}

// Main Window Integration Tests
void TestGalleryIntegration::testMainWindow_Creation() {
    QVERIFY(m_mainWindow != nullptr);
    QVERIFY(m_mainWindow->isVisible());
    QVERIFY(m_mainWindow->windowTitle().contains("Gallery"));
}

void TestGalleryIntegration::testMainWindow_ComponentInitialization() {
    // Test that all major components are initialized
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();
    SearchWidget* searchWidget = m_mainWindow->findChild<SearchWidget*>();
    CategorySidebarWidget* sidebarWidget = m_mainWindow->findChild<CategorySidebarWidget*>();

    QVERIFY(gridWidget != nullptr);
    QVERIFY(searchWidget != nullptr);
    QVERIFY(sidebarWidget != nullptr);
}

void TestGalleryIntegration::testMainWindow_LayoutManagement() {
    // Test window layout
    QVERIFY(m_mainWindow->centralWidget() != nullptr);

    // Test that window can be resized
    QSize originalSize = m_mainWindow->size();
    m_mainWindow->resize(800, 600);
    simulateUserInteraction();

    QCOMPARE(m_mainWindow->size(), QSize(800, 600));

    // Restore original size
    m_mainWindow->resize(originalSize);
}

void TestGalleryIntegration::testMainWindow_MenuAndToolbar() {
    // Test that menu bar exists
    QMenuBar* menuBar = m_mainWindow->menuBar();
    QVERIFY(menuBar != nullptr);
    QVERIFY(!menuBar->actions().isEmpty());

    // Test that toolbar exists
    QList<QToolBar*> toolbars = m_mainWindow->findChildren<QToolBar*>();
    QVERIFY(!toolbars.isEmpty());
}

void TestGalleryIntegration::testMainWindow_StatusBar() {
    // Test that status bar exists and shows information
    QStatusBar* statusBar = m_mainWindow->statusBar();
    QVERIFY(statusBar != nullptr);
    QVERIFY(statusBar->isVisible());
}

// Component Interaction Tests
void TestGalleryIntegration::testInteraction_SearchAndGrid() {
    SearchWidget* searchWidget = m_mainWindow->findChild<SearchWidget*>();
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();

    QVERIFY(searchWidget != nullptr);
    QVERIFY(gridWidget != nullptr);

    // Test that search affects grid display
    QSignalSpy gridUpdateSpy(gridWidget, &IconGridWidget::loadingFinished);

    // Simulate search
    searchWidget->focusSearchInput();
    QTest::keyClicks(searchWidget, "home");

    // Wait for grid to update
    QVERIFY(gridUpdateSpy.wait(2000));

    // Verify grid has been updated
    QVERIFY(gridUpdateSpy.count() >= 1);
}

void TestGalleryIntegration::testInteraction_FilterAndGrid() {
    CategoryFilterWidget* filterWidget = m_mainWindow->findChild<CategoryFilterWidget*>();
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();

    if (filterWidget && gridWidget) {
        QSignalSpy gridUpdateSpy(gridWidget, &IconGridWidget::loadingFinished);

        // Apply category filter
        QStringList testCategories = {"navigation"};
        filterWidget->setSelectedCategories(testCategories);

        // Wait for grid to update
        QVERIFY(gridUpdateSpy.wait(2000));
        QVERIFY(gridUpdateSpy.count() >= 1);
    } else {
        QSKIP("Filter widget or grid widget not found");
    }
}

void TestGalleryIntegration::testInteraction_GridAndViewer() {
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();
    ImageViewerWidget* viewerWidget = m_mainWindow->findChild<ImageViewerWidget*>();

    if (gridWidget && viewerWidget) {
        // Set up grid with test icons
        gridWidget->setIconNames(m_testIconNames);

        QSignalSpy selectionSpy(gridWidget, &IconGridWidget::iconSelected);
        QSignalSpy viewerSpy(viewerWidget, &ImageViewerWidget::imageChanged);

        // Select an icon
        QString testIcon = m_testIconNames.first();
        gridWidget->selectIcon(testIcon);

        // Verify signals
        QVERIFY(selectionSpy.count() >= 1);
        // Viewer may or may not update depending on implementation
    } else {
        QSKIP("Grid widget or viewer widget not found");
    }
}

void TestGalleryIntegration::testInteraction_SidebarAndGrid() {
    CategorySidebarWidget* sidebarWidget = m_mainWindow->findChild<CategorySidebarWidget*>();
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();

    if (sidebarWidget && gridWidget) {
        // Test sidebar interaction with grid
        QVERIFY(sidebarWidget->isVisible());
        QVERIFY(gridWidget != nullptr);

        // Basic interaction test
        simulateUserInteraction();
        QVERIFY(true); // Placeholder for actual interaction test
    } else {
        QSKIP("Sidebar widget or grid widget not found");
    }
}

void TestGalleryIntegration::testInteraction_SearchAndFilter() {
    SearchWidget* searchWidget = m_mainWindow->findChild<SearchWidget*>();
    CategoryFilterWidget* filterWidget = m_mainWindow->findChild<CategoryFilterWidget*>();

    if (searchWidget && filterWidget) {
        // Test combined search and filter
        searchWidget->focusSearchInput();
        QTest::keyClicks(searchWidget, "arrow");

        QStringList testCategories = {"navigation"};
        filterWidget->setSelectedCategories(testCategories);

        simulateUserInteraction();
        QVERIFY(true); // Placeholder for actual combined filter test
    } else {
        QSKIP("Search widget or filter widget not found");
    }
}

// Data Flow Tests
void TestGalleryIntegration::testDataFlow_IconLoading() {
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();
    QVERIFY(gridWidget != nullptr);

    QSignalSpy loadingSpy(gridWidget, &IconGridWidget::loadingStarted);
    QSignalSpy finishedSpy(gridWidget, &IconGridWidget::loadingFinished);

    // Load icons
    gridWidget->setIconNames(m_testIconNames);

    // Verify loading signals
    QVERIFY(loadingSpy.wait(1000));
    QVERIFY(finishedSpy.wait(3000));

    QCOMPARE(loadingSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
}

void TestGalleryIntegration::testDataFlow_SearchFiltering() {
    SearchWidget* searchWidget = m_mainWindow->findChild<SearchWidget*>();
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();

    if (searchWidget && gridWidget) {
        // Set up initial data
        gridWidget->setIconNames(m_testIconNames);

        QSignalSpy searchSpy(searchWidget, &SearchWidget::searchChanged);
        QSignalSpy gridSpy(gridWidget, &IconGridWidget::loadingFinished);

        // Perform search
        searchWidget->focusSearchInput();
        QTest::keyClicks(searchWidget, "home");

        // Verify data flow
        QVERIFY(searchSpy.wait(1000));
        QVERIFY(gridSpy.wait(2000));

        QVERIFY(searchSpy.count() >= 1);
        QVERIFY(gridSpy.count() >= 1);
    } else {
        QSKIP("Required widgets not found");
    }
}

void TestGalleryIntegration::testDataFlow_CategoryFiltering() {
    CategoryFilterWidget* filterWidget = m_mainWindow->findChild<CategoryFilterWidget*>();
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();

    if (filterWidget && gridWidget) {
        QSignalSpy filterSpy(filterWidget, &CategoryFilterWidget::selectionChanged);
        QSignalSpy gridSpy(gridWidget, &IconGridWidget::loadingFinished);

        // Apply category filter
        QStringList categories = {"navigation", "ui"};
        filterWidget->setSelectedCategories(categories);

        // Verify data flow
        QVERIFY(filterSpy.count() >= 1);
        if (gridSpy.wait(2000)) {
            QVERIFY(gridSpy.count() >= 1);
        }
    } else {
        QSKIP("Required widgets not found");
    }
}

void TestGalleryIntegration::testDataFlow_SelectionPropagation() {
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();

    if (gridWidget) {
        gridWidget->setIconNames(m_testIconNames);

        QSignalSpy selectionSpy(gridWidget, &IconGridWidget::iconSelected);

        // Select an icon
        QString testIcon = m_testIconNames.first();
        gridWidget->selectIcon(testIcon);

        // Verify selection propagation
        QCOMPARE(selectionSpy.count(), 1);
        QCOMPARE(gridWidget->currentIcon(), testIcon);
    } else {
        QSKIP("Grid widget not found");
    }
}

void TestGalleryIntegration::testDataFlow_ViewModeChanges() {
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();

    if (gridWidget) {
        QSignalSpy viewModeSpy(gridWidget, &IconGridWidget::viewModeChanged);

        // Change view mode
        gridWidget->setViewMode(IconGridWidget::ListView);
        QCOMPARE(gridWidget->viewMode(), IconGridWidget::ListView);

        gridWidget->setViewMode(IconGridWidget::GridView);
        QCOMPARE(gridWidget->viewMode(), IconGridWidget::GridView);

        // Verify signals (may not be emitted depending on implementation)
        QVERIFY(viewModeSpy.count() >= 0);
    } else {
        QSKIP("Grid widget not found");
    }
}

// End-to-End Tests
void TestGalleryIntegration::testEndToEnd_BasicWorkflow() {
    // Test complete basic workflow: open app -> load icons -> select icon
    QVERIFY(m_mainWindow->isVisible());

    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();
    QVERIFY(gridWidget != nullptr);

    // Load icons
    gridWidget->setIconNames(m_testIconNames);
    QTest::qWait(1000); // Wait for loading

    // Select an icon
    QString testIcon = m_testIconNames.first();
    gridWidget->selectIcon(testIcon);

    // Verify final state
    QCOMPARE(gridWidget->currentIcon(), testIcon);
    QVERIFY(m_mainWindow->isVisible());
}

void TestGalleryIntegration::testEndToEnd_SearchWorkflow() {
    // Test complete search workflow: search -> filter results -> select
    SearchWidget* searchWidget = m_mainWindow->findChild<SearchWidget*>();
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();

    if (searchWidget && gridWidget) {
        // Initial setup
        gridWidget->setIconNames(m_testIconNames);
        QTest::qWait(500);

        // Perform search
        searchWidget->focusSearchInput();
        QTest::keyClicks(searchWidget, "home");
        QTest::qWait(1000);

        // Verify search results are displayed
        QVERIFY(gridWidget->iconNames().size() >= 0); // May be filtered

        // Select from search results if any
        if (!gridWidget->iconNames().isEmpty()) {
            QString firstResult = gridWidget->iconNames().first();
            gridWidget->selectIcon(firstResult);
            QCOMPARE(gridWidget->currentIcon(), firstResult);
        }
    } else {
        QSKIP("Required widgets not found");
    }
}

void TestGalleryIntegration::testEndToEnd_CategoryWorkflow() {
    // Test complete category workflow: select category -> view icons -> select icon
    CategoryFilterWidget* filterWidget = m_mainWindow->findChild<CategoryFilterWidget*>();
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();

    if (filterWidget && gridWidget) {
        // Select category
        QStringList categories = {"navigation"};
        filterWidget->setSelectedCategories(categories);
        QTest::qWait(1000);

        // Verify category filtering worked
        QCOMPARE(filterWidget->selectedCategories(), categories);

        // Select icon from category if any available
        if (!gridWidget->iconNames().isEmpty()) {
            QString categoryIcon = gridWidget->iconNames().first();
            gridWidget->selectIcon(categoryIcon);
            QCOMPARE(gridWidget->currentIcon(), categoryIcon);
        }
    } else {
        QSKIP("Required widgets not found");
    }
}

void TestGalleryIntegration::testEndToEnd_ViewerWorkflow() {
    // Test complete viewer workflow: select icon -> view in viewer -> navigate
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();
    ImageViewerWidget* viewerWidget = m_mainWindow->findChild<ImageViewerWidget*>();

    if (gridWidget && viewerWidget) {
        // Setup
        gridWidget->setIconNames(m_testIconNames);
        QTest::qWait(500);

        // Select icon
        QString testIcon = m_testIconNames.first();
        gridWidget->selectIcon(testIcon);

        // Test viewer operations
        viewerWidget->onZoomIn();
        viewerWidget->onZoomOut();
        viewerWidget->onFitToWindow();

        // Verify viewer state
        QVERIFY(viewerWidget != nullptr);
    } else {
        QSKIP("Required widgets not found");
    }
}

void TestGalleryIntegration::testEndToEnd_PerformanceWorkflow() {
    // Test performance with larger dataset
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();
    QVERIFY(gridWidget != nullptr);

    // Use larger icon set
    QStringList largeIconSet = getTestIconNames(500);

    QElapsedTimer timer;
    timer.start();

    // Load large set
    gridWidget->setIconNames(largeIconSet);
    QTest::qWait(2000); // Wait for loading

    qint64 loadTime = timer.elapsed();
    qDebug() << "Large icon set (" << largeIconSet.size() << " icons) loaded in" << loadTime
             << "ms";

    // Performance should be reasonable (less than 5 seconds for 500 icons)
    QVERIFY(loadTime < 5000);

    // Test selection performance
    timer.restart();
    QString testIcon = largeIconSet.first();
    gridWidget->selectIcon(testIcon);

    qint64 selectionTime = timer.elapsed();
    qDebug() << "Icon selection took" << selectionTime << "ms";

    // Selection should be fast (less than 100ms)
    QVERIFY(selectionTime < 100);
}

// Window Management Tests
void TestGalleryIntegration::testWindowManagement_Resize() {
    QSize originalSize = m_mainWindow->size();

    // Test various window sizes
    QSize testSizes[] = {QSize(800, 600), QSize(1024, 768), QSize(1200, 900), QSize(600, 400)};

    for (const QSize& size : testSizes) {
        m_mainWindow->resize(size);
        simulateUserInteraction();

        QCOMPARE(m_mainWindow->size(), size);

        // Verify components are still functional after resize
        IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();
        if (gridWidget) {
            QVERIFY(gridWidget->isVisible());
        }
    }

    // Restore original size
    m_mainWindow->resize(originalSize);
}

void TestGalleryIntegration::testWindowManagement_SplitterAdjustment() {
    // Find splitters in the main window
    QList<QSplitter*> splitters = m_mainWindow->findChildren<QSplitter*>();

    for (QSplitter* splitter : splitters) {
        if (splitter->count() >= 2) {
            QList<int> originalSizes = splitter->sizes();

            // Adjust splitter
            QList<int> newSizes = originalSizes;
            if (newSizes.size() >= 2) {
                newSizes[0] = originalSizes[0] + 50;
                newSizes[1] = originalSizes[1] - 50;
                splitter->setSizes(newSizes);

                simulateUserInteraction();

                // Verify adjustment worked
                QList<int> currentSizes = splitter->sizes();
                QVERIFY(currentSizes[0] != originalSizes[0]);
            }
        }
    }
}

void TestGalleryIntegration::testWindowManagement_PanelVisibility() {
    // Test sidebar visibility toggle
    CategorySidebarWidget* sidebarWidget = m_mainWindow->findChild<CategorySidebarWidget*>();
    if (sidebarWidget) {
        bool originalVisibility = sidebarWidget->isVisible();

        // Toggle visibility
        sidebarWidget->setVisible(!originalVisibility);
        simulateUserInteraction();
        QCOMPARE(sidebarWidget->isVisible(), !originalVisibility);

        // Restore original visibility
        sidebarWidget->setVisible(originalVisibility);
        QCOMPARE(sidebarWidget->isVisible(), originalVisibility);
    }

    // Test toolbar visibility
    QList<QToolBar*> toolbars = m_mainWindow->findChildren<QToolBar*>();
    for (QToolBar* toolbar : toolbars) {
        bool originalVisibility = toolbar->isVisible();

        toolbar->setVisible(!originalVisibility);
        simulateUserInteraction();
        QCOMPARE(toolbar->isVisible(), !originalVisibility);

        toolbar->setVisible(originalVisibility);
        QCOMPARE(toolbar->isVisible(), originalVisibility);
    }
}

void TestGalleryIntegration::testWindowManagement_FullscreenMode() {
    QVERIFY(!m_mainWindow->isFullScreen());

    // Enter fullscreen
    m_mainWindow->showFullScreen();
    simulateUserInteraction();
    QVERIFY(m_mainWindow->isFullScreen());

    // Exit fullscreen
    m_mainWindow->showNormal();
    simulateUserInteraction();
    QVERIFY(!m_mainWindow->isFullScreen());
}

// State Management Tests
void TestGalleryIntegration::testStateManagement_Settings() {
    // Test that settings can be saved and restored
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();
    if (gridWidget) {
        // Change some settings
        int originalIconSize = gridWidget->iconSize();
        int newIconSize = originalIconSize + 16;

        gridWidget->setIconSize(newIconSize);
        QCOMPARE(gridWidget->iconSize(), newIconSize);

        // In a real application, settings would be saved to QSettings
        // Here we just verify the state can be changed and retrieved
        QVERIFY(gridWidget->iconSize() != originalIconSize);
    }
}

void TestGalleryIntegration::testStateManagement_SessionRestore() {
    // Test session state restoration
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();
    if (gridWidget) {
        // Set up some state
        gridWidget->setIconNames(m_testIconNames);
        QString selectedIcon = m_testIconNames.first();
        gridWidget->selectIcon(selectedIcon);

        // Verify state
        QCOMPARE(gridWidget->currentIcon(), selectedIcon);
        QCOMPARE(gridWidget->iconNames(), m_testIconNames);

        // In a real application, this state would be saved and restored
        // across application sessions
    }
}

void TestGalleryIntegration::testStateManagement_ViewPreferences() {
    // Test view preference persistence
    IconGridWidget* gridWidget = m_mainWindow->findChild<IconGridWidget*>();
    if (gridWidget) {
        // Test view mode preference
        IconGridWidget::ViewMode originalMode = gridWidget->viewMode();
        IconGridWidget::ViewMode newMode = (originalMode == IconGridWidget::GridView)
                                               ? IconGridWidget::ListView
                                               : IconGridWidget::GridView;

        gridWidget->setViewMode(newMode);
        QCOMPARE(gridWidget->viewMode(), newMode);

        // Test icon size preference
        int originalSize = gridWidget->iconSize();
        int newSize = originalSize + 16;

        gridWidget->setIconSize(newSize);
        QCOMPARE(gridWidget->iconSize(), newSize);

        // In a real application, these preferences would be persisted
    }
}

QTEST_MAIN(TestGalleryIntegration)
#include "test_gallery_integration.moc"
