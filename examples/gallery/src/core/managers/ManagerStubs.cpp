/**
 * @file ManagerStubs.cpp
 * @brief Implementation of manager stub classes
 */

#include "ManagerStubs.h"

#include <QDebug>

namespace gallery {

ImageMetadataManager::ImageMetadataManager(QObject *parent)
    : QObject(parent) {
}

bool ImageMetadataManager::initialize(const QString &path) {
    // Stub implementation
    qDebug() << "ImageMetadataManager::initialize() called with path:" << path;
    return true;
}

QString ImageMetadataManager::getImageMetadata(const QString &imageName) const {
    // Stub implementation
    qDebug() << "ImageMetadataManager::getImageMetadata() called for:" << imageName;
    return QString();
}

QStringList ImageMetadataManager::getImages() const {
    // Stub implementation
    return m_images;
}

}  // namespace gallery
