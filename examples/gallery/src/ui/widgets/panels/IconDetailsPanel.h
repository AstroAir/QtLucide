/**
 * QtLucide Gallery Application - Enhanced Icon Details and Preview Panel
 *
 * A comprehensive, feature-rich panel for displaying detailed icon information with:
 * - High-quality multi-resolution preview with zoom and pan
 * - Complete metadata display with editing capabilities
 * - Advanced code generation for multiple frameworks
 * - Export functionality with various formats (SVG, PNG, ICO, etc.)
 * - Usage examples and documentation
 * - Performance analytics and optimization suggestions
 * - Accessibility information and testing tools
 * - Version history and change tracking
 * - Integration with external design tools
 * - Collaborative features for team workflows
 */

#ifndef ICONDETAILSPANEL_H
#define ICONDETAILSPANEL_H

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QFontDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMimeData>
#include <QPixmapCache>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedLayout>
#include <QSyntaxHighlighter>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextDocument>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "IconMetadataManager.h"
#include <QtLucide/QtLucide.h>
#include <memory>

// Forward declarations
class IconPreviewWidget;
class IconCodeGenerator;
class IconExportWidget;
class IconMetadataEditor;
class IconUsageAnalyzer;

/**
 * @brief Enhanced syntax highlighter for multiple programming languages
 */
class CodeSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    enum Language {
        JavaScript = 0,
        TypeScript = 1,
        React = 2,
        Vue = 3,
        Angular = 4,
        HTML = 5,
        CSS = 6,
        SCSS = 7,
        Python = 8,
        CPlusPlus = 9,
        CSharp = 10,
        Java = 11,
        Swift = 12,
        Kotlin = 13,
        Dart = 14,
        XML = 15,
        JSON = 16
    };
    Q_ENUM(Language)

    explicit CodeSyntaxHighlighter(QTextDocument* parent = nullptr);
    explicit CodeSyntaxHighlighter(Language language, QTextDocument* parent = nullptr);

    void setLanguage(Language language);
    void setTheme(const QString& themeName);
    void setCustomColors(const QHash<QString, QColor>& colors);

protected:
    void highlightBlock(const QString& text) override;

private:
    void setupRules();
    void setupJavaScriptRules();
    void setupCppRules();
    void setupHtmlRules();
    void setupCssRules();
    void setupPythonRules();

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> m_highlightingRules;
    Language m_language;
    QString m_theme;
    QHash<QString, QColor> m_customColors;
};

/**
 * @brief Enhanced widget for high-quality icon preview with advanced controls
 */
class IconPreviewWidget : public QFrame {
    Q_OBJECT

public:
    enum PreviewMode {
        SingleIcon = 0,
        MultipleResolutions = 1,
        ContextualPreview = 2,
        AnimatedPreview = 3
    };
    Q_ENUM(PreviewMode)

    enum BackgroundType {
        Transparent = 0,
        White = 1,
        Black = 2,
        Gray = 3,
        Checkered = 4,
        Custom = 5
    };
    Q_ENUM(BackgroundType)

    explicit IconPreviewWidget(lucide::QtLucide* lucide, QWidget* parent = nullptr);
    ~IconPreviewWidget();

    // Icon management
    void setIconName(const QString& iconName);
    QString iconName() const { return m_iconName; }
    void clearIcon();

    // Size and scaling
    void setIconSize(int size);
    int iconSize() const { return m_iconSize; }
    void setZoomLevel(double zoom);
    double zoomLevel() const { return m_zoomLevel; }
    void setAutoFitEnabled(bool enabled);

    // Appearance
    void setIconColor(const QColor& color);
    QColor iconColor() const { return m_iconColor; }
    void setBackgroundType(BackgroundType type);
    void setCustomBackground(const QColor& color);
    void setPreviewMode(PreviewMode mode);

    // Advanced features
    void setShowGrid(bool show);
    void setShowRuler(bool show);
    void setShowCenterLines(bool show);
    void setHighQualityRendering(bool enabled);

    // Export and clipboard
    QPixmap getPreviewPixmap(int size = -1) const;
    void copyToClipboard();
    void saveAsImage(const QString& filePath);

    // Animation
    void setAnimationEnabled(bool enabled);
    void startRotationAnimation();
    void stopRotationAnimation();

public slots:
    void refreshPreview();
    void resetToDefaults();
    void zoomIn();
    void zoomOut();
    void zoomToFit();
    void resetZoom();

signals:
    void iconSizeChanged(int size);
    void iconColorChanged(const QColor& color);
    void zoomLevelChanged(double zoom);
    void previewModeChanged(PreviewMode mode);
    void copyRequested(const QString& format);
    void exportRequested(const QString& format);

private slots:
    void onSizeSliderChanged(int value);
    void onSizeSpinBoxChanged(int value);
    void onZoomSliderChanged(int value);
    void onColorButtonClicked();
    void onBackgroundChanged();
    void onPreviewModeChanged();
    void onCopyIconClicked();
    void onCopyCodeClicked();
    void onCopySvgClicked();
    void onExportClicked();
    void onAnimationTimer();

private:
    // Enhanced UI setup methods
    void setupUI();
    void setupPreviewArea();
    void setupControls();
    void setupToolbar();
    void setupAnimations();

    // Preview management
    void updatePreview();
    void updateSizeControls();
    void updateZoomControls();
    void updateBackground();
    void renderIcon();
    void renderMultiResolution();
    void renderContextual();

    // Utility methods
    QPixmap createIconPixmap(int size, const QColor& color) const;
    void drawGrid(QPainter& painter, const QRect& rect);
    void drawRuler(QPainter& painter, const QRect& rect);
    void drawCenterLines(QPainter& painter, const QRect& rect);

    lucide::QtLucide* m_lucide;

    // Enhanced UI components
    QVBoxLayout* m_layout; // Used by implementation
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_previewLayout;
    QHBoxLayout* m_controlsLayout;
    QGridLayout* m_toolbarLayout;

    // Preview area
    QGraphicsView* m_previewView;
    QGraphicsScene* m_previewScene;
    QGraphicsPixmapItem* m_iconItem;
    QFrame* m_previewFrame;
    QLabel* m_iconLabel;

    // Enhanced controls
    QSlider* m_sizeSlider;
    QSpinBox* m_sizeSpinBox;
    QSlider* m_zoomSlider;
    QDoubleSpinBox* m_zoomSpinBox;
    QPushButton* m_colorButton;
    QComboBox* m_backgroundCombo;
    QComboBox* m_previewModeCombo;

    // Toolbar buttons
    QToolButton* m_zoomInButton;
    QToolButton* m_zoomOutButton;
    QToolButton* m_zoomFitButton;
    QToolButton* m_copyIconButton;
    QToolButton* m_copyCodeButton;
    QToolButton* m_copySvgButton;
    QToolButton* m_exportButton;
    QToolButton* m_resetButton;

    // Options
    QCheckBox* m_showGridCheck;
    QCheckBox* m_showRulerCheck;
    QCheckBox* m_showCenterLinesCheck;
    QCheckBox* m_highQualityCheck;
    QCheckBox* m_animationCheck;

    // Enhanced state
    QString m_iconName;
    int m_iconSize;
    double m_zoomLevel;
    QColor m_iconColor;
    QColor m_customBackground;
    PreviewMode m_previewMode;
    BackgroundType m_backgroundType;
    bool m_autoFitEnabled;
    bool m_showGrid;
    bool m_showRuler;
    bool m_showCenterLines;
    bool m_highQualityRendering;
    bool m_animationEnabled;

    // Animation
    QPropertyAnimation* m_rotationAnimation;
    QTimer* m_animationTimer;
    qreal m_rotationAngle;

    // Performance
    QPixmapCache* m_pixmapCache;
    QElapsedTimer m_renderTimer;

    // Enhanced constants
    static constexpr int MIN_PREVIEW_SIZE = 8;
    static constexpr int MAX_PREVIEW_SIZE = 512;
    static constexpr int DEFAULT_PREVIEW_SIZE = 64;
    static constexpr double MIN_ZOOM = 0.1;
    static constexpr double MAX_ZOOM = 10.0;
    static constexpr double DEFAULT_ZOOM = 1.0;
    static constexpr int ANIMATION_DURATION = 1000;
};

/**
 * @brief Enhanced widget for displaying comprehensive code examples and usage instructions
 */
class CodeExampleWidget : public QWidget {
    Q_OBJECT

public:
    explicit CodeExampleWidget(QWidget* parent = nullptr);
    ~CodeExampleWidget();

    void setIconName(const QString& iconName);
    void setIconSize(int size);
    void setIconColor(const QColor& color);
    void updateExamples();

    // Language and framework support
    void addLanguage(const QString& name, CodeSyntaxHighlighter::Language language);
    void setCurrentLanguage(const QString& name);
    QString currentLanguage() const;

    // Template management
    void setCustomTemplate(const QString& language, const QString& templateCode);
    void resetToDefaultTemplates();

    // Export functionality
    QString getCurrentCode() const;
    void exportCode(const QString& filePath) const;

signals:
    void copyCodeRequested(const QString& code, const QString& language);
    void exportCodeRequested(const QString& code, const QString& language, const QString& filePath);
    void languageChanged(const QString& language);

private slots:
    void onLanguageChanged(int index);
    void onCopyCodeClicked();
    void onExportCodeClicked();
    void onTemplateChanged();
    void onCustomizeTemplate();

private:
    void setupUI();
    void setupLanguages();
    void setupTemplates();
    void generateExample(const QString& language);
    void generateReactExample();
    void generateVueExample();
    void generateAngularExample();
    void generateSwiftExample();
    void generateKotlinExample();
    void generatePythonExample();
    void generateCppExample();
    void generateJavaScriptExample();
    void generateTypeScriptExample();
    void generateHtmlExample();
    void generateCssExample();
    void updateSyntaxHighlighting();
    void updatePreview();

    QString m_iconName;
    int m_iconSize;
    QColor m_iconColor;

    // Enhanced UI components
    QVBoxLayout* m_layout; // Used by implementation
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_headerLayout;
    QHBoxLayout* m_toolbarLayout;
    QSplitter* m_splitter;

    QComboBox* m_languageCombo;
    QComboBox* m_frameworkCombo;
    QToolButton* m_copyButton;
    QToolButton* m_exportButton;
    QToolButton* m_customizeButton;
    QToolButton* m_previewButton;
    QPlainTextEdit* m_codeEdit;
    QTextEdit* m_previewEdit;

    // Enhanced syntax highlighting
    CodeSyntaxHighlighter* m_highlighter;
    QHash<QString, CodeSyntaxHighlighter::Language> m_languageMap;

    // Enhanced code templates
    QHash<QString, QString> m_defaultTemplates;
    QHash<QString, QString> m_customTemplates;
    QSettings* m_settings;
};

/**
 * @brief Enhanced main details panel widget with comprehensive icon information
 */
class IconDetailsPanel : public QWidget {
    Q_OBJECT

public:
    enum ViewMode { CompactView = 0, DetailedView = 1, DeveloperView = 2, DesignerView = 3 };
    Q_ENUM(ViewMode)

    explicit IconDetailsPanel(lucide::QtLucide* lucide, IconMetadataManager* metadataManager,
                              QWidget* parent = nullptr);
    ~IconDetailsPanel();

    // Icon management
    void setIconName(const QString& iconName);
    QString currentIconName() const { return m_currentIconName; }
    bool isEmpty() const { return m_currentIconName.isEmpty(); }
    void clear();

    // View configuration
    void setViewMode(ViewMode mode);
    ViewMode viewMode() const { return m_viewMode; }
    void setCompactMode(bool compact);
    void setShowAdvancedOptions(bool show);

    // Panel management
    void setCollapsible(bool collapsible);
    void setAutoRefresh(bool enabled);
    void setPinned(bool pinned);
    void setAnimationsEnabled(bool enabled) { Q_UNUSED(enabled); } // Stub implementation

    // Export and sharing
    void exportIconInfo(const QString& filePath) const;
    void shareIcon() const;

public slots:
    void refreshDetails();
    void updateFavoriteStatus();
    void updateUsageStatistics();
    void toggleViewMode();
    void expandAll();
    void collapseAll();

signals:
    void favoriteToggled(const QString& iconName, bool favorite);
    void copyRequested(const QString& iconName, const QString& format, const QString& data);
    void exportRequested(const QString& iconName, const QString& format);
    void iconUsed(const QString& iconName);
    void viewModeChanged(ViewMode mode);
    void panelResized(const QSize& size);
    void metadataEdited(const QString& iconName, const IconMetadata& metadata);

private slots:
    void onFavoriteButtonClicked();
    void onCopyNameClicked();
    void onCopyCodeClicked();
    void onCopySvgClicked();
    void onExportClicked();
    void onShareClicked();
    void onPreviewSizeChanged(int size);
    void onPreviewColorChanged(const QColor& color);
    void onCodeCopyRequested(const QString& code, const QString& language);
    void onCodeExportRequested(const QString& code, const QString& language,
                               const QString& filePath);
    void onTabChanged(int index);
    void onViewModeChanged();
    void onMetadataEdited();
    void onRefreshTimer();
    void onPinToggled();
    void onExpandCollapseToggled();

private:
    // Enhanced UI setup methods
    void setupUI();
    void setupHeaderSection();
    void setupTabWidget();
    void setupMetadataTab();
    void setupPreviewTab();
    void setupCodeTab();
    void setupUsageTab();
    void setupAnalyticsTab();
    void setupAccessibilityTab();
    void setupToolbar();
    void setupAnimations();

    // Update methods
    void updateHeader();
    void updateMetadata();
    void updatePreview();
    void updateCodeExamples();
    void updateUsageInfo();
    void updateAnalytics();
    void updateAccessibilityInfo();
    void updateFavoriteButton();
    void updateViewMode();

    // Data population methods
    void populateMetadataTable();
    void populateTagsList();
    void populateCategoriesList();
    void populateUsageStatistics();
    void populateAccessibilityInfo();

    // Utility methods
    QString formatIconName(const QString& name) const;
    QString generateIconCode(const QString& format) const;
    QString formatFileSize(qint64 bytes) const;
    QString formatUsageCount(int count) const;
    QIcon createFavoriteIcon(bool favorite) const;

    // Export and sharing
    void exportToFile(const QString& filePath, const QString& format) const;
    void copyToClipboard(const QString& data, const QString& mimeType = "text/plain") const;

    // Core components
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;

    // Enhanced UI components
    QVBoxLayout* m_layout; // Used by implementation
    QVBoxLayout* m_mainLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
    QSplitter* m_splitter;

    // Header section
    QFrame* m_headerFrame;
    QHBoxLayout* m_headerLayout;
    QLabel* m_iconLabel; // Used by implementation for icon display
    QLabel* m_nameLabel; // Used by implementation for icon name
    QLabel* m_iconNameLabel;
    QLabel* m_iconSizeLabel;
    QToolButton* m_favoriteButton;
    QToolButton* m_pinButton;
    QToolButton* m_expandCollapseButton;
    QToolButton* m_viewModeButton;

    // Enhanced tab widget
    QTabWidget* m_tabWidget;
    QWidget* m_metadataTab;
    QWidget* m_previewTab;
    QWidget* m_codeTab;
    QWidget* m_usageTab;
    QWidget* m_analyticsTab;
    QWidget* m_accessibilityTab;

    // Metadata tab components
    QTableWidget* m_metadataTable;
    QListWidget* m_tagsList;
    QListWidget* m_categoriesList;
    IconMetadataEditor* m_metadataEditor;

    // Preview tab components
    IconPreviewWidget* m_previewWidget;

    // Code tab components
    CodeExampleWidget* m_codeWidget;

    // Usage tab components
    QLabel* m_usageCountLabel;
    QLabel* m_lastUsedLabel;
    QProgressBar* m_popularityBar;
    QTreeWidget* m_usageHistoryTree;

    // Analytics tab components
    QLabel* m_fileSizeLabel;
    QLabel* m_renderTimeLabel;
    QLabel* m_memoryUsageLabel;
    QProgressBar* m_performanceBar;

    // Accessibility tab components
    QLabel* m_accessibilityScoreLabel;
    QTextEdit* m_accessibilityReport;
    QListWidget* m_accessibilityIssues;

    // Toolbar
    QFrame* m_toolbar;
    QHBoxLayout* m_toolbarLayout;
    QToolButton* m_copyNameButton;
    QToolButton* m_copyCodeButton;
    QToolButton* m_copySvgButton;
    QToolButton* m_exportButton;
    QToolButton* m_shareButton;
    QToolButton* m_refreshButton;

    // Enhanced state management
    QString m_currentIconName;
    ViewMode m_viewMode;
    bool m_compactMode;
    bool m_showAdvancedOptions;
    bool m_collapsible;
    bool m_autoRefresh;
    bool m_pinned;
    bool m_expanded;

    // Performance and caching
    QTimer* m_refreshTimer;
    QTimer* m_updateTimer; // Changed from QElapsedTimer to match implementation
    QPixmapCache* m_iconCache;

    // Settings
    QSettings* m_settings;

    // Animation
    QPropertyAnimation* m_expandAnimation;
    QPropertyAnimation* m_fadeAnimation;

    // Constants
    static constexpr int REFRESH_INTERVAL = 5000; // 5 seconds
    static constexpr int UPDATE_DELAY = 500;      // 500ms delay for updates
    static constexpr int ANIMATION_DURATION = 300;
    static constexpr const char* SETTINGS_GROUP = "IconDetailsPanel";
};

#endif // ICONDETAILSPANEL_H
