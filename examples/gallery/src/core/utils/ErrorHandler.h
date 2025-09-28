/**
 * QtLucide Gallery Application - Error Handler
 *
 * Centralized error handling and validation utilities for the gallery application.
 * Provides consistent error reporting, logging, and user notification mechanisms.
 */

#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <QDebug>
#include <QException>
#include <QFile>
#include <QFileDevice>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <functional>

/**
 * @brief Custom exception class for gallery-specific errors
 */
class GalleryException : public QException {
public:
    explicit GalleryException(const QString& message, const QString& context = QString())
        : m_message(message), m_context(context) {}

    const char* what() const noexcept override { return m_message.toUtf8().constData(); }

    QString message() const { return m_message; }
    QString context() const { return m_context; }

private:
    QString m_message;
    QString m_context;
};

/**
 * @brief Error severity levels
 */
enum class ErrorSeverity { Info, Warning, Error, Critical };

/**
 * @brief Error categories for better organization
 */
enum class ErrorCategory {
    FileSystem,
    Network,
    Validation,
    Resource,
    UI,
    Database,
    Export,
    Import,
    Theme,
    Plugin
};

/**
 * @brief Error information structure
 */
struct ErrorInfo {
    QString message;
    QString details;
    QString context;
    ErrorSeverity severity;
    ErrorCategory category;
    QString timestamp;
    QString sourceFile;
    int sourceLine;

    ErrorInfo() : severity(ErrorSeverity::Error), category(ErrorCategory::UI), sourceLine(0) {}
};

/**
 * @brief Centralized error handler for the gallery application
 */
class ErrorHandler : public QObject {
    Q_OBJECT

public:
    static ErrorHandler* instance();

    // Error reporting methods
    static void reportError(const QString& message, ErrorSeverity severity = ErrorSeverity::Error,
                            ErrorCategory category = ErrorCategory::UI,
                            const QString& context = QString(), const QString& details = QString());

    static void reportException(const GalleryException& exception,
                                ErrorSeverity severity = ErrorSeverity::Error,
                                ErrorCategory category = ErrorCategory::UI);

    static void reportSystemError(const QString& operation, const QString& systemError,
                                  ErrorCategory category = ErrorCategory::FileSystem);

    // User notification methods
    static void showErrorDialog(QWidget* parent, const QString& title, const QString& message,
                                const QString& details = QString());

    static void showWarningDialog(QWidget* parent, const QString& title, const QString& message);

    static void showInfoDialog(QWidget* parent, const QString& title, const QString& message);

    static QMessageBox::StandardButton
    showQuestionDialog(QWidget* parent, const QString& title, const QString& message,
                       QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No);

    // Validation utilities
    static bool validateFilePath(const QString& filePath, QString* errorMessage = nullptr);
    static bool validateDirectoryPath(const QString& dirPath, QString* errorMessage = nullptr);
    static bool validateFileName(const QString& fileName, QString* errorMessage = nullptr);
    static bool validateImageFile(const QString& filePath, QString* errorMessage = nullptr);
    static bool validateIconName(const QString& iconName, QString* errorMessage = nullptr);
    static bool validateExportSettings(const QVariantMap& settings, QStringList* errors = nullptr);

    // Resource management utilities
    static bool ensureDirectoryExists(const QString& dirPath, QString* errorMessage = nullptr);
    static bool checkDiskSpace(const QString& path, qint64 requiredBytes,
                               QString* errorMessage = nullptr);
    static bool checkFilePermissions(const QString& filePath, QFileDevice::Permissions required,
                                     QString* errorMessage = nullptr);

    // Safe operation wrappers
    template <typename Func>
    static bool safeExecute(Func&& func, const QString& operation, QString* errorMessage = nullptr);

    template <typename Func, typename Fallback>
    static auto safeExecuteWithFallback(Func&& func, Fallback&& fallback, const QString& operation)
        -> decltype(func());

    // Error history and logging
    QList<ErrorInfo> getErrorHistory() const { return m_errorHistory; }
    void clearErrorHistory() { m_errorHistory.clear(); }

    // Configuration
    void setMaxHistorySize(int size) { m_maxHistorySize = size; }
    void setShowUserDialogs(bool show) { m_showUserDialogs = show; }
    void setLogToFile(bool log) { m_logToFile = log; }

signals:
    void errorReported(const ErrorInfo& error);
    void criticalErrorReported(const ErrorInfo& error);

private:
    explicit ErrorHandler(QObject* parent = nullptr);

    void addToHistory(const ErrorInfo& error);
    void logError(const ErrorInfo& error);
    QString formatErrorMessage(const ErrorInfo& error) const;

    static ErrorHandler* s_instance;

    QList<ErrorInfo> m_errorHistory;
    int m_maxHistorySize;
    bool m_showUserDialogs;
    bool m_logToFile;
};

// Convenience macros for error reporting with source location
#define GALLERY_REPORT_ERROR(message, severity, category)                                          \
    ErrorHandler::reportError(message, severity, category, QString(), QString())

#define GALLERY_REPORT_ERROR_WITH_CONTEXT(message, context, severity, category)                    \
    ErrorHandler::reportError(message, severity, category, context, QString())

#define GALLERY_REPORT_ERROR_WITH_DETAILS(message, details, severity, category)                    \
    ErrorHandler::reportError(message, severity, category, QString(), details)

// Template implementations
template <typename Func>
bool ErrorHandler::safeExecute(Func&& func, const QString& operation, QString* errorMessage) {
    try {
        func();
        return true;
    } catch (const GalleryException& e) {
        if (errorMessage)
            *errorMessage = e.message();
        reportException(e, ErrorSeverity::Error, ErrorCategory::UI);
        return false;
    } catch (const std::exception& e) {
        QString message = QString("Standard exception in %1: %2").arg(operation).arg(e.what());
        if (errorMessage)
            *errorMessage = message;
        reportError(message, ErrorSeverity::Error, ErrorCategory::UI);
        return false;
    } catch (...) {
        QString message = QString("Unknown exception in %1").arg(operation);
        if (errorMessage)
            *errorMessage = message;
        reportError(message, ErrorSeverity::Critical, ErrorCategory::UI);
        return false;
    }
}

template <typename Func, typename Fallback>
auto ErrorHandler::safeExecuteWithFallback(Func&& func, Fallback&& fallback,
                                           const QString& operation) -> decltype(func()) {
    try {
        return func();
    } catch (const GalleryException& e) {
        reportException(e, ErrorSeverity::Warning, ErrorCategory::UI);
        return fallback();
    } catch (const std::exception& e) {
        QString message = QString("Standard exception in %1: %2").arg(operation).arg(e.what());
        reportError(message, ErrorSeverity::Warning, ErrorCategory::UI);
        return fallback();
    } catch (...) {
        QString message = QString("Unknown exception in %1").arg(operation);
        reportError(message, ErrorSeverity::Error, ErrorCategory::UI);
        return fallback();
    }
}

#endif // ERRORHANDLER_H
