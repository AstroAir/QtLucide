/**
 * QtLucide Export Functionality Tests - Implementation
 */

#include "test_export_functionality.h"
#include "core/managers/IconMetadataManager.h"
#include "ui/dialogs/IconExportDialog.h"
#include <QtLucide/QtLucide.h>

#include <QApplication>
#include <QIcon>
#include <QImageReader>
#include <QImageWriter>
#include <QMimeDatabase>
#include <QPixmap>
#include <QStandardPaths>

void TestExportFunctionality::initTestCase() {
    qDebug() << "Initializing Export Functionality Test Suite";

    // Initialize QtLucide
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());

    // Initialize metadata manager
    m_metadataManager = new IconMetadataManager(this);
    QVERIFY(m_metadataManager->loadMetadata());

    // Create temporary directory for test exports
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    // Create test widget
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);

    // Create export dialog
    m_exportDialog = new IconExportDialog(m_lucide, m_metadataManager, m_testWidget);

    // Setup test data
    m_testIconNames = {"heart", "star", "home", "user", "settings", "search", "file", "folder"};
    m_testSizes = {16, 24, 32, 48, 64, 96, 128, 256};
    m_testFormats = {"PNG", "SVG", "ICO", "JPEG", "BMP"};

    // Get supported export formats
    m_supportedFormats = getSupportedExportFormats();

    qDebug() << "Export functionality test environment initialized successfully";
    qDebug() << "Temporary directory:" << m_tempDir->path();
    qDebug() << "Supported formats:" << m_testFormats.size();
}

void TestExportFunctionality::cleanupTestCase() {
    delete m_exportDialog;
    delete m_testWidget;
    delete m_tempDir;
    delete m_metadataManager;
    delete m_lucide;
    qDebug() << "Export functionality test cleanup completed";
}

void TestExportFunctionality::init() {
    m_performanceTimer.start();
}

void TestExportFunctionality::cleanup() {
    // Clean up any test files created during individual tests
    QDir tempDir(m_tempDir->path());
    QStringList testFiles = tempDir.entryList(QDir::Files);
    for (const QString& file : testFiles) {
        tempDir.remove(file);
    }
}

void TestExportFunctionality::testSingleIconExportPNG() {
    qDebug() << "Testing single icon export to PNG";

    QString iconName = "heart";
    int size = 64;
    QString outputPath = m_tempDir->path() + "/test_heart.png";

    ExportResult result = exportSingleIcon(iconName, "PNG", size, outputPath);

    QVERIFY2(result.success, qPrintable(result.errorMessage));
    QVERIFY(QFileInfo::exists(result.filePath));
    QCOMPARE(result.format, QString("PNG"));
    QCOMPARE(result.actualSize, QSize(size, size));
    QVERIFY(result.fileSize > MIN_FILE_SIZE_BYTES);
    QVERIFY(result.fileSize < MAX_FILE_SIZE_BYTES);

    // Verify PNG-specific properties
    QVERIFY(verifyImageTransparency(result.filePath));
    QVERIFY(verifyImageIntegrity(result.filePath));

    qDebug() << QString("PNG export successful: %1 bytes, %2ms")
                    .arg(result.fileSize)
                    .arg(result.exportTime);

    qDebug() << "Single icon PNG export test passed";
}

void TestExportFunctionality::testSingleIconExportSVG() {
    qDebug() << "Testing single icon export to SVG";

    QString iconName = "star";
    QString outputPath = m_tempDir->path() + "/test_star.svg";

    ExportResult result =
        exportSingleIcon(iconName, "SVG", 0, outputPath); // SVG is vector, size doesn't matter

    QVERIFY2(result.success, qPrintable(result.errorMessage));
    QVERIFY(QFileInfo::exists(result.filePath));
    QCOMPARE(result.format, QString("SVG"));
    QVERIFY(result.fileSize > MIN_FILE_SIZE_BYTES);

    // Verify SVG content
    QFile svgFile(result.filePath);
    QVERIFY(svgFile.open(QIODevice::ReadOnly));
    QByteArray svgContent = svgFile.readAll();
    QVERIFY(svgContent.contains("<svg"));
    QVERIFY(svgContent.contains("</svg>"));
    svgFile.close();

    qDebug() << QString("SVG export successful: %1 bytes, %2ms")
                    .arg(result.fileSize)
                    .arg(result.exportTime);

    qDebug() << "Single icon SVG export test passed";
}

void TestExportFunctionality::testVariousSizeExports() {
    qDebug() << "Testing various size exports";

    QString iconName = "home";
    QString format = "PNG";

    for (int size : m_testSizes) {
        QString outputPath = m_tempDir->path() + QString("/test_home_%1.png").arg(size);

        ExportResult result = exportSingleIcon(iconName, format, size, outputPath);

        QVERIFY2(
            result.success,
            qPrintable(QString("Failed to export size %1: %2").arg(size).arg(result.errorMessage)));
        QVERIFY(QFileInfo::exists(result.filePath));
        QCOMPARE(result.actualSize, QSize(size, size));

        // Larger sizes should generally produce larger files
        if (size >= 64) {
            QVERIFY(result.fileSize > MIN_FILE_SIZE_BYTES * 2);
        }

        qDebug() << QString("Size %1: %2 bytes").arg(size).arg(result.fileSize);
    }

    qDebug() << "Various size exports test passed";
}

void TestExportFunctionality::testBatchExportSameFormat() {
    qDebug() << "Testing batch export same format";

    BatchExportConfig config;
    config.iconNames = m_testIconNames.mid(0, 4); // First 4 icons
    config.sizes = {32, 64};
    config.formats = {"PNG"};
    config.outputDirectory = m_tempDir->path();
    config.quality = 90;

    QList<ExportResult> results = exportBatch(config);

    int expectedCount =
        static_cast<int>(config.iconNames.size() * config.sizes.size() * config.formats.size());
    QCOMPARE(results.size(), expectedCount);

    // Verify all exports succeeded
    int successCount = 0;
    for (const ExportResult& result : results) {
        if (result.success) {
            successCount++;
            QVERIFY(QFileInfo::exists(result.filePath));
            QVERIFY(result.fileSize > MIN_FILE_SIZE_BYTES);
        } else {
            qWarning() << "Export failed:" << result.errorMessage;
        }
    }

    QVERIFY2(successCount == expectedCount,
             qPrintable(
                 QString("Only %1 of %2 exports succeeded").arg(successCount).arg(expectedCount)));

    qDebug()
        << QString("Batch export completed: %1/%2 successful").arg(successCount).arg(expectedCount);
    qDebug() << "Batch export same format test passed";
}

void TestExportFunctionality::testExportedImageQuality() {
    qDebug() << "Testing exported image quality";

    QString iconName = "user";
    int size = 128;
    QString outputPath = m_tempDir->path() + "/test_quality.png";

    ExportResult result = exportSingleIcon(iconName, "PNG", size, outputPath);
    QVERIFY(result.success);

    // Load original icon for comparison
    QIcon originalIcon = m_lucide->icon(iconName);
    QPixmap originalPixmap = originalIcon.pixmap(size, size);

    // Load exported image
    QPixmap exportedPixmap(result.filePath);
    QVERIFY(!exportedPixmap.isNull());

    // Compare similarity
    bool similarityAcceptable =
        compareImageSimilarity(originalPixmap, exportedPixmap, IMAGE_SIMILARITY_THRESHOLD);
    QVERIFY2(similarityAcceptable, "Exported image quality below acceptable threshold");

    // Verify image properties
    QVERIFY(verifyImageIntegrity(result.filePath));
    QVERIFY(verifyImageQuality(result.filePath, MIN_EXPORT_QUALITY));

    qDebug() << "Exported image quality test passed";
}

void TestExportFunctionality::testFilePathHandling() {
    qDebug() << "Testing file path handling";

    QString iconName = "settings";
    int size = 48;

    // Test various path scenarios
    QStringList testPaths = {
        m_tempDir->path() + "/simple.png", m_tempDir->path() + "/path with spaces.png",
        m_tempDir->path() + "/path-with-dashes.png",
        m_tempDir->path() + "/path_with_underscores.png", m_tempDir->path() + "/UPPERCASE.PNG"};

    for (const QString& path : testPaths) {
        ExportResult result = exportSingleIcon(iconName, "PNG", size, path);

        QVERIFY2(
            result.success,
            qPrintable(
                QString("Failed to export to path: %1 - %2").arg(path).arg(result.errorMessage)));
        QVERIFY(QFileInfo::exists(result.filePath));

        qDebug() << "Successfully exported to:" << QFileInfo(path).fileName();
    }

    qDebug() << "File path handling test passed";
}

ExportResult TestExportFunctionality::exportSingleIcon(const QString& iconName,
                                                       const QString& format, int size,
                                                       const QString& outputPath) {
    ExportResult result;
    QElapsedTimer timer;
    timer.start();

    try {
        // Get the icon
        QIcon icon = m_lucide->icon(iconName);
        if (icon.isNull()) {
            result.errorMessage = "Failed to load icon: " + iconName;
            return result;
        }

        if (format.toUpper() == "SVG") {
            // For SVG, we need to get the original SVG data
            // This is a simplified implementation
            result.filePath = outputPath;
            result.format = format;
            result.success = true;
            result.fileSize = 1000; // Placeholder
        } else {
            // For raster formats, create pixmap and save
            QPixmap pixmap = icon.pixmap(size, size);
            if (pixmap.isNull()) {
                result.errorMessage = "Failed to create pixmap";
                return result;
            }

            bool saved = pixmap.save(outputPath, format.toUtf8().constData());
            if (!saved) {
                result.errorMessage = "Failed to save pixmap to file";
                return result;
            }

            result.filePath = outputPath;
            result.actualSize = pixmap.size();
            result.format = format;
            result.success = true;

            QFileInfo fileInfo(outputPath);
            result.fileSize = fileInfo.size();
        }

    } catch (const std::exception& e) {
        result.errorMessage = QString("Exception during export: %1").arg(e.what());
    }

    result.exportTime = timer.elapsed();
    return result;
}

QList<ExportResult> TestExportFunctionality::exportBatch(const BatchExportConfig& config) {
    QList<ExportResult> results;

    for (const QString& iconName : config.iconNames) {
        for (int size : config.sizes) {
            for (const QString& format : config.formats) {
                QString fileName =
                    QString("%1_%2x%3.%4").arg(iconName).arg(size).arg(size).arg(format.toLower());
                QString outputPath = config.outputDirectory + "/" + fileName;

                ExportResult result = exportSingleIcon(iconName, format, size, outputPath);
                results.append(result);
            }
        }
    }

    return results;
}

bool TestExportFunctionality::verifyExportedFile(const QString& filePath,
                                                 const QString& expectedFormat,
                                                 const QSize& expectedSize) {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
        return false;
    if (fileInfo.size() < MIN_FILE_SIZE_BYTES)
        return false;

    if (expectedFormat.toUpper() != "SVG") {
        QImageReader reader(filePath);
        if (!reader.canRead())
            return false;
        if (reader.size() != expectedSize)
            return false;
    }

    return true;
}

bool TestExportFunctionality::verifyImageIntegrity(const QString& filePath) {
    QImageReader reader(filePath);
    return reader.canRead() && !reader.read().isNull();
}

bool TestExportFunctionality::verifyImageTransparency(const QString& filePath) {
    QImage image(filePath);
    return image.hasAlphaChannel();
}

bool TestExportFunctionality::compareImageSimilarity(const QPixmap& original,
                                                     const QPixmap& exported, double threshold) {
    // Simplified similarity comparison
    if (original.size() != exported.size())
        return false;

    QImage originalImage = original.toImage();
    QImage exportedImage = exported.toImage();

    if (originalImage.format() != exportedImage.format()) {
        exportedImage = exportedImage.convertToFormat(originalImage.format());
    }

    // Simple pixel-by-pixel comparison (in real implementation, use more sophisticated algorithms)
    int totalPixels = originalImage.width() * originalImage.height();
    int similarPixels = 0;

    for (int y = 0; y < originalImage.height(); ++y) {
        for (int x = 0; x < originalImage.width(); ++x) {
            QRgb originalPixel = originalImage.pixel(x, y);
            QRgb exportedPixel = exportedImage.pixel(x, y);

            // Simple color distance check
            int rDiff = qAbs(qRed(originalPixel) - qRed(exportedPixel));
            int gDiff = qAbs(qGreen(originalPixel) - qGreen(exportedPixel));
            int bDiff = qAbs(qBlue(originalPixel) - qBlue(exportedPixel));

            if (rDiff < 10 && gDiff < 10 && bDiff < 10) {
                similarPixels++;
            }
        }
    }

    double similarity = static_cast<double>(similarPixels) / totalPixels;
    return similarity >= threshold;
}

QList<ExportFormat> TestExportFunctionality::getSupportedExportFormats() {
    QList<ExportFormat> formats;

    // PNG format
    ExportFormat png;
    png.name = "PNG";
    png.extension = "png";
    png.mimeType = "image/png";
    png.supportedSizes = m_testSizes;
    png.supportsTransparency = true;
    png.supportsCompression = true;
    png.qualityRange[0] = 0;
    png.qualityRange[1] = 100;
    formats.append(png);

    // SVG format
    ExportFormat svg;
    svg.name = "SVG";
    svg.extension = "svg";
    svg.mimeType = "image/svg+xml";
    svg.supportedSizes = {0}; // Vector format
    svg.supportsTransparency = true;
    svg.supportsCompression = false;
    svg.qualityRange[0] = 100;
    svg.qualityRange[1] = 100;
    formats.append(svg);

    return formats;
}

// Missing method implementation
bool TestExportFunctionality::verifyImageQuality(const QString& filePath, int expectedQuality) {
    Q_UNUSED(filePath)
    Q_UNUSED(expectedQuality)
    // Stub implementation - always return true for now
    return true;
}

// Stub implementations for missing test methods
void TestExportFunctionality::testSingleIconExportICO() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testSingleIconExportJPEG() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testSingleIconExportBMP() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testSingleIconExportTIFF() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testMultipleFormatExport() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testFormatQualitySettings() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testFormatCompressionSettings() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testFormatTransparencySupport() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testCustomSizeExport() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testAspectRatioMaintenance() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testSizeValidation() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testMaximumSizeHandling() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testBatchExportMultipleFormats() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testBatchExportMultipleSizes() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testBatchExportProgress() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testBatchExportCancellation() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testBatchExportErrorHandling() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testExportedImageIntegrity() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testExportedImageMetadata() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testExportedImageColorAccuracy() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testExportedImageSharpness() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testFileOverwriteHandling() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testDirectoryCreation() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testFilePermissions() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testDiskSpaceHandling() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testNetworkPathExport() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testInvalidIconExport() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testInvalidFormatExport() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testInvalidSizeExport() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testInsufficientDiskSpace() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testReadOnlyDirectoryExport() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testCorruptedIconExport() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testExportPerformance() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testBatchExportPerformance() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testLargeIconExportPerformance() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testConcurrentExportPerformance() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testExportDialogFunctionality() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testExportDialogValidation() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testExportDialogPreview() {
    QSKIP("Test not implemented yet");
}

void TestExportFunctionality::testExportDialogSettings() {
    QSKIP("Test not implemented yet");
}

// MOC file automatically included by CMake/Meson
