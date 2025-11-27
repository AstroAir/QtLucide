/**
 * @file test_icon_engine.h
 * @brief Unit tests for QtLucideIconEngine
 * @details Tests for icon engine functionality including caching, cloning,
 *          size handling, and null state detection.
 */

#ifndef TEST_ICON_ENGINE_H
#define TEST_ICON_ENGINE_H

#include <QObject>
#include <QtTest/QtTest>

namespace lucide {
class QtLucide;
class QtLucideIconEngine;
class QtLucideIconPainter;
}  // namespace lucide

/**
 * @brief Test class for QtLucideIconEngine
 * @details Covers all public methods of QtLucideIconEngine:
 *          - paint(), pixmap(), clone(), key()
 *          - actualSize(), availableSizes(), iconName(), isNull()
 *          - Caching behavior and cache key generation
 */
class TestIconEngine : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Construction and basic properties
    void testConstructionWithValidParameters();
    void testConstructionWithNullLucide();
    void testConstructionWithNullPainter();
    void testKeyReturnsCorrectIdentifier();

    // isNull() behavior
    void testIsNullWithValidBuiltinIcon();
    void testIsNullWithInvalidIconId();
    void testIsNullWithCustomPainter();
    void testIsNullWithNullPainter();
    void testIsNullWithNullLucide();

    // paint() and pixmap()
    void testPaintDoesNotCrashWithValidIcon();
    void testPaintDoesNothingWithNullPainter();
    void testPixmapReturnsValidPixmap();
    void testPixmapCachingWorks();
    void testPixmapCacheLimitRespected();
    void testPixmapDifferentModesAreCachedSeparately();
    void testPixmapDifferentSizesAreCachedSeparately();

    // clone()
    void testCloneCreatesNewInstance();
    void testClonePreservesOptions();
    void testCloneSharesPainterReference();

    // actualSize()
    void testActualSizeReturnsScaledSize();
    void testActualSizeWithDefaultScaleFactor();
    void testActualSizeWithCustomScaleFactor();
    void testActualSizeClampsBounds();

    // availableSizes()
    void testAvailableSizesReturnsEmptyList();

    // iconName()
    void testIconNameWithValidIconId();
    void testIconNameWithInvalidIconId();
    void testIconNameWithNullLucide();

    // Mode and state handling
    void testPixmapNormalMode();
    void testPixmapDisabledMode();
    void testPixmapActiveMode();
    void testPixmapSelectedMode();
    void testPixmapOnState();
    void testPixmapOffState();

    // High-DPI support
    void testPixmapDevicePixelRatio();

private:
    lucide::QtLucide* m_lucide = nullptr;
};

#endif  // TEST_ICON_ENGINE_H
