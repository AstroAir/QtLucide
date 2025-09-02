#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Testing resource access...";
    
    // Test if resource file exists
    QFile resourceFile(":/lucide/activity");
    if (resourceFile.exists()) {
        qDebug() << "SUCCESS: Resource :/lucide/activity exists!";
        if (resourceFile.open(QIODevice::ReadOnly)) {
            QByteArray data = resourceFile.readAll();
            qDebug() << "Resource size:" << data.size() << "bytes";
            qDebug() << "First 100 chars:" << data.left(100);
        }
    } else {
        qDebug() << "ERROR: Resource :/lucide/activity does not exist!";
    }
    
    // List all available resources
    QDir resourceDir(":/lucide");
    if (resourceDir.exists()) {
        qDebug() << "SUCCESS: Resource directory :/lucide exists!";
        QStringList entries = resourceDir.entryList();
        qDebug() << "Available resources:" << entries.size();
        for (int i = 0; i < qMin(10, entries.size()); ++i) {
            qDebug() << "  -" << entries[i];
        }
    } else {
        qDebug() << "ERROR: Resource directory :/lucide does not exist!";
    }
    
    return 0;
}
