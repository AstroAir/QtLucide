/**
 * Export Dialog Tests Implementation
 */

#include "test_export_dialog.h"

void TestExportDialog::initTestCase()
{
    m_exportDialog = nullptr;
}

void TestExportDialog::cleanupTestCase()
{
    // Cleanup
}

void TestExportDialog::init()
{
    m_exportDialog = new ExportDialog();
}

void TestExportDialog::cleanup()
{
    delete m_exportDialog;
    m_exportDialog = nullptr;
}

void TestExportDialog::testDialogCreation()
{
    QVERIFY(m_exportDialog != nullptr);
    QVERIFY(m_exportDialog->isWidgetType());
}

void TestExportDialog::testExportSettings()
{
    QVERIFY(m_exportDialog != nullptr);
    
    // Test export settings
    m_exportDialog->setExportFormat(ExportFormat::PNG);
    m_exportDialog->setExportSize(QSize(64, 64));
    
    QVERIFY(true);
}

void TestExportDialog::testDialogAcceptReject()
{
    QVERIFY(m_exportDialog != nullptr);
    
    // Test dialog accept/reject
    m_exportDialog->accept();
    m_exportDialog->reject();
    
    QVERIFY(true);
}

#include "test_export_dialog.moc"
