#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMenuBar;
class QStatusBar;
class QToolBar;
class QLabel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void about();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    QMenu *fileMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;
    QAction *aboutAct;
    QLabel *statusLabel;
};
