/**
 * QtLucide Gallery Application - Icon Item Implementation
 */

#include "IconItem.h"
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QHelpEvent>
#include <QMimeData>
#include <QPainter>
#include <QRegularExpression>
#include <QStyleOption>
#include <QToolTip>

// Static color definitions
const QColor IconItem::SELECTION_COLOR = QColor(42, 130, 218);
const QColor IconItem::HOVER_COLOR = QColor(255, 255, 255, 30);
const QColor IconItem::FAVORITE_COLOR = QColor(255, 193, 7);
const QColor IconItem::BORDER_COLOR = QColor(200, 200, 200, 100);

IconItem::IconItem(const QString& iconName, lucide::QtLucide* lucide,
                   IconMetadataManager* metadataManager, QWidget* parent)
    : QFrame(parent), m_iconName(iconName), m_lucide(lucide), m_metadataManager(metadataManager),
      m_selected(false), m_favorite(false), m_hovered(false), m_iconSize(DEFAULT_ICON_SIZE),
      m_displayMode(GridMode), m_iconColor(Qt::black), m_hoverAnimation(nullptr),
      m_shadowEffect(nullptr), m_tooltipTimer(new QTimer(this)) {
    setupUI();
    setupContextMenu();
    setupAnimations();
    updateFromMetadata();

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_tooltipTimer->setSingleShot(true);
    m_tooltipTimer->setInterval(TOOLTIP_DELAY);
    connect(m_tooltipTimer, &QTimer::timeout, this, [this]() { updateTooltip(); });
}

IconItem::~IconItem() {
    delete m_shadowEffect;
}

void IconItem::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);
    m_mainLayout->setSpacing(2);

    // Create icon label
    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setScaledContents(false);

    // Create name label
    m_nameLabel = new QLabel(this);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setWordWrap(true);
    m_nameLabel->setStyleSheet("QLabel { color: palette(text); font-size: 10px; }");

    // Create favorite button
    m_favoriteButton = new QPushButton(this);
    m_favoriteButton->setFixedSize(16, 16);
    m_favoriteButton->setFlat(true);
    m_favoriteButton->setVisible(false);
    connect(m_favoriteButton, &QPushButton::clicked, this, &IconItem::onToggleFavorite);

    updateLayout();
    updateIconDisplay();
    updateNameDisplay();
}

void IconItem::setupContextMenu() {
    m_contextMenu = new QMenu(this);

    m_copyNameAction = m_contextMenu->addAction("Copy Name");
    m_copyCodeAction = m_contextMenu->addAction("Copy Code");
    m_copySvgAction = m_contextMenu->addAction("Copy SVG");
    m_contextMenu->addSeparator();
    m_toggleFavoriteAction = m_contextMenu->addAction("Add to Favorites");
    m_contextMenu->addSeparator();
    m_showDetailsAction = m_contextMenu->addAction("Show Details");

    connect(m_copyNameAction, &QAction::triggered, this, &IconItem::onCopyIconName);
    connect(m_copyCodeAction, &QAction::triggered, this, &IconItem::onCopyIconCode);
    connect(m_copySvgAction, &QAction::triggered, this, &IconItem::onCopyIconSvg);
    connect(m_toggleFavoriteAction, &QAction::triggered, this, &IconItem::onToggleFavorite);
    connect(m_showDetailsAction, &QAction::triggered, this, &IconItem::onShowDetails);
}

void IconItem::setupAnimations() {
    m_hoverAnimation = new QPropertyAnimation(this, "geometry", this);
    m_hoverAnimation->setDuration(ANIMATION_DURATION);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_hoverAnimation, &QPropertyAnimation::finished, this,
            &IconItem::onHoverAnimationFinished);
}

void IconItem::updateLayout() {
    // Clear existing layout
    while (QLayoutItem* item = m_mainLayout->takeAt(0)) {
        delete item;
    }

    switch (m_displayMode) {
        case GridMode:
            m_mainLayout->addWidget(m_iconLabel, 0, Qt::AlignCenter);
            m_mainLayout->addWidget(m_nameLabel, 0, Qt::AlignCenter);
            m_nameLabel->setVisible(true);
            break;

        case ListMode:
            m_contentLayout = new QHBoxLayout();
            m_contentLayout->addWidget(m_iconLabel, 0, Qt::AlignCenter);
            m_contentLayout->addWidget(m_nameLabel, 1, Qt::AlignLeft | Qt::AlignVCenter);
            m_mainLayout->addLayout(m_contentLayout);
            m_nameLabel->setVisible(true);
            break;

        case CompactMode:
            m_mainLayout->addWidget(m_iconLabel, 0, Qt::AlignCenter);
            m_nameLabel->setVisible(false);
            break;
    }

    // Add favorite button overlay
    if (m_favoriteButton) {
        m_favoriteButton->raise();
    }
}

void IconItem::updateIconDisplay() {
    if (!m_lucide || m_iconName.isEmpty()) {
        return;
    }

    QVariantMap options;
    options["color"] = m_iconColor;
    options["scale-factor"] = 0.9;

    QIcon icon = m_lucide->icon(m_iconName, options);
    if (!icon.isNull()) {
        QPixmap pixmap = icon.pixmap(QSize(m_iconSize, m_iconSize));
        m_iconLabel->setPixmap(pixmap);
        m_iconLabel->setFixedSize(m_iconSize + 8, m_iconSize + 8);
    }
}

void IconItem::updateNameDisplay() {
    QString displayName = formatIconName();
    m_nameLabel->setText(displayName);

    // Adjust font size based on display mode
    QFont font = m_nameLabel->font();
    switch (m_displayMode) {
        case GridMode:
            font.setPointSize(8);
            break;
        case ListMode:
            font.setPointSize(9);
            break;
        case CompactMode:
            font.setPointSize(7);
            break;
    }
    m_nameLabel->setFont(font);
}

void IconItem::updateSelectionState() {
    setProperty("selected", m_selected);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void IconItem::updateHoverState() {
    setProperty("hovered", m_hovered);

    if (m_hovered) {
        applyVisualEffects();
        m_favoriteButton->setVisible(true);
    } else {
        removeVisualEffects();
        m_favoriteButton->setVisible(m_favorite);
    }

    style()->unpolish(this);
    style()->polish(this);
    update();
}

void IconItem::updateFavoriteState() {
    if (m_metadataManager) {
        m_favorite = m_metadataManager->isFavorite(m_iconName);
    }

    updateFavoriteButton();
    setProperty("favorite", m_favorite);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void IconItem::updateFavoriteButton() {
    if (!m_favoriteButton)
        return;

    QString iconName = m_favorite ? "heart" : "heart-off";
    if (m_lucide) {
        QVariantMap options;
        options["color"] = m_favorite ? FAVORITE_COLOR : QColor(128, 128, 128);
        QIcon icon = m_lucide->icon(iconName, options);
        m_favoriteButton->setIcon(icon);
    }

    m_favoriteButton->setToolTip(m_favorite ? "Remove from Favorites" : "Add to Favorites");
    m_toggleFavoriteAction->setText(m_favorite ? "Remove from Favorites" : "Add to Favorites");
}

void IconItem::updateTooltip() {
    QString tooltip = createTooltipText();
    setToolTip(tooltip);
}

QString IconItem::createTooltipText() const {
    IconMetadata metadata = this->metadata();
    if (!metadata.isValid()) {
        return m_iconName;
    }

    QStringList lines;
    lines << QString("<b>%1</b>").arg(metadata.getDisplayName());
    lines << QString("Name: %1").arg(m_iconName);

    if (!metadata.categories.isEmpty()) {
        lines << QString("Categories: %1").arg(metadata.categories.join(", "));
    }

    if (!metadata.tags.isEmpty()) {
        QStringList limitedTags = metadata.tags.mid(0, 5);
        if (metadata.tags.size() > 5) {
            limitedTags << QString("... (%1 more)").arg(metadata.tags.size() - 5);
        }
        lines << QString("Tags: %1").arg(limitedTags.join(", "));
    }

    if (m_metadataManager) {
        int usageCount = m_metadataManager->getIconUsageCount(m_iconName);
        if (usageCount > 0) {
            lines << QString("Used %1 time(s)").arg(usageCount);
        }
    }

    return lines.join("<br>");
}

QString IconItem::formatIconName() const {
    IconMetadata metadata = this->metadata();
    if (metadata.isValid() && !metadata.displayName.isEmpty()) {
        return metadata.displayName;
    }

    // Fallback formatting
    QString formatted = m_iconName;
    formatted.replace(QRegularExpression("[-_]"), " ");

    // Capitalize first letter of each word
    QStringList words = formatted.split(" ", Qt::SkipEmptyParts);
    for (QString& word : words) {
        if (!word.isEmpty()) {
            word[0] = word[0].toUpper();
        }
    }

    return words.join(" ");
}

IconMetadata IconItem::metadata() const {
    if (m_metadataManager) {
        return m_metadataManager->getIconMetadata(m_iconName);
    }
    return IconMetadata();
}

QStringList IconItem::categories() const {
    return metadata().categories;
}

QStringList IconItem::tags() const {
    return metadata().tags;
}

void IconItem::setSelected(bool selected) {
    if (m_selected != selected) {
        m_selected = selected;
        updateSelectionState();
        emit selectionChanged(selected);
    }
}

void IconItem::setFavorite(bool favorite) {
    if (m_favorite != favorite) {
        m_favorite = favorite;
        updateFavoriteState();
        emit favoriteChanged(favorite);
    }
}

void IconItem::setIconSize(int size) {
    size = qBound(MIN_ICON_SIZE, size, MAX_ICON_SIZE);
    if (m_iconSize != size) {
        m_iconSize = size;
        updateIconDisplay();
        emit iconSizeChanged(size);
    }
}

void IconItem::setDisplayMode(DisplayMode mode) {
    if (m_displayMode != mode) {
        m_displayMode = mode;
        updateLayout();
        updateNameDisplay();
    }
}

void IconItem::setIconColor(const QColor& color) {
    if (m_iconColor != color) {
        m_iconColor = color;
        updateIconDisplay();
    }
}

void IconItem::refreshIcon() {
    updateIconDisplay();
    updateNameDisplay();
    updateFromMetadata();
}

void IconItem::updateFromMetadata() {
    updateFavoriteState();
    updateTooltip();
}

void IconItem::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_iconName);
        if (m_metadataManager) {
            m_metadataManager->recordIconUsage(m_iconName);
        }
    }
    QFrame::mousePressEvent(event);
}

void IconItem::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(m_iconName);
    }
    QFrame::mouseDoubleClickEvent(event);
}

void IconItem::contextMenuEvent(QContextMenuEvent* event) {
    updateFavoriteButton();
    m_contextMenu->exec(event->globalPos());
}

void IconItem::enterEvent(QEnterEvent* event) {
    m_hovered = true;
    updateHoverState();
    m_tooltipTimer->start();
    QFrame::enterEvent(event);
}

void IconItem::leaveEvent(QEvent* event) {
    m_hovered = false;
    updateHoverState();
    m_tooltipTimer->stop();
    QFrame::leaveEvent(event);
}

void IconItem::paintEvent(QPaintEvent* event) {
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect().adjusted(1, 1, -1, -1);

    // Draw selection background
    if (m_selected) {
        painter.fillRect(rect, QColor(SELECTION_COLOR.red(), SELECTION_COLOR.green(),
                                      SELECTION_COLOR.blue(), 50));
        painter.setPen(QPen(SELECTION_COLOR, 2));
        painter.drawRoundedRect(rect, 4, 4);
    }

    // Draw hover background
    if (m_hovered && !m_selected) {
        painter.fillRect(rect, HOVER_COLOR);
        painter.setPen(QPen(BORDER_COLOR, 1));
        painter.drawRoundedRect(rect, 4, 4);
    }

    // Draw favorite indicator
    if (m_favorite && !m_hovered) {
        QRect favoriteRect(rect.right() - 20, rect.top() + 4, 16, 16);
        painter.fillRect(favoriteRect, FAVORITE_COLOR);
        painter.setPen(Qt::white);
        painter.drawText(favoriteRect, Qt::AlignCenter, "★");
    }
}

void IconItem::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);

    // Position favorite button
    if (m_favoriteButton) {
        QRect rect = this->rect();
        m_favoriteButton->move(rect.right() - 20, rect.top() + 4);
    }
}

void IconItem::onCopyIconName() {
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(m_iconName);
    emit copyRequested(m_iconName, "name");
}

void IconItem::onCopyIconCode() {
    QString code = QString("lucide->icon(\"%1\")").arg(m_iconName);
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(code);
    emit copyRequested(m_iconName, "code");
}

void IconItem::onCopyIconSvg() {
    if (m_lucide) {
        QByteArray svgData = m_lucide->svgData(m_iconName);
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(QString::fromUtf8(svgData));
        emit copyRequested(m_iconName, "svg");
    }
}

void IconItem::onToggleFavorite() {
    bool newFavoriteState = !m_favorite;
    setFavorite(newFavoriteState);

    if (m_metadataManager) {
        if (newFavoriteState) {
            m_metadataManager->addToFavorites(m_iconName);
        } else {
            m_metadataManager->removeFromFavorites(m_iconName);
        }
    }

    emit favoriteToggled(m_iconName, newFavoriteState);
}

void IconItem::onShowDetails() {
    emit doubleClicked(m_iconName);
}

void IconItem::onHoverAnimationFinished() {
    // Animation completed
}

void IconItem::startHoverAnimation(bool entering) {
    if (!m_hoverAnimation)
        return;

    QRect currentGeometry = geometry();
    QRect targetGeometry = currentGeometry;

    if (entering) {
        // Slightly expand on hover
        targetGeometry.adjust(-2, -2, 2, 2);
    }

    m_hoverAnimation->setStartValue(currentGeometry);
    m_hoverAnimation->setEndValue(targetGeometry);
    m_hoverAnimation->start();
}

void IconItem::applyVisualEffects() {
    if (!m_shadowEffect) {
        m_shadowEffect = new QGraphicsDropShadowEffect(this);
        m_shadowEffect->setBlurRadius(8);
        m_shadowEffect->setColor(QColor(0, 0, 0, 80));
        m_shadowEffect->setOffset(2, 2);
    }
    setGraphicsEffect(m_shadowEffect);
}

void IconItem::removeVisualEffects() {
    setGraphicsEffect(nullptr);
}

QSize IconItem::calculateOptimalSize() const {
    int width = m_iconSize + 16;
    int height = m_iconSize + 16;

    if (m_displayMode == GridMode && m_nameLabel->isVisible()) {
        height += 24; // Space for name
    } else if (m_displayMode == ListMode) {
        width += 100; // Space for name
    }

    return QSize(width, height);
}

// IconItemContainer implementation
IconItemContainer::IconItemContainer(QWidget* parent)
    : QWidget(parent), m_itemSize(64), m_displayMode(IconItem::GridMode), m_columns(1),
      m_spacing(4) {}
void IconItemContainer::addIconItem(IconItem* item) {
    m_items.append(item);
}
void IconItemContainer::removeIconItem(IconItem* item) {
    m_items.removeOne(item);
}
void IconItemContainer::clearItems() {
    m_items.clear();
}
void IconItemContainer::setItemSize(int size) {
    m_itemSize = size;
}
void IconItemContainer::setDisplayMode(IconItem::DisplayMode mode) {
    m_displayMode = mode;
}
void IconItemContainer::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateLayout();
}
void IconItemContainer::updateLayout() { /* TODO */ }
void IconItemContainer::calculateLayout() { /* TODO */ }
