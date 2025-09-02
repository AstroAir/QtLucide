/**
 * QtLucide Gallery Application - Icon Details Panel Header
 * 
 * Displays comprehensive information about selected icons including
 * metadata, preview options, usage examples, and copy functionality.
 */

#ifndef ICONDETAILSPANEL_H
#define ICONDETAILSPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QScrollArea>
#include <QFrame>
#include <QGroupBox>
#include <QTabWidget>
#include <QSlider>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QListWidget>
#include <QTableWidget>
#include <QSplitter>
#include <QProgressBar>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QSyntaxHighlighter>
#include <QTextDocument>

#include <QtLucide/QtLucide.h>
#include "IconMetadataManager.h"

/**
 * @brief Syntax highlighter for code examples
 */
class CodeSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    enum Language {
        Cpp,
        JavaScript,
        Html,
        Css
    };

    explicit CodeSyntaxHighlighter(Language language, QTextDocument *parent = nullptr);
    void setLanguage(Language language);

protected:
    void highlightBlock(const QString &text) override;

private:
    void setupCppRules();
    void setupJavaScriptRules();
    void setupHtmlRules();
    void setupCssRules();

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> m_highlightingRules;
    Language m_language;
};

/**
 * @brief Widget for displaying icon preview with size and color controls
 */
class IconPreviewWidget : public QFrame
{
    Q_OBJECT

public:
    explicit IconPreviewWidget(lucide::QtLucide* lucide, QWidget *parent = nullptr);
    
    void setIconName(const QString& iconName);
    QString iconName() const { return m_iconName; }
    
    void setIconSize(int size);
    int iconSize() const { return m_iconSize; }
    
    void setIconColor(const QColor& color);
    QColor iconColor() const { return m_iconColor; }

public slots:
    void refreshPreview();
    void resetToDefaults();

signals:
    void iconSizeChanged(int size);
    void iconColorChanged(const QColor& color);
    void copyRequested(const QString& format);

private slots:
    void onSizeSliderChanged(int value);
    void onSizeSpinBoxChanged(int value);
    void onColorButtonClicked();
    void onCopyIconClicked();
    void onCopyCodeClicked();
    void onCopySvgClicked();

private:
    void setupUI();
    void setupControls();
    void updatePreview();
    void updateSizeControls();
    
    lucide::QtLucide* m_lucide;
    
    // UI components
    QVBoxLayout* m_layout;
    QHBoxLayout* m_previewLayout;
    QHBoxLayout* m_controlsLayout;
    
    QLabel* m_iconLabel;
    QFrame* m_previewFrame;
    
    QSlider* m_sizeSlider;
    QSpinBox* m_sizeSpinBox;
    QPushButton* m_colorButton;
    QToolButton* m_copyIconButton;
    QToolButton* m_copyCodeButton;
    QToolButton* m_copySvgButton;
    QToolButton* m_resetButton;
    
    // State
    QString m_iconName;
    int m_iconSize;
    QColor m_iconColor;
    
    static constexpr int MIN_PREVIEW_SIZE = 16;
    static constexpr int MAX_PREVIEW_SIZE = 256;
    static constexpr int DEFAULT_PREVIEW_SIZE = 64;
};

/**
 * @brief Widget for displaying code examples and usage instructions
 */
class CodeExampleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CodeExampleWidget(QWidget *parent = nullptr);
    
    void setIconName(const QString& iconName);
    void updateExamples();

signals:
    void copyCodeRequested(const QString& code, const QString& language);

private slots:
    void onLanguageChanged(int index);
    void onCopyCodeClicked();

private:
    void setupUI();
    void generateCppExample();
    void generateJavaScriptExample();
    void generateHtmlExample();
    void generateCssExample();
    void updateSyntaxHighlighting();
    
    QString m_iconName;
    
    // UI components
    QVBoxLayout* m_layout;
    QHBoxLayout* m_headerLayout;
    
    QComboBox* m_languageCombo;
    QToolButton* m_copyButton;
    QPlainTextEdit* m_codeEdit;
    
    // Syntax highlighting
    CodeSyntaxHighlighter* m_highlighter;
    
    // Code templates
    QHash<QString, QString> m_codeTemplates;
};

/**
 * @brief Main details panel widget
 */
class IconDetailsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit IconDetailsPanel(lucide::QtLucide* lucide,
                             IconMetadataManager* metadataManager,
                             QWidget *parent = nullptr);
    ~IconDetailsPanel();
    
    void setIconName(const QString& iconName);
    QString currentIconName() const { return m_currentIconName; }
    
    bool isEmpty() const { return m_currentIconName.isEmpty(); }
    void clear();

public slots:
    void refreshDetails();
    void updateFavoriteStatus();

signals:
    void favoriteToggled(const QString& iconName, bool favorite);
    void copyRequested(const QString& iconName, const QString& format, const QString& data);
    void iconUsed(const QString& iconName);

private slots:
    void onFavoriteButtonClicked();
    void onCopyNameClicked();
    void onCopyCodeClicked();
    void onCopySvgClicked();
    void onPreviewSizeChanged(int size);
    void onPreviewColorChanged(const QColor& color);
    void onCodeCopyRequested(const QString& code, const QString& language);
    void onTabChanged(int index);

private:
    void setupUI();
    void setupHeaderSection();
    void setupTabWidget();
    void setupMetadataTab();
    void setupPreviewTab();
    void setupCodeTab();
    void setupUsageTab();
    
    void updateHeader();
    void updateMetadata();
    void updatePreview();
    void updateCodeExamples();
    void updateUsageInfo();
    void updateFavoriteButton();
    
    void populateMetadataTable();
    void populateTagsList();
    void populateCategoriesList();
    
    QString formatIconName(const QString& name) const;
    QString generateIconCode(const QString& format) const;
    
    // Core components
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    
    // UI components
    QVBoxLayout* m_layout;
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
    
    // Header section
    QFrame* m_headerFrame;
    QHBoxLayout* m_headerLayout;
    QLabel* m_iconLabel;
    QLabel* m_nameLabel;
    QToolButton* m_favoriteButton;
    QToolButton* m_copyNameButton;
    QToolButton* m_copyCodeButton;
    QToolButton* m_copySvgButton;
    
    // Tab widget
    QTabWidget* m_tabWidget;
    
    // Metadata tab
    QWidget* m_metadataTab;
    QTableWidget* m_metadataTable;
    QListWidget* m_tagsList;
    QListWidget* m_categoriesList;
    
    // Preview tab
    QWidget* m_previewTab;
    IconPreviewWidget* m_previewWidget;
    
    // Code tab
    QWidget* m_codeTab;
    CodeExampleWidget* m_codeWidget;
    
    // Usage tab
    QWidget* m_usageTab;
    QLabel* m_usageCountLabel;
    QLabel* m_lastUsedLabel;
    QProgressBar* m_popularityBar;
    
    // State
    QString m_currentIconName;
    IconMetadata m_currentMetadata;
    
    // Animation
    QPropertyAnimation* m_fadeAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;
    QTimer* m_updateTimer;
    
    static constexpr int UPDATE_DELAY = 100;
};

#endif // ICONDETAILSPANEL_H
