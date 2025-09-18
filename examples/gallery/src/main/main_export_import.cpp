/**
 * QtLucide Gallery Application - Export/Import Test Main
 *
 * Test application for the new export and import functionality
 */

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QTextEdit>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QTimer>

#ifdef QTLUCIDE_AVAILABLE
#include <QtLucide/QtLucide.h>
#endif

#include "../ui/dialogs/ExportDialog.h"
#include "../ui/dialogs/ImportDialog.h"
#include "../ui/themes/ThemeManager.h"
#include "../core/BatchExportManager.h"

class ExportImportTestWindow : public QMainWindow
{
    Q_OBJECT

public:
    ExportImportTestWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , m_themeManager(new ThemeManager(this))
        , m_exportManager(new BatchExportManager(this))
        , m_exportDialog(nullptr)
        , m_importDialog(nullptr)
#ifdef QTLUCIDE_AVAILABLE
        , m_lucide(new lucide::QtLucide(this))
#else
        , m_lucide(nullptr)
#endif
    {
        setWindowTitle("QtLucide Gallery - Export/Import Test");
        setMinimumSize(1000, 700);
        resize(1200, 800);

        setupUI();
        setupMenus();
        setupConnections();
        initializeComponents();

        statusBar()->showMessage("Export/Import test application initialized");
    }

private slots:
    void onShowExportDialog()
    {
        if (!m_exportDialog) {
            m_exportDialog = new ExportDialog(this);
            m_exportDialog->setThemeManager(m_themeManager);
#ifdef QTLUCIDE_AVAILABLE
            m_exportDialog->setLucide(m_lucide);
#endif
        }

        // Set some test icons
        QStringList testIcons = getTestIconNames();
        m_exportDialog->setIconNames(testIcons);

        // Show dialog
        if (m_exportDialog->exec() == QDialog::Accepted) {
            if (m_exportDialog->wasExportSuccessful()) {
                QStringList exportedFiles = m_exportDialog->getExportedFiles();
                showExportSuccess(exportedFiles);
            }
        }
    }

    void onShowImportDialog()
    {
        if (!m_importDialog) {
            m_importDialog = new ImportDialog(this);
            m_importDialog->setThemeManager(m_themeManager);
        }

        // Show dialog
        if (m_importDialog->exec() == QDialog::Accepted) {
            if (m_importDialog->wasImportSuccessful()) {
                QStringList importedTypes = m_importDialog->getImportedDataTypes();
                showImportSuccess(importedTypes);
            }
        }
    }

    void onQuickExport()
    {
        QString outputDir = QFileDialog::getExistingDirectory(
            this,
            "Select Export Directory",
            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)
        );

        if (outputDir.isEmpty()) return;

        // Create export config
        ExportConfig config;
        config.format = ExportFormat::PNG;
        config.sizes = {64, 128};
        config.outputDirectory = outputDir;
        config.fileNameTemplate = "{name}_{size}x{size}";
        config.transparentBackground = true;

        // Get test icons
        QStringList iconNames = getTestIconNames();
        if (iconNames.isEmpty()) {
            QMessageBox::information(this, "Quick Export", 
                "No icons available for export. This is a test application.");
            return;
        }

        // Create export tasks
        QList<ExportTask> tasks = BatchExportManager::createExportTasks(iconNames, config);
        
        if (tasks.isEmpty()) {
            QMessageBox::warning(this, "Quick Export", "No export tasks created.");
            return;
        }

        // Add tasks to export manager
        m_exportManager->clearTasks();
        m_exportManager->addExportTasks(tasks);

        // Start export
        m_exportManager->startExport();
        
        statusBar()->showMessage(QString("Exporting %1 icons to %2...")
                                .arg(iconNames.size()).arg(outputDir));
    }

    void onToggleTheme()
    {
        ThemeManager::Theme currentTheme = m_themeManager->currentTheme();
        ThemeManager::Theme newTheme = (currentTheme == ThemeManager::LightTheme) 
                                     ? ThemeManager::DarkTheme 
                                     : ThemeManager::LightTheme;
        
        m_themeManager->setTheme(newTheme);
        
        QString themeName = (newTheme == ThemeManager::LightTheme) ? "Light" : "Dark";
        statusBar()->showMessage(QString("Switched to %1 theme").arg(themeName));
    }

    void onAbout()
    {
        QMessageBox::about(this, "About Export/Import Test",
            "<h3>QtLucide Gallery Export/Import Test</h3>"
            "<p>This application demonstrates the new export and import functionality "
            "added to the QtLucide Gallery application.</p>"
            "<p><b>Features:</b></p>"
            "<ul>"
            "<li>Export icons in multiple formats (PNG, SVG, ICO, PDF, JPEG)</li>"
            "<li>Batch export with progress tracking</li>"
            "<li>Import/export settings and preferences</li>"
            "<li>Modern theme system with light/dark modes</li>"
            "<li>Responsive layout management</li>"
            "</ul>"
#ifdef QTLUCIDE_AVAILABLE
            "<p>QtLucide integration: <b>Enabled</b></p>"
#else
            "<p>QtLucide integration: <b>Disabled</b> (library not found)</p>"
#endif
            "<p>Built with Qt " QT_VERSION_STR "</p>");
    }

    void onExportFinished(bool success)
    {
        if (success) {
            ExportStats stats = m_exportManager->getStatistics();
            QString message = QString("Export completed successfully! "
                                    "Exported %1 files (%2 successful, %3 failed)")
                            .arg(stats.completedTasks)
                            .arg(stats.successfulTasks)
                            .arg(stats.failedTasks);
            
            statusBar()->showMessage(message);
            
            QMessageBox::information(this, "Export Complete", message);
        } else {
            statusBar()->showMessage("Export failed or was cancelled");
            QMessageBox::warning(this, "Export Failed", 
                "The export operation failed or was cancelled.");
        }
    }

    void onExportProgress(int completed, int total, const QString& currentFile)
    {
        QString message = QString("Exporting %1/%2: %3")
                         .arg(completed).arg(total).arg(currentFile);
        statusBar()->showMessage(message);
    }

private:
    void setupUI()
    {
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(8, 8, 8, 8);
        mainLayout->setSpacing(8);

        // Title
        QLabel* titleLabel = new QLabel("QtLucide Gallery Export/Import Test", this);
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 8px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Main content area
        QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
        
        // Left panel - Test controls
        QWidget* controlPanel = new QWidget(this);
        controlPanel->setMaximumWidth(300);
        controlPanel->setMinimumWidth(250);
        
        QVBoxLayout* controlLayout = new QVBoxLayout(controlPanel);
        controlLayout->setSpacing(8);
        
        QLabel* controlTitle = new QLabel("Test Controls", controlPanel);
        controlTitle->setStyleSheet("font-weight: bold; margin-bottom: 8px;");
        controlLayout->addWidget(controlTitle);
        
        QPushButton* exportDialogBtn = new QPushButton("Show Export Dialog", controlPanel);
        QPushButton* importDialogBtn = new QPushButton("Show Import Dialog", controlPanel);
        QPushButton* quickExportBtn = new QPushButton("Quick Export Test", controlPanel);
        QPushButton* themeToggleBtn = new QPushButton("Toggle Theme", controlPanel);
        
        controlLayout->addWidget(exportDialogBtn);
        controlLayout->addWidget(importDialogBtn);
        controlLayout->addWidget(quickExportBtn);
        controlLayout->addWidget(themeToggleBtn);
        controlLayout->addStretch();
        
        // Right panel - Information display
        QWidget* infoPanel = new QWidget(this);
        QVBoxLayout* infoLayout = new QVBoxLayout(infoPanel);
        
        QLabel* infoTitle = new QLabel("Information", infoPanel);
        infoTitle->setStyleSheet("font-weight: bold; margin-bottom: 8px;");
        infoLayout->addWidget(infoTitle);
        
        m_infoText = new QTextEdit(infoPanel);
        m_infoText->setReadOnly(true);
        m_infoText->setPlainText(
            "Export/Import Test Application\n\n"
            "This application demonstrates the new export and import functionality:\n\n"
            "• Export Dialog: Comprehensive export options with multiple formats\n"
            "• Import Dialog: Settings and configuration import\n"
            "• Quick Export: Simple batch export test\n"
            "• Theme Toggle: Switch between light and dark themes\n\n"
            "Click the buttons on the left to test different features."
        );
        infoLayout->addWidget(m_infoText);
        
        splitter->addWidget(controlPanel);
        splitter->addWidget(infoPanel);
        splitter->setSizes({300, 700});
        
        mainLayout->addWidget(splitter);

        // Connect buttons
        connect(exportDialogBtn, &QPushButton::clicked, this, &ExportImportTestWindow::onShowExportDialog);
        connect(importDialogBtn, &QPushButton::clicked, this, &ExportImportTestWindow::onShowImportDialog);
        connect(quickExportBtn, &QPushButton::clicked, this, &ExportImportTestWindow::onQuickExport);
        connect(themeToggleBtn, &QPushButton::clicked, this, &ExportImportTestWindow::onToggleTheme);
    }

    void setupMenus()
    {
        // File menu
        QMenu* fileMenu = menuBar()->addMenu("&File");
        
        QAction* exportAction = fileMenu->addAction("&Export Icons...");
        exportAction->setShortcut(QKeySequence("Ctrl+E"));
        connect(exportAction, &QAction::triggered, this, &ExportImportTestWindow::onShowExportDialog);
        
        QAction* importAction = fileMenu->addAction("&Import Settings...");
        importAction->setShortcut(QKeySequence("Ctrl+I"));
        connect(importAction, &QAction::triggered, this, &ExportImportTestWindow::onShowImportDialog);
        
        fileMenu->addSeparator();
        
        QAction* exitAction = fileMenu->addAction("E&xit");
        exitAction->setShortcut(QKeySequence::Quit);
        connect(exitAction, &QAction::triggered, this, &QWidget::close);

        // View menu
        QMenu* viewMenu = menuBar()->addMenu("&View");
        
        QAction* themeAction = viewMenu->addAction("Toggle &Theme");
        themeAction->setShortcut(QKeySequence("Ctrl+T"));
        connect(themeAction, &QAction::triggered, this, &ExportImportTestWindow::onToggleTheme);

        // Help menu
        QMenu* helpMenu = menuBar()->addMenu("&Help");
        
        QAction* aboutAction = helpMenu->addAction("&About");
        connect(aboutAction, &QAction::triggered, this, &ExportImportTestWindow::onAbout);
        
        QAction* aboutQtAction = helpMenu->addAction("About &Qt");
        connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    }

    void setupConnections()
    {
        // Export manager connections
        connect(m_exportManager, &BatchExportManager::exportFinished,
                this, &ExportImportTestWindow::onExportFinished);
        connect(m_exportManager, &BatchExportManager::exportProgress,
                this, &ExportImportTestWindow::onExportProgress);
    }

    void initializeComponents()
    {
#ifdef QTLUCIDE_AVAILABLE
        // Initialize QtLucide
        if (m_lucide && m_lucide->initLucide()) {
            m_exportManager->setLucide(m_lucide);
            statusBar()->showMessage("QtLucide initialized successfully");
        } else {
            statusBar()->showMessage("QtLucide initialization failed");
        }
#endif

        // Apply initial theme
        m_themeManager->setTheme(ThemeManager::LightTheme);
    }

    QStringList getTestIconNames() const
    {
        // Return some test icon names
        QStringList testIcons;
        
#ifdef QTLUCIDE_AVAILABLE
        if (m_lucide) {
            QStringList available = m_lucide->availableIcons();
            if (!available.isEmpty()) {
                // Take first 10 icons for testing
                int count = qMin(10, available.size());
                for (int i = 0; i < count; ++i) {
                    testIcons << available[i];
                }
            }
        }
#endif
        
        // Fallback test icons if QtLucide is not available
        if (testIcons.isEmpty()) {
            testIcons << "heart" << "star" << "home" << "user" << "settings"
                     << "search" << "mail" << "phone" << "camera" << "music";
        }
        
        return testIcons;
    }

    void showExportSuccess(const QStringList& exportedFiles)
    {
        QString message = QString("Export completed successfully!\n\n"
                                "Exported %1 files:\n%2")
                         .arg(exportedFiles.size())
                         .arg(exportedFiles.join("\n"));
        
        QMessageBox::information(this, "Export Successful", message);
    }

    void showImportSuccess(const QStringList& importedTypes)
    {
        QString message = QString("Import completed successfully!\n\n"
                                "Imported data types:\n%1")
                         .arg(importedTypes.join("\n"));
        
        QMessageBox::information(this, "Import Successful", message);
    }

private:
    ThemeManager* m_themeManager;
    BatchExportManager* m_exportManager;
    ExportDialog* m_exportDialog;
    ImportDialog* m_importDialog;
    QTextEdit* m_infoText;
    
#ifdef QTLUCIDE_AVAILABLE
    lucide::QtLucide* m_lucide;
#else
    void* m_lucide; // Placeholder
#endif
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("QtLucide Gallery Export/Import Test");
    app.setApplicationVersion("2.1.0");
    app.setOrganizationName("QtLucide Project");
    app.setApplicationDisplayName("QtLucide Gallery Export/Import");

    ExportImportTestWindow window;
    window.show();

    return app.exec();
}

#include "main_export_import.moc"
