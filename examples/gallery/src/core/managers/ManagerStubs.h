/**
 * @file ManagerStubs.h
 * @brief Stub implementations for manager classes
 * @details This file contains stub implementations for manager classes that will
 *          be fully implemented in the future.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef MANAGER_STUBS_H
#define MANAGER_STUBS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

namespace gallery {

/**
 * @class ImageMetadataManager
 * @brief Stub implementation for image metadata management
 * @details This is a stub class for managing image metadata. Full implementation
 *          will be provided in future versions.
 */
class ImageMetadataManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct ImageMetadataManager
     * @param parent The parent QObject
     */
    explicit ImageMetadataManager(QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ImageMetadataManager() override = default;

    /**
     * @brief Initialize the image metadata manager
     * @param path The path to image metadata
     * @return true if initialization was successful, false otherwise
     */
    bool initialize(const QString &path);

    /**
     * @brief Get metadata for an image
     * @param imageName The image name
     * @return Image metadata as a string, empty if not found
     */
    QString getImageMetadata(const QString &imageName) const;

    /**
     * @brief Get list of all images
     * @return List of all image names
     */
    QStringList getImages() const;

private:
    // Stub member variables
    QStringList m_images;
};

}  // namespace gallery

#endif  // MANAGER_STUBS_H
