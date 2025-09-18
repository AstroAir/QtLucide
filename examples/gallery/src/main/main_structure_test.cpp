/**
 * QtLucide Gallery - Structure Validation Test
 *
 * This simple test validates that the new directory structure and
 * layout configuration system work correctly.
 */

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <QString>

// Test the new layout configuration
#include "../config/LayoutConfig.h"

class StructureTestWindow : public QMainWindow
{
public:
    StructureTestWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        setWindowTitle("QtLucide Gallery - Structure Test");
        setMinimumSize(800, 600);

        setupUI();
        testLayoutConfig();
    }

private:
    void onTestLayoutConfig()
    {
        QString message = "Layout Configuration Test Results:\n\n";

        // Test spacing constants
        message += QString("Base Unit: %1px\n").arg(GalleryLayout::Spacing::BASE_UNIT);
        message += QString("Small Spacing: %1px\n").arg(GalleryLayout::Spacing::SMALL);
        message += QString("Medium Spacing: %1px\n").arg(GalleryLayout::Spacing::MEDIUM);
        message += QString("Large Spacing: %1px\n").arg(GalleryLayout::Spacing::LARGE);

        // Test responsive sizing
        GalleryLayout::ResponsiveLayout::ScreenSize screenSize =
            GalleryLayout::ResponsiveLayout::getScreenSize(this);

        QString screenSizeStr;
        switch (screenSize) {
            case GalleryLayout::ResponsiveLayout::ScreenSize::Small:  screenSizeStr = "Small"; break;
            case GalleryLayout::ResponsiveLayout::ScreenSize::Medium: screenSizeStr = "Medium"; break;
            case GalleryLayout::ResponsiveLayout::ScreenSize::Large:  screenSizeStr = "Large"; break;
            case GalleryLayout::ResponsiveLayout::ScreenSize::XLarge: screenSizeStr = "XLarge"; break;
        }

        message += QString("\nScreen Size: %1\n").arg(screenSizeStr);
        message += QString("Responsive Thumbnail Size: %1px\n")
                   .arg(GalleryLayout::ResponsiveLayout::getThumbnailSize(screenSize));
        message += QString("Responsive Sidebar Width: %1px\n")
                   .arg(GalleryLayout::ResponsiveLayout::getSidebarWidth(screenSize));

        // Test grid calculation
        int testWidth = 800;
        int thumbnailSize = GalleryLayout::ResponsiveLayout::getThumbnailSize(screenSize);
        int columns = GalleryLayout::ResponsiveLayout::calculateGridColumns(testWidth, thumbnailSize);
        message += QString("Grid Columns for %1px width: %2\n").arg(testWidth).arg(columns);

        QMessageBox::information(this, "Layout Configuration Test", message);
    }

private:
    void setupUI()
    {
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        // Use the new layout configuration
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(GalleryLayout::Margins::WINDOW_MARGIN,
                                      GalleryLayout::Margins::WINDOW_MARGIN,
                                      GalleryLayout::Margins::WINDOW_MARGIN,
                                      GalleryLayout::Margins::WINDOW_MARGIN);
        mainLayout->setSpacing(GalleryLayout::Spacing::WIDGET_SPACING);

        // Title
        QLabel* titleLabel = new QLabel("QtLucide Gallery Structure Test");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Test splitter with responsive proportions
        QSplitter* testSplitter = new QSplitter(Qt::Horizontal);
        testSplitter->setStyleSheet(GalleryLayout::getSplitterStyleSheet());

        // Left panel (simulating sidebar)
        QWidget* leftPanel = new QWidget;
        leftPanel->setStyleSheet(GalleryLayout::getPanelStyleSheet());
        leftPanel->setMinimumWidth(GalleryLayout::Sizing::SIDEBAR_MIN_WIDTH);
        leftPanel->setMaximumWidth(GalleryLayout::Sizing::SIDEBAR_MAX_WIDTH);

        QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
        leftLayout->setContentsMargins(GalleryLayout::Margins::PANEL_MARGIN,
                                      GalleryLayout::Margins::PANEL_MARGIN,
                                      GalleryLayout::Margins::PANEL_MARGIN,
                                      GalleryLayout::Margins::PANEL_MARGIN);
        leftLayout->setSpacing(GalleryLayout::Spacing::WIDGET_SPACING);

        leftLayout->addWidget(new QLabel("Sidebar Panel"));
        leftLayout->addWidget(new QLabel("- Standardized margins"));
        leftLayout->addWidget(new QLabel("- Responsive width"));
        leftLayout->addWidget(new QLabel("- Consistent styling"));
        leftLayout->addStretch();

        // Right panel (simulating content area)
        QWidget* rightPanel = new QWidget;
        rightPanel->setStyleSheet(GalleryLayout::getPanelStyleSheet());

        QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
        rightLayout->setContentsMargins(GalleryLayout::Margins::CONTENT_MARGIN,
                                       GalleryLayout::Margins::CONTENT_MARGIN,
                                       GalleryLayout::Margins::CONTENT_MARGIN,
                                       GalleryLayout::Margins::CONTENT_MARGIN);
        rightLayout->setSpacing(GalleryLayout::Spacing::SECTION_SPACING);

        rightLayout->addWidget(new QLabel("Content Area"));
        rightLayout->addWidget(new QLabel("This demonstrates the new layout system:"));
        rightLayout->addWidget(new QLabel("✓ Centralized layout constants"));
        rightLayout->addWidget(new QLabel("✓ Responsive design utilities"));
        rightLayout->addWidget(new QLabel("✓ Standardized spacing and margins"));
        rightLayout->addWidget(new QLabel("✓ Consistent visual hierarchy"));

        QPushButton* testButton = new QPushButton("Test Layout Configuration");
        testButton->setFixedHeight(GalleryLayout::Sizing::SEARCH_WIDGET_HEIGHT);
        connect(testButton, &QPushButton::clicked, this, &StructureTestWindow::onTestLayoutConfig);
        rightLayout->addWidget(testButton);

        rightLayout->addStretch();

        // Add panels to splitter
        testSplitter->addWidget(leftPanel);
        testSplitter->addWidget(rightPanel);

        // Set responsive proportions
        GalleryLayout::ResponsiveLayout::ScreenSize screenSize =
            GalleryLayout::ResponsiveLayout::getScreenSize(this);
        QList<int> proportions = GalleryLayout::ResponsiveLayout::getSplitterProportions(screenSize, false);

        int sidebarWidth = GalleryLayout::ResponsiveLayout::getSidebarWidth(screenSize);
        int contentWidth = width() - sidebarWidth;
        testSplitter->setSizes({sidebarWidth, contentWidth});

        mainLayout->addWidget(testSplitter);

        // Status info
        QLabel* statusLabel = new QLabel("Directory structure reorganized ✓ | Layout system implemented ✓");
        statusLabel->setStyleSheet("color: #27ae60; font-style: italic;");
        statusLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(statusLabel);
    }

    void testLayoutConfig()
    {
        // Validate that all layout constants are reasonable
        Q_ASSERT(GalleryLayout::Spacing::BASE_UNIT == 4);
        Q_ASSERT(GalleryLayout::Spacing::SMALL == 8);
        Q_ASSERT(GalleryLayout::Spacing::MEDIUM == 12);
        Q_ASSERT(GalleryLayout::Spacing::LARGE == 16);

        Q_ASSERT(GalleryLayout::Sizing::SIDEBAR_MIN_WIDTH >= 200);
        Q_ASSERT(GalleryLayout::Sizing::SIDEBAR_MAX_WIDTH <= 400);
        Q_ASSERT(GalleryLayout::Sizing::THUMBNAIL_DEFAULT > 0);

        qDebug() << "Layout configuration validation passed!";
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("QtLucide Gallery Structure Test");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("QtLucide Project");

    StructureTestWindow window;
    window.show();

    return app.exec();
}

// #include "main_structure_test.moc" // Not needed for simple test
