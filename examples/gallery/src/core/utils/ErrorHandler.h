/**
 * @file ErrorHandler.h
 * @brief Error handling utility for the QtLucide gallery application
 * @details This file provides centralized error handling with user-friendly messages
 *          and signal-based error reporting for the gallery application.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef GALLERY_ERRORHANDLER_H
#define GALLERY_ERRORHANDLER_H

#include <QObject>
#include <QString>

namespace gallery {

/**
 * @brief Error type enumeration
 * @details Defines different categories of errors that can occur in the application
 */
enum class ErrorType {
    FileNotFound = 0,     ///< Required file or resource was not found
    FileReadError = 1,    ///< Error reading from file
    FileWriteError = 2,   ///< Error writing to file
    PermissionDenied = 3, ///< Insufficient permissions for operation
    InvalidFormat = 4,    ///< Data in invalid format
    CorruptedData = 5,    ///< Data is corrupted or incomplete
    OutOfMemory = 6,      ///< Not enough memory for operation
    OperationFailed = 7,  ///< Generic operation failure
    InvalidInput = 8,     ///< User provided invalid input
    NotInitialized = 9,   ///< Required component not initialized
    AlreadyExists = 10,   ///< Resource already exists
    Timeout = 11,         ///< Operation timed out
    Unknown = 12          ///< Unknown error
};

/**
 * @brief Error severity enumeration
 * @details Defines the severity level of different errors
 */
enum class ErrorSeverity {
    Info = 0,    ///< Informational message (not critical)
    Warning = 1, ///< Warning (operation may have issues)
    Error = 2,   ///< Error (operation failed)
    Critical = 3 ///< Critical error (application integrity at risk)
};

/**
 * @brief Error information structure
 * @details Contains all details about an error that occurred
 */
struct ErrorInfo {
    /**
     * @brief The type of error
     */
    ErrorType type{ErrorType::Unknown};

    /**
     * @brief The severity level
     */
    ErrorSeverity severity{ErrorSeverity::Error};

    /**
     * @brief User-friendly error message
     * @details Message suitable for displaying to end users
     */
    QString userMessage;

    /**
     * @brief Detailed technical error message
     * @details Technical details for debugging (shown in logs)
     */
    QString technicalMessage;

    /**
     * @brief Suggested recovery action
     * @details What the user can do to recover from this error
     */
    QString suggestedAction;

    /**
     * @brief Error code for internal use
     */
    int errorCode{0};

    /**
     * @brief Additional context data
     * @details Extra information related to the error
     */
    QString context;

    /**
     * @brief Equality comparison operator
     * @param other The other ErrorInfo to compare with
     * @return true if all properties are equal, false otherwise
     */
    bool operator==(const ErrorInfo& other) const noexcept {
        return type == other.type && severity == other.severity &&
               userMessage == other.userMessage && technicalMessage == other.technicalMessage &&
               suggestedAction == other.suggestedAction && errorCode == other.errorCode &&
               context == other.context;
    }

    /**
     * @brief Inequality comparison operator
     * @param other The other ErrorInfo to compare with
     * @return true if any property differs, false otherwise
     */
    bool operator!=(const ErrorInfo& other) const noexcept { return !(*this == other); }
};

/**
 * @brief Central error handler for the gallery application
 * @details Provides error handling with user-friendly messages and signal-based
 *          error reporting. Errors are logged automatically for debugging.
 * @ingroup Gallery
 *
 * @par Usage:
 * @code
 * ErrorHandler handler;
 * connect(&handler, &ErrorHandler::errorOccurred,
 *         this, &MyClass::onError);
 *
 * ErrorInfo error;
 * error.type = ErrorType::FileNotFound;
 * error.userMessage = "Icon file not found";
 * error.technicalMessage = "File: /path/to/icon.svg";
 * handler.reportError(error);
 * @endcode
 */
class ErrorHandler : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs an ErrorHandler instance
     * @param parent Parent QObject for memory management
     */
    explicit ErrorHandler(QObject* parent = nullptr);

    /**
     * @brief Destroys the ErrorHandler instance
     */
    ~ErrorHandler() override;

    /**
     * @brief Report an error with full information
     * @param error The ErrorInfo structure containing error details
     * @details Reports an error to all connected receivers and logs it for debugging
     *
     * @example
     * @code
     * ErrorInfo error;
     * error.type = ErrorType::FileNotFound;
     * error.severity = ErrorSeverity::Error;
     * error.userMessage = "Could not find the requested file";
     * error.technicalMessage = "File not found: /home/user/icons.svg";
     * error.suggestedAction = "Check the file path and try again";
     * handler.reportError(error);
     * @endcode
     */
    void reportError(const ErrorInfo& error);

    /**
     * @brief Report an error with minimal information
     * @param type The error type
     * @param userMessage User-friendly error message
     * @param technicalMessage Optional technical details
     * @details Convenience method for reporting simple errors
     *
     * @example
     * @code
     * handler.reportError(ErrorType::OperationFailed,
     *                     "Export failed",
     *                     "Failed to write to disk: No space available");
     * @endcode
     */
    void reportError(ErrorType type, const QString& userMessage,
                     const QString& technicalMessage = QString());

    /**
     * @brief Report a file-related error
     * @param type File operation error type
     * @param filePath Path to the file that caused the error
     * @param details Additional error details
     * @details Specialized method for reporting file operation errors
     *
     * @example
     * @code
     * handler.reportFileError(ErrorType::FileReadError,
     *                         "/home/user/config.ini",
     *                         "Permission denied");
     * @endcode
     */
    void reportFileError(ErrorType type, const QString& filePath,
                         const QString& details = QString());

    /**
     * @brief Get the last reported error
     * @return The ErrorInfo of the last error, or empty ErrorInfo if no error
     *
     * @example
     * @code
     * ErrorInfo lastError = handler.lastError();
     * if (lastError.type != ErrorType::Unknown) {
     *     qDebug() << lastError.userMessage;
     * }
     * @endcode
     */
    [[nodiscard]] ErrorInfo lastError() const;

    /**
     * @brief Check if an error has been reported
     * @return true if an error has been reported, false otherwise
     *
     * @example
     * @code
     * if (handler.hasError()) {
     *     qDebug() << "Error occurred:" << handler.lastError().userMessage;
     * }
     * @endcode
     */
    [[nodiscard]] bool hasError() const;

    /**
     * @brief Clear the last error information
     * @details Resets the error state
     *
     * @example
     * @code
     * handler.clearError();
     * @endcode
     */
    void clearError();

    /**
     * @brief Get a user-friendly message for an error type
     * @param type The error type
     * @return A localized, user-friendly message
     * @details Provides consistent error messages for different error types
     *
     * @example
     * @code
     * QString msg = ErrorHandler::getUserMessage(ErrorType::FileNotFound);
     * // Returns something like: "The requested file could not be found"
     * @endcode
     */
    static QString getUserMessage(ErrorType type);

    /**
     * @brief Get an error type from an error description
     * @param description Error description string
     * @return The matching ErrorType, or ErrorType::Unknown if no match
     *
     * @example
     * @code
     * ErrorType type = ErrorHandler::errorTypeFromDescription("file not found");
     * @endcode
     */
    static ErrorType errorTypeFromDescription(const QString& description);

Q_SIGNALS:
    /**
     * @brief Emitted when an error occurs
     * @param error The ErrorInfo containing error details
     * @details This signal is emitted whenever reportError() is called,
     *          allowing UI elements to update and inform the user
     *
     * @see reportError()
     */
    void errorOccurred(const ErrorInfo& error);

    /**
     * @brief Emitted when a critical error occurs
     * @param message User-friendly error message
     * @details Emitted specifically for critical errors that require
     *          immediate user attention or may cause application shutdown
     *
     * @see errorOccurred()
     */
    void criticalErrorOccurred(const QString& message);

    /**
     * @brief Emitted when an error is cleared
     * @details Emitted after clearError() is called
     *
     * @see clearError()
     */
    void errorCleared();

private:
    /**
     * @brief Log an error for debugging
     * @param error The error information to log
     */
    void logError(const ErrorInfo& error);

    /**
     * @brief Convert error type to string
     * @param type The error type
     * @return String representation of the error type
     */
    static QString errorTypeToString(ErrorType type);

    ErrorInfo m_lastError;  ///< Information about the last reported error
    bool m_hasError{false}; ///< Whether an error is currently pending
};

} // namespace gallery

#endif // GALLERY_ERRORHANDLER_H
