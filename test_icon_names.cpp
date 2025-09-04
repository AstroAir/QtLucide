#include <QApplication>
#include <QDebug>
#include <QStringList>

#include "QtLucide/QtLucide.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Testing icon names availability";
    
    lucide::QtLucide* lucide = new lucide::QtLucide();
    if (!lucide->initLucide()) {
        qDebug() << "Failed to initialize QtLucide";
        return 1;
    }
    
    QStringList availableIcons = lucide->availableIcons();
    qDebug() << "Total available icons:" << availableIcons.size();
    
    // Test common icon names
    QStringList testNames = {
        "heart", "star", "house", "user", "settings", "search", 
        "menu", "x", "check", "folder", "home", "close",
        "plus", "minus", "edit", "trash", "download", "upload",
        "mail", "phone", "calendar", "clock", "camera", "image"
    };
    
    qDebug() << "\nTesting icon names:";
    QStringList validIcons;
    QStringList invalidIcons;
    
    for (const QString& name : testNames) {
        if (availableIcons.contains(name)) {
            validIcons << name;
            qDebug() << "✓" << name << "- VALID";
        } else {
            invalidIcons << name;
            qDebug() << "✗" << name << "- INVALID";
        }
    }
    
    qDebug() << "\nSummary:";
    qDebug() << "Valid icons:" << validIcons;
    qDebug() << "Invalid icons:" << invalidIcons;
    
    // Find similar names for invalid icons
    qDebug() << "\nSuggestions for invalid icons:";
    for (const QString& invalid : invalidIcons) {
        QStringList suggestions;
        for (const QString& available : availableIcons) {
            if (available.contains(invalid, Qt::CaseInsensitive) || 
                invalid.contains(available, Qt::CaseInsensitive)) {
                suggestions << available;
                if (suggestions.size() >= 3) break; // Limit to 3 suggestions
            }
        }
        if (!suggestions.isEmpty()) {
            qDebug() << "  " << invalid << "-> suggestions:" << suggestions;
        }
    }
    
    // Show first 20 available icons as reference
    qDebug() << "\nFirst 20 available icons for reference:";
    qDebug() << availableIcons.mid(0, 20);
    
    return 0;
}
