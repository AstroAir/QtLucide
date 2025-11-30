/**
 * @file SearchWidget.cpp
 * @brief Implementation of SearchWidget
 * @author Max Qian
 * @date 2025
 * @copyright MIT Licensed
 */

#include "SearchWidget.h"

#include <QFont>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

namespace gallery {

SearchWidget::SearchWidget(QWidget* parent)
    : QWidget(parent), m_searchInput(nullptr), m_clearButton(nullptr), m_searchIconLabel(nullptr),
      m_shortcutLabel(nullptr), m_debounceTimer(std::make_unique<QTimer>(this)),
      m_debounceTime(300) {
    setupUI();
    setMaximumHeight(40);
}

SearchWidget::~SearchWidget() = default;

void SearchWidget::setupUI() {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(6);

    // Search icon
    m_searchIconLabel = new QLabel(this);
    m_searchIconLabel->setText("🔍");
    m_searchIconLabel->setFixedWidth(20);

    // Search input
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search 1634 icons...");
    m_searchInput->setMinimumHeight(28);
    m_searchInput->setStyleSheet("QLineEdit {"
                                 "  border: 1px solid #555;"
                                 "  border-radius: 4px;"
                                 "  padding: 4px 8px;"
                                 "  background-color: #2a2a2a;"
                                 "  color: white;"
                                 "  font-size: 11pt;"
                                 "}"
                                 "QLineEdit:focus {"
                                 "  border: 2px solid #0066CC;"
                                 "  outline: none;"
                                 "}");
    connect(m_searchInput, &QLineEdit::textChanged, this, &SearchWidget::onSearchTextChanged);

    // Clear button
    m_clearButton = new QPushButton(this);
    m_clearButton->setText("✕");
    m_clearButton->setFixedSize(24, 24);
    m_clearButton->setToolTip("Clear search");
    m_clearButton->setStyleSheet("QPushButton {"
                                 "  border: none;"
                                 "  background-color: transparent;"
                                 "  color: #888;"
                                 "  font-weight: bold;"
                                 "  padding: 0px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "  color: #ccc;"
                                 "}");
    m_clearButton->setVisible(false);
    connect(m_clearButton, &QPushButton::clicked, this, &SearchWidget::onClearClicked);

    // Shortcut hint
    m_shortcutLabel = new QLabel("Ctrl+K", this);
    m_shortcutLabel->setStyleSheet("color: #888;"
                                   "font-size: 9pt;");
    m_shortcutLabel->setMaximumWidth(50);
    m_shortcutLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Debounce timer
    m_debounceTimer->setSingleShot(true);
    connect(m_debounceTimer.get(), &QTimer::timeout, this, &SearchWidget::onDebounceTimeout);

    // Layout assembly
    layout->addWidget(m_searchIconLabel);
    layout->addWidget(m_searchInput);
    layout->addWidget(m_clearButton);
    layout->addWidget(m_shortcutLabel);

    setLayout(layout);
}

QString SearchWidget::searchText() const {
    return m_searchInput->text();
}

void SearchWidget::setSearchText(const QString& text) {
    m_searchInput->blockSignals(true);
    m_searchInput->setText(text);
    m_searchInput->blockSignals(false);
    updateClearButtonVisibility();
}

void SearchWidget::setPlaceholderText(const QString& text) {
    m_searchInput->setPlaceholderText(text);
}

void SearchWidget::setDebounceTime(int milliseconds) {
    m_debounceTime = std::max(0, milliseconds);
}

int SearchWidget::debounceTime() const {
    return m_debounceTime;
}

void SearchWidget::clear() {
    m_searchInput->clear();
    updateClearButtonVisibility();
}

void SearchWidget::focusSearchInput() {
    m_searchInput->setFocus();
    m_searchInput->selectAll();
}

void SearchWidget::keyPressEvent(QKeyEvent* event) {
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_K) {
        focusSearchInput();
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void SearchWidget::onSearchTextChanged(const QString& text) {
    updateClearButtonVisibility();

    if (m_debounceTime <= 0) {
        // Emit immediately without debouncing
        emit searchTextChanged(text);
    } else {
        // Start debounce timer
        m_debounceTimer->stop();
        m_debounceTimer->start(m_debounceTime);
    }
}

void SearchWidget::onClearClicked() {
    clear();
    emit searchTextChanged("");
    m_searchInput->setFocus();
}

void SearchWidget::onDebounceTimeout() {
    emit searchTextChanged(m_searchInput->text());
}

void SearchWidget::updateClearButtonVisibility() {
    m_clearButton->setVisible(!m_searchInput->text().isEmpty());
}

} // namespace gallery
