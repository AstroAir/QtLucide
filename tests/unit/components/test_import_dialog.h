/**
 * Import Dialog Tests
 */

#ifndef TEST_IMPORT_DIALOG_H
#define TEST_IMPORT_DIALOG_H

#include "ui/dialogs/ImportDialog.h"
#include <QObject>
#include <QtTest/QtTest>

class TestImportDialog : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testDialogCreation();
    void testImportSettings();
    void testDialogAcceptReject();

private:
    ImportDialog* m_importDialog;
};

#endif // TEST_IMPORT_DIALOG_H
