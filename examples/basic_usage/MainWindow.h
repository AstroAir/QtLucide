/**
 * QtLucide Basic Usage Example - Main Window Header
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QColorDialog>
#include <QGroupBox>
#include <QScrollArea>
#include <QLineEdit>
#include <QCheckBox>

#include <QtLucide/QtLucide.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onColorButtonClicked();
    void onIconSizeChanged(int size);
    void onScaleFactorChanged(double factor);
    void onSearchTextChanged(const QString& text);
    void onShowAllIconsToggled(bool checked);
    void createIconGrid();

private:
    void setupUI();
    void setupControls();
    void updateIconColors();
    void validateIconNames();

    lucide::QtLucide* m_lucide;

    // UI Components
    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;
    QGroupBox* m_controlsGroup;
    QGroupBox* m_iconsGroup;
    QScrollArea* m_scrollArea;
    QWidget* m_iconContainer;
    QGridLayout* m_iconLayout;

    // Controls
    QPushButton* m_colorButton;
    QSpinBox* m_sizeSpinBox;
    QSpinBox* m_scaleSpinBox;
    QLineEdit* m_searchEdit;
    QCheckBox* m_showAllCheckBox;

    // Settings
    QColor m_currentColor;
    int m_iconSize;
    double m_scaleFactor;

    // Sample icons to display
    QStringList m_sampleIcons;
    QStringList m_filteredIcons;
    QString m_searchFilter;
    bool m_showAllIcons;
};

#endif // MAINWINDOW_H
