/**
 * QtLucide Gallery Application - Error Handler Implementation
 */

#include "ErrorHandler.h"
#include "GalleryLogger.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeDatabase>
#include <QMimeType>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStorageInfo>

// Static instance
ErrorHandler* ErrorHandler::s_instance = nullptr;

ErrorHandler::ErrorHandler(QObject* parent)
    : QObject(parent), m_maxHistorySize(1000), m_showUserDialogs(true), m_logToFile(true) {}

ErrorHandler* ErrorHandler::instance() {
    if (!s_instance) {
        s_instance = new ErrorHandler();
    }
    return s_instance;
}

void ErrorHandler::reportError(const QString& message, ErrorSeverity severity,
                               ErrorCategory category, const QString& context,
                               const QString& details) {
    ErrorInfo error;
    error.message = message;
    error.details = details;
    error.context = context;
    error.severity = severity;
    error.category = category;
    error.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

    instance()->addToHistory(error);
    instance()->logError(error);

    // Emit signals
    emit instance() -> errorReported(error);
    if (severity == ErrorSeverity::Critical) {
        emit instance() -> criticalErrorReported(error);
    }
}

void ErrorHandler::reportException(const GalleryException& exception, ErrorSeverity severity,
                                   ErrorCategory category) {
    reportError(exception.message(), severity, category, exception.context());
}

void ErrorHandler::reportSystemError(const QString& operation, const QString& systemError,
                                     ErrorCategory category) {
    QString message = QString("System error during %1: %2").arg(operation).arg(systemError);
    reportError(message, ErrorSeverity::Error, category);
}

void ErrorHandler::showErrorDialog(QWidget* parent, const QString& title, const QString& message,
                                   const QString& details) {
    if (!instance()->m_showUserDialogs)
        return;

    QMessageBox msgBox(parent);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    if (!details.isEmpty()) {
        msgBox.setDetailedText(details);
    }
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void ErrorHandler::showWarningDialog(QWidget* parent, const QString& title,
                                     const QString& message) {
    if (!instance()->m_showUserDialogs)
        return;

    QMessageBox::warning(parent, title, message);
}

void ErrorHandler::showInfoDialog(QWidget* parent, const QString& title, const QString& message) {
    if (!instance()->m_showUserDialogs)
        return;

    QMessageBox::information(parent, title, message);
}

QMessageBox::StandardButton ErrorHandler::showQuestionDialog(QWidget* parent, const QString& title,
                                                             const QString& message,
                                                             QMessageBox::StandardButtons buttons) {
    if (!instance()->m_showUserDialogs)
        return QMessageBox::No;

    return QMessageBox::question(parent, title, message, buttons);
}

bool ErrorHandler::validateFilePath(const QString& filePath, QString* errorMessage) {
    if (filePath.isEmpty()) {
        if (errorMessage)
            *errorMessage = "File path is empty";
        return false;
    }

    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        if (errorMessage)
            *errorMessage = QString("File does not exist: %1").arg(filePath);
        return false;
    }

    if (!fileInfo.isFile()) {
        if (errorMessage)
            *errorMessage = QString("Path is not a file: %1").arg(filePath);
        return false;
    }

    if (!fileInfo.isReadable()) {
        if (errorMessage)
            *errorMessage = QString("File is not readable: %1").arg(filePath);
        return false;
    }

    return true;
}

bool ErrorHandler::validateDirectoryPath(const QString& dirPath, QString* errorMessage) {
    if (dirPath.isEmpty()) {
        if (errorMessage)
            *errorMessage = "Directory path is empty";
        return false;
    }

    QDir dir(dirPath);
    if (!dir.exists()) {
        if (errorMessage)
            *errorMessage = QString("Directory does not exist: %1").arg(dirPath);
        return false;
    }

    QFileInfo dirInfo(dirPath);
    if (!dirInfo.isReadable()) {
        if (errorMessage)
            *errorMessage = QString("Directory is not readable: %1").arg(dirPath);
        return false;
    }

    return true;
}

bool ErrorHandler::validateFileName(const QString& fileName, QString* errorMessage) {
    if (fileName.isEmpty()) {
        if (errorMessage)
            *errorMessage = "File name is empty";
        return false;
    }

    // Check for invalid characters
    QRegularExpression invalidChars(R"([<>:"/\\|?*])");
    if (invalidChars.match(fileName).hasMatch()) {
        if (errorMessage)
            *errorMessage = QString("File name contains invalid characters: %1").arg(fileName);
        return false;
    }

    // Check length
    if (fileName.length() > 255) {
        if (errorMessage)
            *errorMessage = "File name is too long (max 255 characters)";
        return false;
    }

    return true;
}

bool ErrorHandler::validateImageFile(const QString& filePath, QString* errorMessage) {
    if (!validateFilePath(filePath, errorMessage)) {
        return false;
    }

    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(filePath);

    if (!mimeType.name().startsWith("image/")) {
        if (errorMessage)
            *errorMessage = QString("File is not an image: %1").arg(filePath);
        return false;
    }

    // Try to read the image
    QImageReader reader(filePath);
    if (!reader.canRead()) {
        if (errorMessage)
            *errorMessage =
                QString("Cannot read image file: %1 - %2").arg(filePath).arg(reader.errorString());
        return false;
    }

    return true;
}

bool ErrorHandler::validateIconName(const QString& iconName, QString* errorMessage) {
    if (iconName.isEmpty()) {
        if (errorMessage)
            *errorMessage = "Icon name is empty";
        return false;
    }

    // Check for valid icon name pattern (lowercase, hyphens, numbers)
    QRegularExpression validPattern("^[a-z0-9-]+$");
    if (!validPattern.match(iconName).hasMatch()) {
        if (errorMessage)
            *errorMessage = QString("Invalid icon name format: %1").arg(iconName);
        return false;
    }

    return true;
}

bool ErrorHandler::validateExportSettings(const QVariantMap& settings, QStringList* errors) {
    QStringList validationErrors;

    // Check output directory
    QString outputDir = settings.value("outputDirectory").toString();
    if (outputDir.isEmpty()) {
        validationErrors << "Output directory is not specified";
    } else if (!QDir(outputDir).exists()) {
        validationErrors << "Output directory does not exist";
    }

    // Check icon size
    int iconSize = settings.value("iconSize", 0).toInt();
    if (iconSize <= 0 || iconSize > 2048) {
        validationErrors << "Icon size must be between 1 and 2048 pixels";
    }

    // Check format
    QString format = settings.value("format").toString();
    QStringList supportedFormats = {"PNG", "SVG", "JPG", "JPEG", "BMP", "ICO"};
    if (!supportedFormats.contains(format.toUpper())) {
        validationErrors << QString("Unsupported export format: %1").arg(format);
    }

    if (errors)
        *errors = validationErrors;
    return validationErrors.isEmpty();
}

bool ErrorHandler::ensureDirectoryExists(const QString& dirPath, QString* errorMessage) {
    QDir dir;
    if (!dir.mkpath(dirPath)) {
        if (errorMessage)
            *errorMessage = QString("Failed to create directory: %1").arg(dirPath);
        return false;
    }
    return true;
}

bool ErrorHandler::checkDiskSpace(const QString& path, qint64 requiredBytes,
                                  QString* errorMessage) {
    QStorageInfo storage(path);
    if (!storage.isValid()) {
        if (errorMessage)
            *errorMessage = QString("Cannot access storage information for: %1").arg(path);
        return false;
    }

    if (storage.bytesAvailable() < requiredBytes) {
        if (errorMessage) {
            *errorMessage = QString("Insufficient disk space. Required: %1 MB, Available: %2 MB")
                                .arg(requiredBytes / 1024 / 1024)
                                .arg(storage.bytesAvailable() / 1024 / 1024);
        }
        return false;
    }

    return true;
}

bool ErrorHandler::checkFilePermissions(const QString& filePath, QFileDevice::Permissions required,
                                        QString* errorMessage) {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        if (errorMessage)
            *errorMessage = QString("File does not exist: %1").arg(filePath);
        return false;
    }

    QFile::Permissions current = fileInfo.permissions();
    if ((current & required) != required) {
        if (errorMessage)
            *errorMessage = QString("Insufficient file permissions: %1").arg(filePath);
        return false;
    }

    return true;
}

void ErrorHandler::addToHistory(const ErrorInfo& error) {
    m_errorHistory.append(error);

    // Limit history size
    while (m_errorHistory.size() > m_maxHistorySize) {
        m_errorHistory.removeFirst();
    }
}

void ErrorHandler::logError(const ErrorInfo& error) {
    if (!m_logToFile)
        return;

    QString logMessage = formatErrorMessage(error);

    // Use the gallery logger if available
    switch (error.severity) {
        case ErrorSeverity::Info:
            GALLERY_LOG_INFO(galleryMain, logMessage);
            break;
        case ErrorSeverity::Warning:
            GALLERY_LOG_WARNING(galleryMain, logMessage);
            break;
        case ErrorSeverity::Error:
            GALLERY_LOG_ERROR(galleryMain, logMessage);
            break;
        case ErrorSeverity::Critical:
            GALLERY_LOG_CRITICAL(galleryMain, logMessage);
            break;
    }
}

QString ErrorHandler::formatErrorMessage(const ErrorInfo& error) const {
    QString message = QString("[%1] %2").arg(error.timestamp).arg(error.message);

    if (!error.context.isEmpty()) {
        message += QString(" (Context: %1)").arg(error.context);
    }

    if (!error.details.isEmpty()) {
        message += QString(" - Details: %1").arg(error.details);
    }

    return message;
}
