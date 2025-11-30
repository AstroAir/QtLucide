/**
 * @file IconItem.cpp
 * @brief Implementation of the IconItem model class
 * @details Provides the icon data model functionality for the gallery
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "IconItem.h"

#include <QDateTime>

namespace gallery {

IconItem::IconItem()
    : m_createdAt(QDateTime::currentDateTime()), m_modifiedAt(QDateTime::currentDateTime()) {}

IconItem::IconItem(const QString& name)
    : m_name(name), m_createdAt(QDateTime::currentDateTime()),
      m_modifiedAt(QDateTime::currentDateTime()) {}

IconItem::IconItem(const QString& name, const QString& category, const QStringList& tags)
    : m_name(name), m_category(category), m_tags(tags), m_createdAt(QDateTime::currentDateTime()),
      m_modifiedAt(QDateTime::currentDateTime()) {
    if (!category.isEmpty()) {
        m_categories << category;
    }
}

QString IconItem::name() const {
    return m_name;
}

QString IconItem::description() const {
    return m_description;
}

QString IconItem::category() const {
    return m_category;
}

QStringList IconItem::categories() const {
    return m_categories;
}

QStringList IconItem::tags() const {
    return m_tags;
}

bool IconItem::isFavorite() const {
    return m_isFavorite;
}

QDateTime IconItem::createdAt() const {
    return m_createdAt;
}

QDateTime IconItem::modifiedAt() const {
    return m_modifiedAt;
}

int IconItem::usageCount() const {
    return m_usageCount;
}

QString IconItem::metadata(const QString& key) const {
    return m_metadata.value(key, QString());
}

void IconItem::setName(const QString& name) {
    if (m_name != name) {
        m_name = name;
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

void IconItem::setDescription(const QString& description) {
    if (m_description != description) {
        m_description = description;
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

void IconItem::setCategory(const QString& category) {
    if (m_category != category) {
        m_category = category;
        if (!category.isEmpty() && !m_categories.contains(category)) {
            m_categories << category;
        }
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

void IconItem::setCategories(const QStringList& categories) {
    if (m_categories != categories) {
        m_categories = categories;
        if (!categories.isEmpty() && m_category.isEmpty()) {
            m_category = categories.first();
        }
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

void IconItem::setTags(const QStringList& tags) {
    if (m_tags != tags) {
        m_tags = tags;
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

void IconItem::addTag(const QString& tag) {
    if (!tag.isEmpty() && !m_tags.contains(tag)) {
        m_tags << tag;
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

void IconItem::removeTag(const QString& tag) {
    if (m_tags.removeAll(tag) > 0) {
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

bool IconItem::hasTag(const QString& tag) const {
    return m_tags.contains(tag);
}

void IconItem::setFavorite(bool isFavorite) {
    if (m_isFavorite != isFavorite) {
        m_isFavorite = isFavorite;
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

void IconItem::incrementUsageCount(int count) {
    if (count > 0) {
        m_usageCount += count;
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

void IconItem::resetUsageCount() {
    if (m_usageCount != 0) {
        m_usageCount = 0;
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

void IconItem::setMetadata(const QString& key, const QString& value) {
    if (!key.isEmpty()) {
        m_metadata[key] = value;
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

void IconItem::removeMetadata(const QString& key) {
    if (m_metadata.remove(key) > 0) {
        m_modifiedAt = QDateTime::currentDateTime();
    }
}

bool IconItem::operator==(const IconItem& other) const noexcept {
    return m_name == other.m_name;
}

bool IconItem::operator!=(const IconItem& other) const noexcept {
    return !(*this == other);
}

bool IconItem::operator<(const IconItem& other) const noexcept {
    return m_name < other.m_name;
}

bool IconItem::operator>(const IconItem& other) const noexcept {
    return m_name > other.m_name;
}

bool IconItem::operator<=(const IconItem& other) const noexcept {
    return m_name <= other.m_name;
}

bool IconItem::operator>=(const IconItem& other) const noexcept {
    return m_name >= other.m_name;
}

bool IconItem::isValid() const {
    return !m_name.isEmpty();
}

QString IconItem::toString() const {
    return QString("IconItem(name=%1, category=%2, tags=%3, favorite=%4, usage=%5)")
        .arg(m_name, m_category, m_tags.join(","), m_isFavorite ? "true" : "false",
             QString::number(m_usageCount));
}

} // namespace gallery
