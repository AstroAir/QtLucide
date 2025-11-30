/**
 * @file test_icon_painter.h
 * @brief Unit tests for QtLucideIconPainter and QtLucideSvgIconPainter
 * @details Tests for icon painter functionality including SVG processing,
 *          color handling, mode/state options, and custom painter support.
 */

#ifndef TEST_ICON_PAINTER_H
#define TEST_ICON_PAINTER_H

#include <QObject>
#include <QtTest/QtTest>

namespace lucide {
class QtLucide;
class QtLucideIconPainter;
class QtLucideSvgIconPainter;
} // namespace lucide

/**
 * @brief Test class for QtLucideIconPainter and QtLucideSvgIconPainter
 * @details Covers:
 *          - QtLucideIconPainter: clone(), iconText(), paint()
 *          - QtLucideSvgIconPainter: SVG color processing, mode/state handling
 *          - Custom painter implementation and registration
 */
class TestIconPainter : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // QtLucideSvgIconPainter basic functionality
    void testSvgPainterConstruction();
    void testSvgPainterClone();
    void testSvgPainterIconText();

    // paint() with valid parameters
    void testPaintWithValidIcon();
    void testPaintWithValidIconAndColor();
    void testPaintWithValidIconAndOpacity();
    void testPaintWithValidIconAndScaleFactor();

    // paint() with invalid parameters
    void testPaintWithNullLucide();
    void testPaintWithNullQPainter();
    void testPaintWithInvalidIconId();
    void testPaintWithNegativeIconId();

    // Color processing
    void testColorReplacementInSvg();
    void testCurrentColorReplacement();
    void testDisabledModeAutoColor();
    void testActiveModeColor();
    void testSelectedModeColor();

    // Mode and state option keys
    void testOptionKeysForNormalMode();
    void testOptionKeysForDisabledMode();
    void testOptionKeysForActiveMode();
    void testOptionKeysForSelectedMode();
    void testOptionKeysForOffState();
    void testOptionKeysForOnState();
    void testOptionKeysPriorityOrder();

    // Option value resolution
    void testOptionValueWithExactMatch();
    void testOptionValueWithFallback();
    void testOptionValueWithNoMatch();
    void testOptionValueWithInvalidVariant();

    // Scale factor handling
    void testScaleFactorDefault();
    void testScaleFactorCustom();
    void testScaleFactorClamping();
    void testScaleFactorZero();
    void testScaleFactorNegative();

    // Opacity handling
    void testOpacityDefault();
    void testOpacityCustom();
    void testOpacityZero();
    void testOpacityGreaterThanOne();
    void testOpacityNegative();

    // Custom painter implementation
    void testCustomPainterRegistration();
    void testCustomPainterClone();
    void testCustomPainterIconText();
    void testCustomPainterPaint();
    void testCustomPainterWithOptions();

    // Edge cases
    void testPaintWithEmptyRect();
    void testPaintWithLargeRect();
    void testPaintWithNonSquareRect();
    void testMultiplePaintsInSequence();

private:
    lucide::QtLucide* m_lucide = nullptr;
    lucide::QtLucideSvgIconPainter* m_svgPainter = nullptr;
};

#endif // TEST_ICON_PAINTER_H
