/**
 * QtLucide Export Functionality Tests - Header
 * 
 * Comprehensive tests for icon export features including multiple formats,
 * batch operations, quality verification, and error handling.
 */

#ifndef TEST_EXPORT_FUNCTIONALITY_H
#define TEST_EXPORT_FUNCTIONALITY_H

#include <QObject>
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QPixmap>
#include <QIcon>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QDir>
#include <QImageReader>
#include <QImageWriter>
#include <QBuffer>
#include <QMimeDatabase>

namespace lucide {
class QtLucide;
}

class IconExportDialog;
class IconMetadataManager;

/**
 * @brief Export format configuration
 */
struct ExportFormat {
    QString name;
    QString extension;
    QString mimeType;
    QList<int> supportedSizes;
    bool supportsTransparency;
    bool supportsCompression;
    int qualityRange[2]; // min, max quality values
};

/**
 * @brief Export operation result
 */
struct ExportResult {
    bool success = false;
    QString filePath;
    QSize actualSize;
    qint64 fileSize = 0;
    QString format;
    QString errorMessage;
    qint64 exportTime = 0;
    bool qualityAcceptable = false;
};

/**
 * @brief Batch export configuration
 */
struct BatchExportConfig {
    QStringList iconNames;
    QList<int> sizes;
    QStringList formats;
    QString outputDirectory;
    bool maintainAspectRatio = true;
    int quality = 90;
    bool overwriteExisting = false;
};

class TestExportFunctionality : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Single icon export tests
    void testSingleIconExportPNG();
    void testSingleIconExportSVG();
    void testSingleIconExportICO();
    void testSingleIconExportJPEG();
    void testSingleIconExportBMP();
    void testSingleIconExportTIFF();

    // Multiple format export tests
    void testMultipleFormatExport();
    void testFormatQualitySettings();
    void testFormatCompressionSettings();
    void testFormatTransparencySupport();

    // Size configuration tests
    void testVariousSizeExports();
    void testCustomSizeExport();
    void testAspectRatioMaintenance();
    void testSizeValidation();
    void testMaximumSizeHandling();

    // Batch export tests
    void testBatchExportSameFormat();
    void testBatchExportMultipleFormats();
    void testBatchExportMultipleSizes();
    void testBatchExportProgress();
    void testBatchExportCancellation();
    void testBatchExportErrorHandling();

    // Quality verification tests
    void testExportedImageQuality();
    void testExportedImageIntegrity();
    void testExportedImageMetadata();
    void testExportedImageColorAccuracy();
    void testExportedImageSharpness();

    // File system integration tests
    void testFilePathHandling();
    void testFileOverwriteHandling();
    void testDirectoryCreation();
    void testFilePermissions();
    void testDiskSpaceHandling();
    void testNetworkPathExport();

    // Error handling tests
    void testInvalidIconExport();
    void testInvalidFormatExport();
    void testInvalidSizeExport();
    void testInsufficientDiskSpace();
    void testReadOnlyDirectoryExport();
    void testCorruptedIconExport();

    // Performance tests
    void testExportPerformance();
    void testBatchExportPerformance();
    void testLargeIconExportPerformance();
    void testConcurrentExportPerformance();

    // UI integration tests
    void testExportDialogFunctionality();
    void testExportDialogValidation();
    void testExportDialogPreview();
    void testExportDialogSettings();

private:
    // Export operation helpers
    ExportResult exportSingleIcon(const QString& iconName, const QString& format, 
                                 int size, const QString& outputPath);
    QList<ExportResult> exportBatch(const BatchExportConfig& config);
    bool verifyExportedFile(const QString& filePath, const QString& expectedFormat, 
                           const QSize& expectedSize);
    
    // Quality verification helpers
    bool verifyImageQuality(const QString& filePath, int expectedQuality);
    bool verifyImageIntegrity(const QString& filePath);
    bool verifyImageTransparency(const QString& filePath);
    bool compareImageSimilarity(const QPixmap& original, const QPixmap& exported, 
                               double threshold = 0.95);
    
    // File system helpers
    bool createTestDirectory(const QString& path);
    bool simulateInsufficientDiskSpace();
    bool simulateReadOnlyDirectory(const QString& path);
    qint64 getAvailableDiskSpace(const QString& path);
    
    // Format support helpers
    QList<ExportFormat> getSupportedExportFormats();
    bool isFormatSupported(const QString& format);
    ExportFormat getFormatInfo(const QString& format);
    
    // Performance measurement helpers
    qint64 measureExportTime(std::function<void()> exportOperation);
    void measureBatchExportProgress(const BatchExportConfig& config);
    
    // Test data and objects
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    IconExportDialog* m_exportDialog;
    QTemporaryDir* m_tempDir;
    QWidget* m_testWidget;
    
    // Test configuration
    QStringList m_testIconNames;
    QList<int> m_testSizes;
    QStringList m_testFormats;
    QList<ExportFormat> m_supportedFormats;
    
    // Performance tracking
    QElapsedTimer m_performanceTimer;
    
    // Test thresholds
    static constexpr qint64 SINGLE_EXPORT_THRESHOLD_MS = 1000;
    static constexpr qint64 BATCH_EXPORT_THRESHOLD_MS = 5000;
    static constexpr qint64 MIN_FILE_SIZE_BYTES = 100;
    static constexpr qint64 MAX_FILE_SIZE_BYTES = 1024 * 1024; // 1MB
    static constexpr double IMAGE_SIMILARITY_THRESHOLD = 0.90;
    static constexpr int MIN_EXPORT_QUALITY = 70;
};

#endif // TEST_EXPORT_FUNCTIONALITY_H
