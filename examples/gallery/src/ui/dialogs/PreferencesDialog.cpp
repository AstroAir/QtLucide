/**
 * @file PreferencesDialog.cpp
 * @brief Implementation of PreferencesDialog
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#include "PreferencesDialog.h"

#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSettings>
#include <QApplication>

namespace gallery {

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Preferences");
    setMinimumWidth(500);
    setMinimumHeight(400);

    // Set default preferences
    m_currentSettings = {
        2,      // themeMode: System
        48,     // defaultIconSize
        2.0,    // defaultStrokeWidth
        true    // showTooltips
    };
    m_lastSavedSettings = m_currentSettings;

    setupUI();
}

PreferencesDialog::~PreferencesDialog() = default;

void PreferencesDialog::loadSettings()
{
    QSettings settings("QtLucide", "Gallery");

    m_currentSettings.themeMode = settings.value("theme/mode", 2).toInt();
    m_currentSettings.defaultIconSize = settings.value("icons/defaultSize", 48).toInt();
    m_currentSettings.defaultStrokeWidth = settings.value("icons/defaultStrokeWidth", 2.0).toDouble();
    m_currentSettings.showTooltips = settings.value("ui/showTooltips", true).toBool();

    m_lastSavedSettings = m_currentSettings;

    // Update UI
    m_themeComboBox->setCurrentIndex(m_currentSettings.themeMode);
    m_defaultSizeSpinBox->setValue(m_currentSettings.defaultIconSize);
    m_defaultStrokeSpinBox->setValue(m_currentSettings.defaultStrokeWidth);
    m_showTooltipsCheckBox->setChecked(m_currentSettings.showTooltips);
}

void PreferencesDialog::saveSettings()
{
    QSettings settings("QtLucide", "Gallery");

    settings.setValue("theme/mode", m_currentSettings.themeMode);
    settings.setValue("icons/defaultSize", m_currentSettings.defaultIconSize);
    settings.setValue("icons/defaultStrokeWidth", m_currentSettings.defaultStrokeWidth);
    settings.setValue("ui/showTooltips", m_currentSettings.showTooltips);

    settings.sync();
    m_lastSavedSettings = m_currentSettings;
}

PreferencesSettings PreferencesDialog::getPreferences() const
{
    return m_currentSettings;
}

void PreferencesDialog::onThemeChanged(int index)
{
    m_currentSettings.themeMode = index;
}

void PreferencesDialog::onDefaultSizeChanged(int value)
{
    m_currentSettings.defaultIconSize = value;
}

void PreferencesDialog::onDefaultStrokeChanged(double value)
{
    m_currentSettings.defaultStrokeWidth = value;
}

void PreferencesDialog::onTooltipsToggled(bool checked)
{
    m_currentSettings.showTooltips = checked;
}

void PreferencesDialog::onResetToDefaults()
{
    resetToDefaults();
}

void PreferencesDialog::onOkClicked()
{
    saveSettings();
    accept();
}

void PreferencesDialog::onCancelClicked()
{
    // Restore previous settings
    m_currentSettings = m_lastSavedSettings;
    reject();
}

void PreferencesDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Appearance section
    mainLayout->addWidget(createAppearanceSection());

    // Behavior section
    mainLayout->addWidget(createBehaviorSection());

    mainLayout->addStretch();

    // Buttons
    mainLayout->addLayout(createButtonSection());

    setLayout(mainLayout);
}

QGroupBox* PreferencesDialog::createAppearanceSection()
{
    QGroupBox* group = new QGroupBox("Appearance", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    // Theme selection
    QHBoxLayout* themeLayout = new QHBoxLayout();
    themeLayout->addWidget(new QLabel("Theme:", this));

    m_themeComboBox = new QComboBox(this);
    m_themeComboBox->addItems({"Dark", "Light", "System"});
    m_themeComboBox->setCurrentIndex(2);
    connect(m_themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PreferencesDialog::onThemeChanged);
    themeLayout->addWidget(m_themeComboBox);
    themeLayout->addStretch();

    layout->addLayout(themeLayout);

    // Default icon size
    QHBoxLayout* sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(new QLabel("Default Icon Size (pixels):", this));

    m_defaultSizeSpinBox = new QSpinBox(this);
    m_defaultSizeSpinBox->setMinimum(16);
    m_defaultSizeSpinBox->setMaximum(512);
    m_defaultSizeSpinBox->setValue(48);
    connect(m_defaultSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PreferencesDialog::onDefaultSizeChanged);
    sizeLayout->addWidget(m_defaultSizeSpinBox);
    sizeLayout->addStretch();

    layout->addLayout(sizeLayout);

    // Default stroke width
    QHBoxLayout* strokeLayout = new QHBoxLayout();
    strokeLayout->addWidget(new QLabel("Default Stroke Width:", this));

    m_defaultStrokeSpinBox = new QDoubleSpinBox(this);
    m_defaultStrokeSpinBox->setMinimum(0.0);
    m_defaultStrokeSpinBox->setMaximum(10.0);
    m_defaultStrokeSpinBox->setValue(2.0);
    m_defaultStrokeSpinBox->setSingleStep(0.1);
    connect(m_defaultStrokeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PreferencesDialog::onDefaultStrokeChanged);
    strokeLayout->addWidget(m_defaultStrokeSpinBox);
    strokeLayout->addStretch();

    layout->addLayout(strokeLayout);

    group->setLayout(layout);
    return group;
}

QGroupBox* PreferencesDialog::createBehaviorSection()
{
    QGroupBox* group = new QGroupBox("Behavior", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    m_showTooltipsCheckBox = new QCheckBox("Show Tooltips", this);
    m_showTooltipsCheckBox->setChecked(true);
    connect(m_showTooltipsCheckBox, &QCheckBox::toggled,
            this, &PreferencesDialog::onTooltipsToggled);
    layout->addWidget(m_showTooltipsCheckBox);

    group->setLayout(layout);
    return group;
}

QHBoxLayout* PreferencesDialog::createButtonSection()
{
    QHBoxLayout* layout = new QHBoxLayout();

    m_resetButton = new QPushButton("Reset to Defaults", this);
    connect(m_resetButton, &QPushButton::clicked,
            this, &PreferencesDialog::onResetToDefaults);
    layout->addWidget(m_resetButton);

    layout->addStretch();

    m_okButton = new QPushButton("OK", this);
    m_okButton->setMinimumWidth(100);
    connect(m_okButton, &QPushButton::clicked,
            this, &PreferencesDialog::onOkClicked);
    layout->addWidget(m_okButton);

    m_cancelButton = new QPushButton("Cancel", this);
    m_cancelButton->setMinimumWidth(100);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &PreferencesDialog::onCancelClicked);
    layout->addWidget(m_cancelButton);

    return layout;
}

void PreferencesDialog::resetToDefaults()
{
    m_currentSettings.themeMode = 2;
    m_currentSettings.defaultIconSize = 48;
    m_currentSettings.defaultStrokeWidth = 2.0;
    m_currentSettings.showTooltips = true;

    m_themeComboBox->setCurrentIndex(m_currentSettings.themeMode);
    m_defaultSizeSpinBox->setValue(m_currentSettings.defaultIconSize);
    m_defaultStrokeSpinBox->setValue(m_currentSettings.defaultStrokeWidth);
    m_showTooltipsCheckBox->setChecked(m_currentSettings.showTooltips);
}

} // namespace gallery
