/**
 * QtLucide Gallery Application - Icon Details Panel Implementation
 */

#include "IconDetailsPanel.h"
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTimer>
#include <QVBoxLayout>

// CodeSyntaxHighlighter Implementation
CodeSyntaxHighlighter::CodeSyntaxHighlighter(Language language, QTextDocument* parent)
    : QSyntaxHighlighter(parent), m_language(language) {
    setLanguage(language);
}

void CodeSyntaxHighlighter::setLanguage(Language language) {
    m_language = language;
    m_highlightingRules.clear();

    switch (language) {
        case CPlusPlus:
            setupCppRules();
            break;
        case JavaScript:
            setupJavaScriptRules();
            break;
        case HTML:
            setupHtmlRules();
            break;
        case CSS:
            setupCssRules();
            break;
        case TypeScript:
        case React:
        case Vue:
        case Angular:
        case SCSS:
        case Python:
        case CSharp:
        case Java:
        case Swift:
        case Kotlin:
        case Dart:
        case XML:
        case JSON:
            // TODO: Implement syntax highlighting for these languages
            // For now, use basic highlighting
            setupCppRules();
            break;
    }
}

void CodeSyntaxHighlighter::highlightBlock(const QString& text) {
    foreach (const HighlightingRule& rule, m_highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(static_cast<int>(match.capturedStart()),
                      static_cast<int>(match.capturedLength()), rule.format);
        }
    }
}

void CodeSyntaxHighlighter::setupCppRules() {
    HighlightingRule rule;

    // Keywords
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(86, 156, 214));
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bclass\\b" << "\\bnamespace\\b" << "\\bpublic\\b" << "\\bprivate\\b"
                    << "\\bprotected\\b" << "\\bvirtual\\b" << "\\bstatic\\b" << "\\bconst\\b";

    foreach (const QString& pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }

    // Strings
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(206, 145, 120));
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
}

void CodeSyntaxHighlighter::setupJavaScriptRules() {
    // Similar to C++ but with JavaScript keywords
    setupCppRules();
}

void CodeSyntaxHighlighter::setupHtmlRules() {
    // HTML tag highlighting
    HighlightingRule rule;
    QTextCharFormat htmlElementFormat;
    htmlElementFormat.setForeground(QColor(86, 156, 214));
    rule.pattern = QRegularExpression("<[!?/]?\\b[A-Za-z]+(?:\\s[^>]*)?>");
    rule.format = htmlElementFormat;
    m_highlightingRules.append(rule);
}

void CodeSyntaxHighlighter::setupCssRules() {
    // CSS property highlighting
    HighlightingRule rule;
    QTextCharFormat cssPropertyFormat;
    cssPropertyFormat.setForeground(QColor(156, 220, 254));
    rule.pattern = QRegularExpression("\\b[A-Za-z-]+(?=\\s*:)");
    rule.format = cssPropertyFormat;
    m_highlightingRules.append(rule);
}

// IconPreviewWidget Implementation
IconPreviewWidget::IconPreviewWidget(lucide::QtLucide* lucide, QWidget* parent)
    : QFrame(parent), m_lucide(lucide), m_iconSize(DEFAULT_PREVIEW_SIZE), m_iconColor(Qt::black) {
    setupUI();
}

void IconPreviewWidget::setupUI() {
    setFrameStyle(QFrame::StyledPanel);
    m_layout = new QVBoxLayout(this);

    // Preview area
    m_previewFrame = new QFrame(this);
    m_previewFrame->setFrameStyle(QFrame::Box);
    m_previewFrame->setMinimumHeight(100);

    m_previewLayout = new QHBoxLayout(m_previewFrame);
    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_previewLayout->addWidget(m_iconLabel);

    // Controls
    m_controlsLayout = new QHBoxLayout();

    m_sizeSlider = new QSlider(Qt::Horizontal, this);
    m_sizeSlider->setRange(MIN_PREVIEW_SIZE, MAX_PREVIEW_SIZE);
    m_sizeSlider->setValue(m_iconSize);

    m_sizeSpinBox = new QSpinBox(this);
    m_sizeSpinBox->setRange(MIN_PREVIEW_SIZE, MAX_PREVIEW_SIZE);
    m_sizeSpinBox->setValue(m_iconSize);

    m_colorButton = new QPushButton("Color", this);
    m_copyIconButton = new QToolButton(this);
    m_copyIconButton->setText("Copy");

    m_controlsLayout->addWidget(new QLabel("Size:", this));
    m_controlsLayout->addWidget(m_sizeSlider, 1);
    m_controlsLayout->addWidget(m_sizeSpinBox);
    m_controlsLayout->addWidget(m_colorButton);
    m_controlsLayout->addWidget(m_copyIconButton);

    m_layout->addWidget(m_previewFrame, 1);
    m_layout->addLayout(m_controlsLayout);

    // Connect signals
    connect(m_sizeSlider, &QSlider::valueChanged, this, &IconPreviewWidget::onSizeSliderChanged);
    connect(m_sizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &IconPreviewWidget::onSizeSpinBoxChanged);
    connect(m_colorButton, &QPushButton::clicked, this, &IconPreviewWidget::onColorButtonClicked);
    connect(m_copyIconButton, &QToolButton::clicked, this, &IconPreviewWidget::onCopyIconClicked);
}

void IconPreviewWidget::setIconName(const QString& iconName) {
    m_iconName = iconName;
    updatePreview();
}

void IconPreviewWidget::setIconSize(int size) {
    size = qBound(MIN_PREVIEW_SIZE, size, MAX_PREVIEW_SIZE);
    if (m_iconSize != size) {
        m_iconSize = size;
        updateSizeControls();
        updatePreview();
        emit iconSizeChanged(size);
    }
}

void IconPreviewWidget::setIconColor(const QColor& color) {
    if (m_iconColor != color) {
        m_iconColor = color;
        updatePreview();
        emit iconColorChanged(color);
    }
}

void IconPreviewWidget::updatePreview() {
    if (!m_lucide || m_iconName.isEmpty()) {
        m_iconLabel->clear();
        return;
    }

    QVariantMap options;
    options["color"] = m_iconColor;

    QIcon icon = m_lucide->icon(m_iconName, options);
    if (!icon.isNull()) {
        QPixmap pixmap = icon.pixmap(QSize(m_iconSize, m_iconSize));
        m_iconLabel->setPixmap(pixmap);
    }
}

void IconPreviewWidget::updateSizeControls() {
    m_sizeSlider->blockSignals(true);
    m_sizeSpinBox->blockSignals(true);

    m_sizeSlider->setValue(m_iconSize);
    m_sizeSpinBox->setValue(m_iconSize);

    m_sizeSlider->blockSignals(false);
    m_sizeSpinBox->blockSignals(false);
}

void IconPreviewWidget::onSizeSliderChanged(int value) {
    setIconSize(value);
}

void IconPreviewWidget::onSizeSpinBoxChanged(int value) {
    setIconSize(value);
}

void IconPreviewWidget::onColorButtonClicked() {
    // TODO: Implement color picker dialog
}

void IconPreviewWidget::onCopyIconClicked() {
    emit copyRequested("icon");
}

void IconPreviewWidget::refreshPreview() {
    updatePreview();
}

void IconPreviewWidget::resetToDefaults() {
    setIconSize(DEFAULT_PREVIEW_SIZE);
    setIconColor(Qt::black);
}

// CodeExampleWidget Implementation
CodeExampleWidget::CodeExampleWidget(QWidget* parent) : QWidget(parent), m_highlighter(nullptr) {
    setupUI();
}

void CodeExampleWidget::setupUI() {
    m_layout = new QVBoxLayout(this);

    // Header
    m_headerLayout = new QHBoxLayout();
    m_languageCombo = new QComboBox(this);
    m_languageCombo->addItems({"C++", "JavaScript", "HTML", "CSS"});

    m_copyButton = new QToolButton(this);
    m_copyButton->setText("Copy");

    m_headerLayout->addWidget(new QLabel("Language:", this));
    m_headerLayout->addWidget(m_languageCombo);
    m_headerLayout->addStretch();
    m_headerLayout->addWidget(m_copyButton);

    // Code editor
    m_codeEdit = new QPlainTextEdit(this);
    m_codeEdit->setReadOnly(true);
    m_codeEdit->setFont(QFont("Consolas", 9));

    m_layout->addLayout(m_headerLayout);
    m_layout->addWidget(m_codeEdit, 1);

    // Setup syntax highlighting
    m_highlighter =
        new CodeSyntaxHighlighter(CodeSyntaxHighlighter::CPlusPlus, m_codeEdit->document());

    connect(m_languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &CodeExampleWidget::onLanguageChanged);
    connect(m_copyButton, &QToolButton::clicked, this, &CodeExampleWidget::onCopyCodeClicked);
}

void CodeExampleWidget::setIconName(const QString& iconName) {
    m_iconName = iconName;
    updateExamples();
}

void CodeExampleWidget::updateExamples() {
    if (m_iconName.isEmpty()) {
        m_codeEdit->clear();
        return;
    }

    int languageIndex = m_languageCombo->currentIndex();
    switch (languageIndex) {
        case 0:
            generateCppExample();
            break;
        case 1:
            generateJavaScriptExample();
            break;
        case 2:
            generateHtmlExample();
            break;
        case 3:
            generateCssExample();
            break;
    }
}

void CodeExampleWidget::generateCppExample() {
    QString code = QString("// Using QtLucide in C++\n"
                           "#include <QtLucide/QtLucide.h>\n\n"
                           "lucide::QtLucide* lucide = new lucide::QtLucide(this);\n"
                           "lucide->initLucide();\n\n"
                           "// Get icon\n"
                           "QIcon icon = lucide->icon(\"%1\");\n\n"
                           "// Use in button\n"
                           "QPushButton* button = new QPushButton(this);\n"
                           "button->setIcon(icon);\n\n"
                           "// Use in label\n"
                           "QLabel* label = new QLabel(this);\n"
                           "label->setPixmap(icon.pixmap(32, 32));")
                       .arg(m_iconName);

    m_codeEdit->setPlainText(code);
}

void CodeExampleWidget::generateJavaScriptExample() {
    QString code = QString("// Using icon in web context\n"
                           "const iconName = '%1';\n"
                           "const iconElement = document.createElement('i');\n"
                           "iconElement.className = `lucide-${iconName}`;\n"
                           "document.body.appendChild(iconElement);")
                       .arg(m_iconName);

    m_codeEdit->setPlainText(code);
}

void CodeExampleWidget::generateHtmlExample() {
    QString code = QString("<!-- HTML usage -->\n"
                           "<i class=\"lucide-%1\"></i>\n\n"
                           "<!-- With custom size -->\n"
                           "<i class=\"lucide-%1\" style=\"width: 24px; height: 24px;\"></i>")
                       .arg(m_iconName);

    m_codeEdit->setPlainText(code);
}

void CodeExampleWidget::generateCssExample() {
    QString code = QString("/* CSS styling for %1 icon */\n"
                           ".lucide-%1 {\n"
                           "    width: 24px;\n"
                           "    height: 24px;\n"
                           "    color: #333;\n"
                           "}\n\n"
                           ".lucide-%1:hover {\n"
                           "    color: #007bff;\n"
                           "}")
                       .arg(m_iconName);

    m_codeEdit->setPlainText(code);
}

void CodeExampleWidget::onLanguageChanged(int index) {
    CodeSyntaxHighlighter::Language language = static_cast<CodeSyntaxHighlighter::Language>(index);
    m_highlighter->setLanguage(language);
    updateExamples();
}

void CodeExampleWidget::onCopyCodeClicked() {
    QString code = m_codeEdit->toPlainText();
    QString language = m_languageCombo->currentText();

    QApplication::clipboard()->setText(code);
    emit copyCodeRequested(code, language);
}

// IconDetailsPanel Implementation
IconDetailsPanel::IconDetailsPanel(lucide::QtLucide* lucide, IconMetadataManager* metadataManager,
                                   QWidget* parent)
    : QWidget(parent), m_lucide(lucide), m_metadataManager(metadataManager),
      m_updateTimer(new QTimer(this)) {
    setupUI();

    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(UPDATE_DELAY);
    connect(m_updateTimer, &QTimer::timeout, this, &IconDetailsPanel::refreshDetails);
}

IconDetailsPanel::~IconDetailsPanel() {}

void IconDetailsPanel::setupUI() {
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(4, 4, 4, 4);

    // Scroll area for content
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_contentWidget = new QWidget();
    m_scrollArea->setWidget(m_contentWidget);

    setupHeaderSection();
    setupTabWidget();

    m_layout->addWidget(m_scrollArea);
}

void IconDetailsPanel::setupHeaderSection() {
    m_headerFrame = new QFrame(m_contentWidget);
    m_headerFrame->setFrameStyle(QFrame::StyledPanel);

    m_headerLayout = new QHBoxLayout(m_headerFrame);

    m_iconLabel = new QLabel(m_headerFrame);
    m_iconLabel->setFixedSize(48, 48);
    m_iconLabel->setAlignment(Qt::AlignCenter);

    m_nameLabel = new QLabel("No icon selected", m_headerFrame);
    m_nameLabel->setStyleSheet("font-size: 14px; font-weight: bold;");

    m_favoriteButton = new QToolButton(m_headerFrame);
    m_favoriteButton->setText("♥");
    m_favoriteButton->setCheckable(true);

    m_headerLayout->addWidget(m_iconLabel);
    m_headerLayout->addWidget(m_nameLabel, 1);
    m_headerLayout->addWidget(m_favoriteButton);

    connect(m_favoriteButton, &QToolButton::clicked, this,
            &IconDetailsPanel::onFavoriteButtonClicked);
}

void IconDetailsPanel::setupTabWidget() {
    m_tabWidget = new QTabWidget(m_contentWidget);

    setupMetadataTab();
    setupPreviewTab();
    setupCodeTab();

    connect(m_tabWidget, &QTabWidget::currentChanged, this, &IconDetailsPanel::onTabChanged);
}

void IconDetailsPanel::setupMetadataTab() {
    m_metadataTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_metadataTab);

    m_metadataTable = new QTableWidget(0, 2, m_metadataTab);
    m_metadataTable->setHorizontalHeaderLabels({"Property", "Value"});
    m_metadataTable->horizontalHeader()->setStretchLastSection(true);

    layout->addWidget(m_metadataTable);
    m_tabWidget->addTab(m_metadataTab, "Details");
}

void IconDetailsPanel::setupPreviewTab() {
    m_previewTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_previewTab);

    m_previewWidget = new IconPreviewWidget(m_lucide, m_previewTab);
    layout->addWidget(m_previewWidget);

    m_tabWidget->addTab(m_previewTab, "Preview");
}

void IconDetailsPanel::setupCodeTab() {
    m_codeTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_codeTab);

    m_codeWidget = new CodeExampleWidget(m_codeTab);
    layout->addWidget(m_codeWidget);

    m_tabWidget->addTab(m_codeTab, "Code");
}

void IconDetailsPanel::setIconName(const QString& iconName) {
    if (m_currentIconName != iconName) {
        m_currentIconName = iconName;
        m_updateTimer->start();
    }
}

void IconDetailsPanel::clear() {
    m_currentIconName.clear();
    m_nameLabel->setText("No icon selected");
    m_iconLabel->clear();
    m_metadataTable->setRowCount(0);
    m_favoriteButton->setChecked(false);
}

void IconDetailsPanel::refreshDetails() {
    if (m_currentIconName.isEmpty()) {
        clear();
        return;
    }

    updateHeader();
    updateMetadata();
    updatePreview();
    updateCodeExamples();
}

void IconDetailsPanel::updateHeader() {
    if (!m_lucide || m_currentIconName.isEmpty())
        return;

    // Update icon
    QIcon icon = m_lucide->icon(m_currentIconName);
    if (!icon.isNull()) {
        m_iconLabel->setPixmap(icon.pixmap(48, 48));
    }

    // Update name
    IconMetadata metadata = m_metadataManager->getIconMetadata(m_currentIconName);
    QString displayName = metadata.isValid() ? metadata.getDisplayName() : m_currentIconName;
    m_nameLabel->setText(displayName);

    // Update favorite status
    updateFavoriteStatus();
}

void IconDetailsPanel::updateMetadata() {
    if (!m_metadataManager)
        return;

    IconMetadata metadata = m_metadataManager->getIconMetadata(m_currentIconName);

    m_metadataTable->setRowCount(0);

    if (metadata.isValid()) {
        populateMetadataTable();
    }
}

void IconDetailsPanel::populateMetadataTable() {
    IconMetadata metadata = m_metadataManager->getIconMetadata(m_currentIconName);

    auto addRow = [this](const QString& property, const QString& value) {
        int row = m_metadataTable->rowCount();
        m_metadataTable->insertRow(row);
        m_metadataTable->setItem(row, 0, new QTableWidgetItem(property));
        m_metadataTable->setItem(row, 1, new QTableWidgetItem(value));
    };

    addRow("Name", metadata.name);
    addRow("Display Name", metadata.getDisplayName());
    addRow("Categories", metadata.categories.join(", "));
    addRow("Tags", metadata.tags.join(", "));
    addRow("Contributors", metadata.contributors.join(", "));

    if (m_metadataManager) {
        int usageCount = m_metadataManager->getIconUsageCount(metadata.name);
        addRow("Usage Count", QString::number(usageCount));
    }
}

void IconDetailsPanel::updatePreview() {
    if (m_previewWidget) {
        m_previewWidget->setIconName(m_currentIconName);
    }
}

void IconDetailsPanel::updateCodeExamples() {
    if (m_codeWidget) {
        m_codeWidget->setIconName(m_currentIconName);
    }
}

void IconDetailsPanel::updateFavoriteStatus() {
    if (m_metadataManager && !m_currentIconName.isEmpty()) {
        bool isFavorite = m_metadataManager->isFavorite(m_currentIconName);
        m_favoriteButton->setChecked(isFavorite);
        m_favoriteButton->setText(isFavorite ? "♥" : "♡");
    }
}

void IconDetailsPanel::onFavoriteButtonClicked() {
    if (!m_metadataManager || m_currentIconName.isEmpty())
        return;

    bool isFavorite = m_favoriteButton->isChecked();

    if (isFavorite) {
        m_metadataManager->addToFavorites(m_currentIconName);
    } else {
        m_metadataManager->removeFromFavorites(m_currentIconName);
    }

    emit favoriteToggled(m_currentIconName, isFavorite);
}

void IconDetailsPanel::onTabChanged(int index) {
    Q_UNUSED(index)
    // Refresh content when tab changes
    m_updateTimer->start();
}

// Copy functionality implementations
void IconDetailsPanel::onCopyNameClicked() {
    if (!m_currentIconName.isEmpty()) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(m_currentIconName);

        // Show temporary feedback
        if (m_nameLabel) {
            QString originalText = m_nameLabel->text();
            m_nameLabel->setText("Copied!");
            m_nameLabel->setStyleSheet("color: green; font-weight: bold;");

            QTimer::singleShot(1500, this, [this, originalText]() {
                if (m_nameLabel) {
                    m_nameLabel->setText(originalText);
                    m_nameLabel->setStyleSheet("");
                }
            });
        }
    }
}

void IconDetailsPanel::onCopyCodeClicked() {
    if (!m_currentIconName.isEmpty()) {
        QString codeSnippet = QString("lucide.icon(\"%1\")").arg(m_currentIconName);

        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(codeSnippet);

        // Show feedback in status or temporary message
        emit copyRequested(m_currentIconName, "code", codeSnippet);
    }
}

void IconDetailsPanel::onCopySvgClicked() {
    if (!m_currentIconName.isEmpty() && m_lucide) {
        QByteArray svgData = m_lucide->svgData(m_currentIconName);
        if (!svgData.isEmpty()) {
            QString svgString = QString::fromUtf8(svgData);

            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setText(svgString);

            // Show feedback
            emit copyRequested(m_currentIconName, "svg", svgString);
        }
    }
}
void IconDetailsPanel::onPreviewSizeChanged(int size) {
    Q_UNUSED(size)
}
void IconDetailsPanel::onPreviewColorChanged(const QColor& color) {
    Q_UNUSED(color)
}
void IconDetailsPanel::onCodeCopyRequested(const QString& code, const QString& language) {
    Q_UNUSED(code)
    Q_UNUSED(language)
}

// IconPreviewWidget missing methods
void IconPreviewWidget::onCopyCodeClicked() {
    emit copyRequested("code");
}
void IconPreviewWidget::onCopySvgClicked() {
    emit copyRequested("svg");
}

// CodeSyntaxHighlighter missing constructor
CodeSyntaxHighlighter::CodeSyntaxHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent), m_language(CPlusPlus) {
    setLanguage(m_language);
}

// Missing destructors
IconPreviewWidget::~IconPreviewWidget() = default;
CodeExampleWidget::~CodeExampleWidget() = default;

// Missing methods declared in headers but not implemented
void IconPreviewWidget::zoomIn() { /* TODO */ }
void IconPreviewWidget::zoomOut() { /* TODO */ }
void IconPreviewWidget::zoomToFit() { /* TODO */ }
void IconPreviewWidget::resetZoom() { /* TODO */ }
void IconPreviewWidget::onZoomSliderChanged(int value) {
    Q_UNUSED(value) /* TODO */
}
void IconPreviewWidget::onBackgroundChanged() { /* TODO */ }
void IconPreviewWidget::onPreviewModeChanged() { /* TODO */ }
void IconPreviewWidget::onExportClicked() { /* TODO */ }
void IconPreviewWidget::onAnimationTimer() { /* TODO */ }

void CodeExampleWidget::onExportCodeClicked() { /* TODO */ }
void CodeExampleWidget::onTemplateChanged() { /* TODO */ }
void CodeExampleWidget::onCustomizeTemplate() { /* TODO */ }

void IconDetailsPanel::updateUsageStatistics() { /* TODO */ }
void IconDetailsPanel::toggleViewMode() { /* TODO */ }
void IconDetailsPanel::expandAll() { /* TODO */ }
void IconDetailsPanel::collapseAll() { /* TODO */ }
void IconDetailsPanel::onExportClicked() { /* TODO */ }
void IconDetailsPanel::onShareClicked() { /* TODO */ }
void IconDetailsPanel::onCodeExportRequested(const QString& code, const QString& language,
                                             const QString& format) {
    Q_UNUSED(code)
    Q_UNUSED(language)
    Q_UNUSED(format) /* TODO */
}
void IconDetailsPanel::onViewModeChanged() { /* TODO */ }
void IconDetailsPanel::onMetadataEdited() { /* TODO */ }
void IconDetailsPanel::onRefreshTimer() { /* TODO */ }
void IconDetailsPanel::onPinToggled() { /* TODO */ }
void IconDetailsPanel::onExpandCollapseToggled() { /* TODO */ }

// Only methods that are actually declared in headers are implemented above
