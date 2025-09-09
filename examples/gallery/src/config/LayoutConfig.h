/**
 * QtLucide Gallery Application - Layout Configuration
 * 
 * Centralized layout constants and responsive design utilities
 * for consistent spacing, sizing, and visual hierarchy throughout the application.
 */

#ifndef LAYOUTCONFIG_H
#define LAYOUTCONFIG_H

#include <QSize>
#include <QMargins>
#include <QApplication>
#include <QScreen>
#include <QWidget>

namespace GalleryLayout {

// ============================================================================
// SPACING AND MARGINS
// ============================================================================

namespace Spacing {
    // Base spacing unit (all other spacing should be multiples of this)
    constexpr int BASE_UNIT = 4;
    
    // Standard spacing values
    constexpr int TINY = BASE_UNIT;           // 4px
    constexpr int SMALL = BASE_UNIT * 2;      // 8px  
    constexpr int MEDIUM = BASE_UNIT * 3;     // 12px
    constexpr int LARGE = BASE_UNIT * 4;      // 16px
    constexpr int XLARGE = BASE_UNIT * 6;     // 24px
    
    // Component-specific spacing
    constexpr int WIDGET_SPACING = SMALL;     // 8px between widgets
    constexpr int PANEL_SPACING = MEDIUM;     // 12px between panels
    constexpr int SECTION_SPACING = LARGE;    // 16px between sections
}

namespace Margins {
    // Standard margin values
    constexpr int NONE = 0;
    constexpr int TINY = Spacing::TINY;       // 4px
    constexpr int SMALL = Spacing::SMALL;     // 8px
    constexpr int MEDIUM = Spacing::MEDIUM;   // 12px
    constexpr int LARGE = Spacing::LARGE;     // 16px
    
    // Component-specific margins
    constexpr int WINDOW_MARGIN = SMALL;      // 8px window margins
    constexpr int PANEL_MARGIN = MEDIUM;      // 12px panel margins
    constexpr int CONTENT_MARGIN = LARGE;     // 16px content margins
}

// ============================================================================
// SIZING CONSTANTS
// ============================================================================

namespace Sizing {
    // Thumbnail sizes
    constexpr int THUMBNAIL_SMALL = 64;
    constexpr int THUMBNAIL_MEDIUM = 96;
    constexpr int THUMBNAIL_LARGE = 128;
    constexpr int THUMBNAIL_XLARGE = 192;
    
    // Default thumbnail size
    constexpr int THUMBNAIL_DEFAULT = THUMBNAIL_MEDIUM;
    
    // Sidebar dimensions
    constexpr int SIDEBAR_MIN_WIDTH = 200;
    constexpr int SIDEBAR_PREFERRED_WIDTH = 280;
    constexpr int SIDEBAR_MAX_WIDTH = 400;
    
    // Panel dimensions
    constexpr int DETAILS_PANEL_MIN_HEIGHT = 200;
    constexpr int DETAILS_PANEL_PREFERRED_HEIGHT = 300;
    
    // Search widget dimensions
    constexpr int SEARCH_WIDGET_HEIGHT = 48;
    constexpr int SEARCH_WIDGET_MIN_WIDTH = 200;
    
    // Splitter handle width
    constexpr int SPLITTER_HANDLE_WIDTH = 6;
}

// ============================================================================
// RESPONSIVE DESIGN UTILITIES
// ============================================================================

class ResponsiveLayout {
public:
    enum class ScreenSize {
        Small,      // < 1024px width
        Medium,     // 1024-1440px width  
        Large,      // 1440-1920px width
        XLarge      // > 1920px width
    };
    
    // Get current screen size category
    static ScreenSize getScreenSize(const QWidget* widget = nullptr) {
        QScreen* screen = QApplication::primaryScreen();
        if (widget && widget->window()) {
            screen = widget->window()->screen();
        }
        
        if (!screen) return ScreenSize::Medium;
        
        int width = screen->availableGeometry().width();
        
        if (width < 1024) return ScreenSize::Small;
        if (width < 1440) return ScreenSize::Medium;
        if (width < 1920) return ScreenSize::Large;
        return ScreenSize::XLarge;
    }
    
    // Get responsive thumbnail size based on screen size
    static int getThumbnailSize(ScreenSize screenSize = ScreenSize::Medium) {
        switch (screenSize) {
            case ScreenSize::Small:  return Sizing::THUMBNAIL_SMALL;
            case ScreenSize::Medium: return Sizing::THUMBNAIL_MEDIUM;
            case ScreenSize::Large:  return Sizing::THUMBNAIL_LARGE;
            case ScreenSize::XLarge: return Sizing::THUMBNAIL_XLARGE;
        }
        return Sizing::THUMBNAIL_DEFAULT;
    }
    
    // Get responsive sidebar width based on screen size
    static int getSidebarWidth(ScreenSize screenSize = ScreenSize::Medium) {
        switch (screenSize) {
            case ScreenSize::Small:  return Sizing::SIDEBAR_MIN_WIDTH;
            case ScreenSize::Medium: return Sizing::SIDEBAR_PREFERRED_WIDTH;
            case ScreenSize::Large:  return Sizing::SIDEBAR_PREFERRED_WIDTH + 40;
            case ScreenSize::XLarge: return Sizing::SIDEBAR_MAX_WIDTH;
        }
        return Sizing::SIDEBAR_PREFERRED_WIDTH;
    }
    
    // Calculate optimal grid columns based on available width and thumbnail size
    static int calculateGridColumns(int availableWidth, int thumbnailSize, 
                                   int itemSpacing = Spacing::SMALL) {
        if (availableWidth <= 0 || thumbnailSize <= 0) return 1;
        
        // Account for margins and padding
        int usableWidth = availableWidth - 2 * Margins::CONTENT_MARGIN;
        
        // Calculate item width including spacing
        int itemWidth = thumbnailSize + 2 * Spacing::TINY; // padding around thumbnail
        int totalItemWidth = itemWidth + itemSpacing;
        
        // Calculate columns
        int columns = qMax(1, usableWidth / totalItemWidth);
        
        // Ensure we don't exceed reasonable limits
        return qMin(columns, 12); // Max 12 columns
    }
    
    // Get responsive splitter proportions
    static QList<int> getSplitterProportions(ScreenSize screenSize, 
                                           bool hasThreePanes = false) {
        if (hasThreePanes) {
            // Sidebar : Content : Details
            switch (screenSize) {
                case ScreenSize::Small:  return {1, 3, 1};  // Compact layout
                case ScreenSize::Medium: return {1, 4, 1};  // Balanced
                case ScreenSize::Large:  return {1, 5, 2};  // More content space
                case ScreenSize::XLarge: return {1, 6, 2};  // Maximum content
            }
        } else {
            // Sidebar : Content
            switch (screenSize) {
                case ScreenSize::Small:  return {1, 3};
                case ScreenSize::Medium: return {1, 4};
                case ScreenSize::Large:  return {1, 5};
                case ScreenSize::XLarge: return {1, 6};
            }
        }
        return {1, 4}; // Default
    }
};

// ============================================================================
// STYLE CONSTANTS
// ============================================================================

namespace Style {
    // Border radius values
    constexpr int BORDER_RADIUS_SMALL = 4;
    constexpr int BORDER_RADIUS_MEDIUM = 6;
    constexpr int BORDER_RADIUS_LARGE = 8;
    
    // Common colors (as string constants for stylesheets)
    constexpr const char* BACKGROUND_PRIMARY = "#f8f9fa";
    constexpr const char* BACKGROUND_SECONDARY = "#ffffff";
    constexpr const char* BORDER_COLOR = "#e9ecef";
    constexpr const char* BORDER_COLOR_HOVER = "#d5dbdb";
    constexpr const char* ACCENT_COLOR = "#3498db";
    constexpr const char* TEXT_COLOR = "#2c3e50";
    constexpr const char* TEXT_COLOR_SECONDARY = "#7f8c8d";
}

// ============================================================================
// CONVENIENCE FUNCTIONS
// ============================================================================

// Create standard margins
inline QMargins createMargins(int margin = Margins::MEDIUM) {
    return QMargins(margin, margin, margin, margin);
}

// Create asymmetric margins
inline QMargins createMargins(int left, int top, int right, int bottom) {
    return QMargins(left, top, right, bottom);
}

// Create standard size
inline QSize createSize(int width, int height) {
    return QSize(width, height);
}

// Get standard stylesheet for panels
inline QString getPanelStyleSheet() {
    return QString(
        "QWidget {"
        "  background-color: %1;"
        "  border: 1px solid %2;"
        "  border-radius: %3px;"
        "}"
    ).arg(Style::BACKGROUND_SECONDARY)
     .arg(Style::BORDER_COLOR)
     .arg(Style::BORDER_RADIUS_MEDIUM);
}

// Get standard stylesheet for splitter handles
inline QString getSplitterStyleSheet() {
    return QString(
        "QSplitter::handle {"
        "  background-color: %1;"
        "  border: 1px solid %2;"
        "  border-radius: %3px;"
        "  margin: 1px;"
        "}"
        "QSplitter::handle:hover {"
        "  background-color: %4;"
        "}"
    ).arg(Style::BACKGROUND_PRIMARY)
     .arg(Style::BORDER_COLOR)
     .arg(Style::BORDER_RADIUS_SMALL)
     .arg(Style::BORDER_COLOR_HOVER);
}

} // namespace GalleryLayout

#endif // LAYOUTCONFIG_H
