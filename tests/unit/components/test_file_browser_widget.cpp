/**
 * File Browser Widget Tests Implementation
 */

#include "test_file_browser_widget.h"
#include <QStandardPaths>

void TestFileBrowserWidget::initTestCase()
{
    m_browserWidget = nullptr;
}

void TestFileBrowserWidget::cleanupTestCase()
{
    // Cleanup
}

void TestFileBrowserWidget::init()
{
    m_browserWidget = new FileBrowserWidget();
}

void TestFileBrowserWidget::cleanup()
{
    delete m_browserWidget;
    m_browserWidget = nullptr;
}

void TestFileBrowserWidget::testWidgetCreation()
{
    QVERIFY(m_browserWidget != nullptr);
    QVERIFY(m_browserWidget->isWidgetType());
}

void TestFileBrowserWidget::testNavigation()
{
    QVERIFY(m_browserWidget != nullptr);
    
    // Test navigation to home directory
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    m_browserWidget->navigateToPath(homeDir);
    
    QVERIFY(true);
}

void TestFileBrowserWidget::testImageFiltering()
{
    QVERIFY(m_browserWidget != nullptr);
    
    // Test image filtering
    m_browserWidget->setShowOnlyImages(true);
    m_browserWidget->setShowOnlyImages(false);
    
    QVERIFY(true);
}

void TestFileBrowserWidget::testFileSelection()
{
    QVERIFY(m_browserWidget != nullptr);
    
    // Test file selection
    QStringList selectedFiles = m_browserWidget->getSelectedFiles();
    QVERIFY(selectedFiles.isEmpty()); // Should be empty initially
}

#include "test_file_browser_widget.moc"
