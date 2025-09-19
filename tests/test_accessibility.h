/**
 * QtLucide Accessibility Tests - Header
 *
 * Comprehensive tests to ensure the application is accessible including
 * keyboard navigation, screen reader compatibility, and high contrast support.
 */

#ifndef TEST_ACCESSIBILITY_H
#define TEST_ACCESSIBILITY_H

#include <QApplication>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QObject>
#include <QPalette>
#include <QStyleFactory>
#include <QTimer>
#include <QWidget>
#include <QtTest/QtTest>

// Include accessibility headers (safe now since we're included before Windows headers)
#include <QAccessible>
#include <QAccessibleInterface>

namespace lucide {
class QtLucide;
}

class GalleryMainWindow;
class SearchWidget;
class CategoryFilterWidget;
class IconGridWidget;
class IconDetailsPanel;
class PreferencesDialog;
class IconMetadataManager;

/**
 * @brief Accessibility test configuration
 */
struct AccessibilityConfig {
    bool keyboardNavigationEnabled = true;
    bool screenReaderSupported = true;
    bool highContrastSupported = true;
    bool focusIndicatorsVisible = true;
    bool accessibleNamesProvided = true;
    bool accessibleDescriptionsProvided = true;
    int minimumFontSize = 9;
    int minimumContrastRatio = 3; // WCAG AA standard
};

/**
 * @brief Keyboard navigation test result
 */
struct KeyboardNavigationResult {
    bool canNavigateToElement = false;
    bool canActivateElement = false;
    bool focusIndicatorVisible = false;
    bool tabOrderCorrect = false;
    QString elementName;
    QString accessibleName;
    QString accessibleDescription;
};

/**
 * @brief Screen reader compatibility result
 */
struct ScreenReaderResult {
    bool hasAccessibleInterface = false;
    bool hasAccessibleName = false;
    bool hasAccessibleDescription = false;
    bool hasAccessibleRole = false;
    bool hasAccessibleState = false;
    QString accessibleName;
    QString accessibleDescription;
    QAccessible::Role accessibleRole = QAccessible::NoRole;
    QAccessible::State accessibleState;
};

class TestAccessibility : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Keyboard navigation tests
    void testMainWindowKeyboardNavigation();
    void testSearchWidgetKeyboardNavigation();
    void testCategoryFilterKeyboardNavigation();
    void testIconGridKeyboardNavigation();
    void testDetailsPanelKeyboardNavigation();
    void testPreferencesDialogKeyboardNavigation();
    void testMenuBarKeyboardNavigation();
    void testToolbarKeyboardNavigation();

    // Tab order tests
    void testTabOrderConsistency();
    void testTabOrderLogicalFlow();
    void testTabOrderSkipsDisabledElements();
    void testTabOrderIncludesAllInteractiveElements();

    // Focus management tests
    void testFocusIndicatorVisibility();
    void testFocusRetention();
    void testFocusRestoration();
    void testInitialFocusPlacement();
    void testModalDialogFocusTrapping();

    // Screen reader compatibility tests
    void testAccessibleInterfaceAvailability();
    void testAccessibleNamesAndDescriptions();
    void testAccessibleRolesAndStates();
    void testAccessibleHierarchy();
    void testAccessibleActions();
    void testAccessibleValueInterfaces();

    // High contrast mode tests
    void testHighContrastModeSupport();
    void testHighContrastColorScheme();
    void testHighContrastIconVisibility();
    void testHighContrastTextReadability();
    void testHighContrastFocusIndicators();

    // Color accessibility tests
    void testColorContrastRatios();
    void testColorBlindnessSupport();
    void testColorIndependentInformation();

    // Font and text accessibility tests
    void testMinimumFontSizes();
    void testFontScaling();
    void testTextReadability();
    void testTextAlternatives();

    // ARIA and semantic tests
    void testAriaLabels();
    void testAriaDescriptions();
    void testAriaRoles();
    void testAriaStates();
    void testLandmarkRoles();

    // Assistive technology integration tests
    void testScreenReaderAnnouncements();
    void testMagnifierCompatibility();
    void testVoiceControlCompatibility();

private:
    // Keyboard navigation helpers
    KeyboardNavigationResult testElementKeyboardNavigation(QWidget* element);
    bool canNavigateToElement(QWidget* element);
    bool canActivateElement(QWidget* element);
    bool isFocusIndicatorVisible(QWidget* element);
    QList<QWidget*> getTabOrder(QWidget* parent);

    // Screen reader helpers
    ScreenReaderResult testElementScreenReaderSupport(QWidget* element);
    QAccessibleInterface* getAccessibleInterface(QWidget* element);
    bool hasValidAccessibleName(QAccessibleInterface* interface);
    bool hasValidAccessibleDescription(QAccessibleInterface* interface);

    // High contrast helpers
    void enableHighContrastMode();
    void disableHighContrastMode();
    bool isHighContrastModeActive();
    double calculateContrastRatio(const QColor& foreground, const QColor& background);
    bool meetsContrastRequirements(double ratio, int level = 2); // 1=AA, 2=AAA

    // Color accessibility helpers
    bool isColorBlindnessFriendly(const QColor& color1, const QColor& color2);
    bool hasNonColorIndicators(QWidget* element);

    // Font and text helpers
    bool meetsFontSizeRequirements(QWidget* element);
    bool isTextReadable(QWidget* element);
    QString getTextAlternative(QWidget* element);

    // ARIA helpers
    QString getAriaLabel(QWidget* element);
    QString getAriaDescription(QWidget* element);
    QString getAriaRole(QWidget* element);
    QStringList getAriaStates(QWidget* element);

    // Test simulation helpers
    void simulateKeyboardNavigation(QWidget* startElement, const QList<Qt::Key>& keys);
    void simulateScreenReaderInteraction(QWidget* element);
    void simulateHighContrastEnvironment();

    // Validation helpers
    bool validateAccessibilityCompliance(QWidget* element);
    QStringList getAccessibilityViolations(QWidget* element);

    // Test data and objects
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    GalleryMainWindow* m_mainWindow;
    SearchWidget* m_searchWidget;
    CategoryFilterWidget* m_categoryFilter;
    IconGridWidget* m_iconGrid;
    IconDetailsPanel* m_detailsPanel;
    PreferencesDialog* m_preferencesDialog;

    // Accessibility configuration
    AccessibilityConfig m_accessibilityConfig;

    // Original system settings (for restoration)
    QPalette m_originalPalette;
    QString m_originalStyle;

    // Test tracking
    QList<KeyboardNavigationResult> m_navigationResults;
    QList<ScreenReaderResult> m_screenReaderResults;
    QStringList m_accessibilityViolations;

    // Accessibility standards compliance
    static constexpr double WCAG_AA_CONTRAST_RATIO = 4.5;
    static constexpr double WCAG_AAA_CONTRAST_RATIO = 7.0;
    static constexpr int MINIMUM_FONT_SIZE_PT = 9;
    static constexpr int MINIMUM_TOUCH_TARGET_SIZE = 44; // pixels
};

#endif // TEST_ACCESSIBILITY_H
