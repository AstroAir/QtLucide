#include "MainWindow.h"
#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextEdit>
#include <QToolBar>
#include <QtLucide/QtLucide.h>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // Create central widget
    QTextEdit* textEdit = new QTextEdit(this);
    setCentralWidget(textEdit);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    setWindowTitle("QtLucide Submodule Example");
    setMinimumSize(600, 400);
    resize(800, 600);

    statusLabel->setText("Ready - QtLucide integrated as submodule");
}

MainWindow::~MainWindow() {}

void MainWindow::newFile() {
    QTextEdit* textEdit = qobject_cast<QTextEdit*>(centralWidget());
    if (textEdit) {
        textEdit->clear();
        statusLabel->setText("New file created");
    }
}

void MainWindow::openFile() {
    statusLabel->setText("Open file clicked - QtLucide icon working!");
}

void MainWindow::saveFile() {
    statusLabel->setText("Save file clicked - QtLucide icon working!");
}

void MainWindow::about() {
    QMessageBox::about(this, "About QtLucide Submodule Example",
                       "This example demonstrates how to use QtLucide as a Git submodule.\n\n"
                       "Features demonstrated:\n"
                       "• QtLucide icons in menus and toolbars\n"
                       "• Automatic submodule detection\n"
                       "• Minimal build configuration\n"
                       "• No installation conflicts\n\n"
                       "QtLucide provides 1634+ beautiful Lucide icons for Qt applications.");
}

void MainWindow::createActions() {
    // Create QtLucide instance
    lucide::QtLucide qtlucide;

    // New action with File Plus icon
    newAct = new QAction("&New", this);
    newAct->setIcon(qtlucide.icon(lucide::Icons::file_plus));
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip("Create a new file");
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    // Open action with Folder Open icon
    openAct = new QAction("&Open...", this);
    openAct->setIcon(qtlucide.icon(lucide::Icons::folder_open));
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip("Open an existing file");
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);

    // Save action with Save icon
    saveAct = new QAction("&Save", this);
    saveAct->setIcon(qtlucide.icon(lucide::Icons::save));
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip("Save the document to disk");
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

    // Exit action with X icon
    exitAct = new QAction("E&xit", this);
    exitAct->setIcon(qtlucide.icon(lucide::Icons::x));
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip("Exit the application");
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    // About action with Info icon
    aboutAct = new QAction("&About", this);
    aboutAct->setIcon(qtlucide.icon(lucide::Icons::info));
    aboutAct->setStatusTip("Show the application's About box");
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars() {
    // Create QtLucide instance
    lucide::QtLucide qtlucide;

    fileToolBar = addToolBar("File");
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();

    // Add some additional icons to showcase QtLucide
    QAction* homeAct = new QAction("Home", this);
    homeAct->setIcon(qtlucide.icon(lucide::Icons::house));
    homeAct->setStatusTip("Go to home");
    fileToolBar->addAction(homeAct);

    QAction* settingsAct = new QAction("Settings", this);
    settingsAct->setIcon(qtlucide.icon(lucide::Icons::settings));
    settingsAct->setStatusTip("Open settings");
    fileToolBar->addAction(settingsAct);

    QAction* searchAct = new QAction("Search", this);
    searchAct->setIcon(qtlucide.icon(lucide::Icons::search));
    searchAct->setStatusTip("Search");
    fileToolBar->addAction(searchAct);
}

void MainWindow::createStatusBar() {
    statusLabel = new QLabel("Ready");
    statusBar()->addWidget(statusLabel);
}
