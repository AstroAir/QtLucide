/**
 * Export Dialog Tests
 */

#ifndef TEST_EXPORT_DIALOG_H
#define TEST_EXPORT_DIALOG_H

#include <QObject>
#include <QtTest/QtTest>
#include "ui/dialogs/ExportDialog.h"

class TestExportDialog : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testDialogCreation();
    void testExportSettings();
    void testDialogAcceptReject();

private:
    ExportDialog* m_exportDialog;
};

#endif // TEST_EXPORT_DIALOG_H
