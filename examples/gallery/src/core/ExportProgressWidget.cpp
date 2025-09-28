#include "BatchExportManager.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

ExportProgressWidget::ExportProgressWidget(QWidget* parent)
    : QWidget(parent), m_mainLayout(nullptr), m_progressLayout(nullptr), m_buttonLayout(nullptr),
      m_statusLabel(nullptr), m_progressBar(nullptr), m_progressLabel(nullptr),
      m_currentFileLabel(nullptr), m_timeLabel(nullptr), m_statsLabel(nullptr),
      m_cancelButton(nullptr), m_pauseButton(nullptr), m_currentProgress(0), m_totalProgress(0),
      m_elapsedTime(0), m_estimatedTimeRemaining(0), m_showStatistics(false), m_isPaused(false),
      m_updateTimer(nullptr) {
    setupUI();
}

ExportProgressWidget::~ExportProgressWidget() = default;

void ExportProgressWidget::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_progressLayout = new QHBoxLayout();
    m_buttonLayout = new QHBoxLayout();

    // Status and progress
    m_statusLabel = new QLabel("Preparing export...", this);
    m_progressBar = new QProgressBar(this);
    m_progressLabel = new QLabel("0 / 0", this);
    m_currentFileLabel = new QLabel("", this);
    m_timeLabel = new QLabel("", this);
    m_statsLabel = new QLabel("", this);

    // Buttons
    m_cancelButton = new QPushButton("Cancel", this);
    m_pauseButton = new QPushButton("Pause", this);

    // Layout
    m_progressLayout->addWidget(m_progressBar);
    m_progressLayout->addWidget(m_progressLabel);

    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_pauseButton);
    m_buttonLayout->addWidget(m_cancelButton);

    m_mainLayout->addWidget(m_statusLabel);
    m_mainLayout->addLayout(m_progressLayout);
    m_mainLayout->addWidget(m_currentFileLabel);
    m_mainLayout->addWidget(m_timeLabel);
    m_mainLayout->addWidget(m_statsLabel);
    m_mainLayout->addLayout(m_buttonLayout);

    // Connections
    connect(m_cancelButton, &QPushButton::clicked, this, &ExportProgressWidget::onCancelClicked);
    connect(m_pauseButton, &QPushButton::clicked, this, &ExportProgressWidget::onPauseClicked);

    // Update timer
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(100);
    connect(m_updateTimer, &QTimer::timeout, this, &ExportProgressWidget::updateDisplay);
    m_updateTimer->start();

    setMinimumHeight(120);
}

void ExportProgressWidget::onCancelClicked() {
    emit cancelRequested();
}

void ExportProgressWidget::onPauseClicked() {
    if (m_isPaused) {
        emit resumeRequested();
        m_isPaused = false;
        if (m_pauseButton) {
            m_pauseButton->setText("Pause");
        }
    } else {
        emit pauseRequested();
        m_isPaused = true;
        if (m_pauseButton) {
            m_pauseButton->setText("Resume");
        }
    }
}

void ExportProgressWidget::updateDisplay() {
    // Update display elements
    if (m_statusLabel && !m_status.isEmpty()) {
        m_statusLabel->setText(m_status);
    }
}

// Public methods
void ExportProgressWidget::setProgress(int current, int total) {
    m_currentProgress = current;
    m_totalProgress = total;

    if (m_progressBar) {
        m_progressBar->setMaximum(total);
        m_progressBar->setValue(current);
    }

    if (m_progressLabel) {
        m_progressLabel->setText(QString("%1 / %2").arg(current).arg(total));
    }
}

void ExportProgressWidget::setCurrentFile(const QString& fileName) {
    m_currentFile = fileName;
    if (m_currentFileLabel) {
        m_currentFileLabel->setText(QString("Current: %1").arg(fileName));
    }
}

void ExportProgressWidget::setStatus(const QString& status) {
    m_status = status;
    if (m_statusLabel) {
        m_statusLabel->setText(status);
    }
}

void ExportProgressWidget::setElapsedTime(qint64 milliseconds) {
    m_elapsedTime = milliseconds;
}

void ExportProgressWidget::setEstimatedTimeRemaining(qint64 milliseconds) {
    m_estimatedTimeRemaining = milliseconds;
}

void ExportProgressWidget::setStatistics(const ExportStats& stats) {
    m_stats = stats;
    if (m_showStatistics && m_statsLabel) {
        QString statsText = QString("Success: %1, Failed: %2, Total: %3")
                                .arg(stats.successfulTasks)
                                .arg(stats.failedTasks)
                                .arg(stats.totalTasks);
        m_statsLabel->setText(statsText);
    }
}

void ExportProgressWidget::showStatistics(bool show) {
    m_showStatistics = show;
    if (m_statsLabel) {
        m_statsLabel->setVisible(show);
    }
}

void ExportProgressWidget::reset() {
    m_currentProgress = 0;
    m_totalProgress = 0;
    m_currentFile.clear();
    m_status = "Preparing export...";
    m_elapsedTime = 0;
    m_estimatedTimeRemaining = 0;
    m_isPaused = false;

    if (m_progressBar) {
        m_progressBar->setValue(0);
        m_progressBar->setMaximum(0);
    }
    if (m_progressLabel) {
        m_progressLabel->setText("0 / 0");
    }
    if (m_statusLabel) {
        m_statusLabel->setText(m_status);
    }
    if (m_currentFileLabel) {
        m_currentFileLabel->clear();
    }
    if (m_timeLabel) {
        m_timeLabel->clear();
    }
    if (m_statsLabel) {
        m_statsLabel->clear();
    }
    if (m_pauseButton) {
        m_pauseButton->setText("Pause");
    }
}

void ExportProgressWidget::setShowCancelButton(bool show) {
    if (m_cancelButton) {
        m_cancelButton->setVisible(show);
    }
}

void ExportProgressWidget::setShowPauseButton(bool show) {
    if (m_pauseButton) {
        m_pauseButton->setVisible(show);
    }
}

void ExportProgressWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
}
