/**
 * @file ErrorHandler.cpp
 * @brief Implementation of the ErrorHandler utility class
 * @details Provides centralized error handling for the gallery application
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "ErrorHandler.h"
#include "GalleryLogger.h"

#include <QDebug>

namespace gallery {

ErrorHandler::ErrorHandler(QObject* parent) : QObject(parent) {
    GalleryLogger::debug("error_handler", "ErrorHandler instance created");
}

ErrorHandler::~ErrorHandler() {
    GalleryLogger::debug("error_handler", "ErrorHandler instance destroyed");
}

void ErrorHandler::reportError(const ErrorInfo& error) {
    m_lastError = error;
    m_hasError = true;

    // Log the error
    logError(error);

    // Emit the signal
    Q_EMIT errorOccurred(error);

    // Emit critical signal if applicable
    if (error.severity == ErrorSeverity::Critical) {
        Q_EMIT criticalErrorOccurred(error.userMessage);
    }
}

void ErrorHandler::reportError(ErrorType type, const QString& userMessage,
                               const QString& technicalMessage) {
    ErrorInfo error;
    error.type = type;
    error.userMessage = userMessage;
    error.technicalMessage = technicalMessage;
    error.severity = (type == ErrorType::OutOfMemory || type == ErrorType::Unknown)
                         ? ErrorSeverity::Critical
                         : ErrorSeverity::Error;

    reportError(error);
}

void ErrorHandler::reportFileError(ErrorType type, const QString& filePath,
                                   const QString& details) {
    ErrorInfo error;
    error.type = type;
    error.context = filePath;

    QString typeStr = errorTypeToString(type);

    switch (type) {
        case ErrorType::FileNotFound:
            error.userMessage = "File not found";
            error.technicalMessage = QString("File '%1' does not exist").arg(filePath);
            error.suggestedAction = "Check the file path and try again";
            break;

        case ErrorType::FileReadError:
            error.userMessage = "Cannot read file";
            error.technicalMessage = QString("Failed to read file '%1': %2").arg(filePath, details);
            error.suggestedAction = "Check file permissions and try again";
            break;

        case ErrorType::FileWriteError:
            error.userMessage = "Cannot write file";
            error.technicalMessage =
                QString("Failed to write to file '%1': %2").arg(filePath, details);
            error.suggestedAction = "Check disk space and file permissions";
            break;

        case ErrorType::PermissionDenied:
            error.userMessage = "Permission denied";
            error.technicalMessage = QString("Permission denied accessing '%1'").arg(filePath);
            error.suggestedAction = "Check file permissions and try again";
            break;

        default:
            error.userMessage = QString("File operation failed: %1").arg(typeStr);
            error.technicalMessage =
                QString("File operation failed for '%1': %2").arg(filePath, details);
            break;
    }

    error.severity = ErrorSeverity::Error;
    reportError(error);
}

ErrorInfo ErrorHandler::lastError() const {
    return m_lastError;
}

bool ErrorHandler::hasError() const {
    return m_hasError;
}

void ErrorHandler::clearError() {
    if (m_hasError) {
        m_hasError = false;
        m_lastError = ErrorInfo();
        GalleryLogger::debug("error_handler", "Error cleared");
        Q_EMIT errorCleared();
    }
}

QString ErrorHandler::getUserMessage(ErrorType type) {
    switch (type) {
        case ErrorType::FileNotFound:
            return "The requested file could not be found";

        case ErrorType::FileReadError:
            return "Error reading file. Please check the file and try again";

        case ErrorType::FileWriteError:
            return "Error writing file. Check disk space and permissions";

        case ErrorType::PermissionDenied:
            return "Permission denied. You do not have sufficient privileges";

        case ErrorType::InvalidFormat:
            return "The file format is invalid or not supported";

        case ErrorType::CorruptedData:
            return "Data is corrupted. Please try using a valid file";

        case ErrorType::OutOfMemory:
            return "Not enough memory to complete the operation";

        case ErrorType::OperationFailed:
            return "The operation failed. Please try again";

        case ErrorType::InvalidInput:
            return "Invalid input provided. Please check your input and try again";

        case ErrorType::NotInitialized:
            return "A required component has not been initialized";

        case ErrorType::AlreadyExists:
            return "This item already exists";

        case ErrorType::Timeout:
            return "The operation timed out. Please try again";

        case ErrorType::Unknown:
            return "An unknown error occurred";

        default:
            return "An unexpected error occurred";
    }
}

ErrorType ErrorHandler::errorTypeFromDescription(const QString& description) {
    QString lower = description.toLower();

    if (lower.contains("not found") || lower.contains("no such")) {
        return ErrorType::FileNotFound;
    } else if (lower.contains("read") && lower.contains("error")) {
        return ErrorType::FileReadError;
    } else if (lower.contains("write") && lower.contains("error")) {
        return ErrorType::FileWriteError;
    } else if (lower.contains("permission")) {
        return ErrorType::PermissionDenied;
    } else if (lower.contains("format")) {
        return ErrorType::InvalidFormat;
    } else if (lower.contains("corrupt")) {
        return ErrorType::CorruptedData;
    } else if (lower.contains("memory")) {
        return ErrorType::OutOfMemory;
    } else if (lower.contains("timeout")) {
        return ErrorType::Timeout;
    } else if (lower.contains("invalid")) {
        return ErrorType::InvalidInput;
    } else if (lower.contains("exists")) {
        return ErrorType::AlreadyExists;
    }

    return ErrorType::Unknown;
}

void ErrorHandler::logError(const ErrorInfo& error) {
    LogLevel level;

    switch (error.severity) {
        case ErrorSeverity::Info:
            level = LogLevel::Info;
            break;
        case ErrorSeverity::Warning:
            level = LogLevel::Warning;
            break;
        case ErrorSeverity::Error:
            level = LogLevel::Error;
            break;
        case ErrorSeverity::Critical:
            level = LogLevel::Critical;
            break;
        default:
            level = LogLevel::Error;
            break;
    }

    QString typeStr = errorTypeToString(error.type);
    QString logMessage = QString("[%1] %2").arg(typeStr, error.technicalMessage);

    if (!error.context.isEmpty()) {
        logMessage += QString(" (Context: %1)").arg(error.context);
    }

    GalleryLogger::log(level, "error_handler", logMessage);
}

QString ErrorHandler::errorTypeToString(ErrorType type) {
    switch (type) {
        case ErrorType::FileNotFound:
            return "FileNotFound";
        case ErrorType::FileReadError:
            return "FileReadError";
        case ErrorType::FileWriteError:
            return "FileWriteError";
        case ErrorType::PermissionDenied:
            return "PermissionDenied";
        case ErrorType::InvalidFormat:
            return "InvalidFormat";
        case ErrorType::CorruptedData:
            return "CorruptedData";
        case ErrorType::OutOfMemory:
            return "OutOfMemory";
        case ErrorType::OperationFailed:
            return "OperationFailed";
        case ErrorType::InvalidInput:
            return "InvalidInput";
        case ErrorType::NotInitialized:
            return "NotInitialized";
        case ErrorType::AlreadyExists:
            return "AlreadyExists";
        case ErrorType::Timeout:
            return "Timeout";
        case ErrorType::Unknown:
            return "Unknown";
        default:
            return "UnknownError";
    }
}

} // namespace gallery
