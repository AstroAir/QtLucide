#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDir>

#include "QtLucide/QtLucide.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Testing resource availability";
    
    // Test QtLucide initialization
    lucide::QtLucide* lucide = new lucide::QtLucide();
    bool initResult = lucide->initLucide();
    qDebug() << "QtLucide init result:" << initResult;
    
    if (initResult) {
        QStringList icons = lucide->availableIcons();
        qDebug() << "Available icons count:" << icons.size();
    }
    
    // Test direct resource access
    QStringList resourcePaths = {
        ":/lucide/metadata/icons.json",
        ":/lucide/metadata/categories.json", 
        ":/lucide/metadata/tags.json",
        ":/lucide/heart",
        ":/lucide/star"
    };
    
    for (const QString& path : resourcePaths) {
        QFile file(path);
        bool exists = file.exists();
        qDebug() << "Resource" << path << "exists:" << exists;
        
        if (exists && file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            qDebug() << "  Size:" << data.size() << "bytes";
            if (data.size() > 0 && data.size() < 1000) {
                qDebug() << "  Content preview:" << data.left(100);
            }
        }
    }
    
    // List all resources with /lucide prefix
    qDebug() << "Listing all /lucide resources:";
    QDir resourceDir(":/lucide");
    if (resourceDir.exists()) {
        QStringList entries = resourceDir.entryList();
        qDebug() << "Found" << entries.size() << "entries in :/lucide";
        if (entries.size() > 0) {
            qDebug() << "First 10 entries:" << entries.mid(0, 10);
        }
    } else {
        qDebug() << ":/lucide directory does not exist";
    }
    
    qDebug() << "Resource test completed";
    
    return 0;
}
