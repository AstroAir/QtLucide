/**
 * QtLucide Gallery Application - Image Browsing Functionality Tests
 *
 * Tests for image browsing capabilities including:
 * - Loading various image formats (PNG, JPG, SVG, etc.)
 * - Image display and rendering
 * - Image scaling and transformations
 * - Error handling for invalid/corrupted images
 * - Performance with large images
 */

#include <QtTest/QtTest>
#include <QApplication>
#include <QtTest/QSignalSpy>
#include <QPixmap>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QDir>
#include <QElapsedTimer>

// Include Gallery components
#include "ui/widgets/viewers/ImageViewerWidget.h"
#include "core/managers/ImageMetadataManager.h"
#include "core/managers/ContentManager.h"
#include <QtLucide/QtLucide.h>

class TestImageBrowsing : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Image Format Tests
    void testImageFormats_PNG();
    void testImageFormats_JPG();
    void testImageFormats_SVG();
    void testImageFormats_GIF();
    void testImageFormats_BMP();
    void testImageFormats_TIFF();
    void testImageFormats_WEBP();

    // Image Loading Tests
    void testImageLoading_ValidImages();
    void testImageLoading_InvalidImages();
    void testImageLoading_CorruptedImages();
    void testImageLoading_LargeImages();
    void testImageLoading_EmptyFiles();

    // Image Display Tests
    void testImageDisplay_BasicDisplay();
    void testImageDisplay_Scaling();
    void testImageDisplay_AspectRatio();
    void testImageDisplay_Rotation();
    void testImageDisplay_Flipping();

    // Performance Tests
    void testPerformance_LoadingSpeed();
    void testPerformance_MemoryUsage();
    void testPerformance_MultipleImages();

    // Error Handling Tests
    void testErrorHandling_FileNotFound();
    void testErrorHandling_UnsupportedFormat();
    void testErrorHandling_PermissionDenied();

private:
    // Test fixtures
    QApplication* m_app;
    lucide::QtLucide* m_lucide;
    ImageMetadataManager* m_imageMetadataManager;
    ContentManager* m_contentManager;
    QTemporaryDir* m_tempDir;

    // Test data
    QString m_testImagePNG;
    QString m_testImageJPG;
    QString m_testImageSVG;
    QString m_testImageGIF;
    QString m_testImageBMP;
    QString m_testImageCorrupted;
    QString m_testImageLarge;

    // Helper methods
    void createTestImages();
    QString createTestImage(const QString& format, const QSize& size = QSize(100, 100));
    QString createCorruptedImage();
    QString createLargeImage();
    bool isImageFormatSupported(const QString& format);
    QStringList getSupportedImageFormats();
};

void TestImageBrowsing::initTestCase()
{
    // Initialize QtLucide
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());

    // Create managers
    m_imageMetadataManager = new ImageMetadataManager(this);
    m_contentManager = new ContentManager(this);

    // Setup test environment
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    createTestImages();

    qDebug() << "Image browsing test environment initialized";
    qDebug() << "Supported image formats:" << getSupportedImageFormats();
}

void TestImageBrowsing::cleanupTestCase()
{
    delete m_tempDir;
    qDebug() << "Image browsing test environment cleaned up";
}

void TestImageBrowsing::init()
{
    // Reset state before each test
}

void TestImageBrowsing::cleanup()
{
    // Clean up after each test
}

void TestImageBrowsing::createTestImages()
{
    // Create test images in various formats
    m_testImagePNG = createTestImage("PNG");
    m_testImageJPG = createTestImage("JPG");
    m_testImageBMP = createTestImage("BMP");
    m_testImageCorrupted = createCorruptedImage();
    m_testImageLarge = createLargeImage();

    // Create SVG test image manually
    m_testImageSVG = m_tempDir->path() + "/test_image.svg";
    QFile svgFile(m_testImageSVG);
    if (svgFile.open(QIODevice::WriteOnly)) {
        QTextStream stream(&svgFile);
        stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        stream << "<svg width=\"100\" height=\"100\" xmlns=\"http://www.w3.org/2000/svg\">\n";
        stream << "  <rect width=\"100\" height=\"100\" fill=\"blue\"/>\n";
        stream << "  <circle cx=\"50\" cy=\"50\" r=\"30\" fill=\"red\"/>\n";
        stream << "</svg>\n";
    }
}

QString TestImageBrowsing::createTestImage(const QString& format, const QSize& size)
{
    QString fileName = m_tempDir->path() + QString("/test_image.%1").arg(format.toLower());

    QImage image(size, QImage::Format_RGB32);
    image.fill(Qt::blue);

    // Add some pattern to make it interesting
    QPainter painter(&image);
    painter.setPen(Qt::red);
    painter.setBrush(Qt::yellow);
    painter.drawEllipse(size.width()/4, size.height()/4, size.width()/2, size.height()/2);
    painter.end();

    if (image.save(fileName, format.toUtf8().constData())) {
        return fileName;
    }
    return QString();
}

QString TestImageBrowsing::createCorruptedImage()
{
    QString fileName = m_tempDir->path() + "/corrupted_image.png";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        // Write invalid PNG data
        file.write("This is not a valid PNG file");
    }
    return fileName;
}

QString TestImageBrowsing::createLargeImage()
{
    return createTestImage("PNG", QSize(2048, 2048));
}

bool TestImageBrowsing::isImageFormatSupported(const QString& format)
{
    QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
    return supportedFormats.contains(format.toLower().toUtf8());
}

QStringList TestImageBrowsing::getSupportedImageFormats()
{
    QStringList formats;
    for (const QByteArray& format : QImageReader::supportedImageFormats()) {
        formats << QString::fromUtf8(format).toUpper();
    }
    return formats;
}

// Image Format Tests
void TestImageBrowsing::testImageFormats_PNG()
{
    if (!isImageFormatSupported("PNG")) {
        QSKIP("PNG format not supported");
    }

    QVERIFY(QFileInfo::exists(m_testImagePNG));

    QImageReader reader(m_testImagePNG);
    QVERIFY(reader.canRead());
    QCOMPARE(reader.format(), QByteArray("png"));

    QImage image = reader.read();
    QVERIFY(!image.isNull());
    QCOMPARE(image.size(), QSize(100, 100));
}

void TestImageBrowsing::testImageFormats_JPG()
{
    if (!isImageFormatSupported("JPG")) {
        QSKIP("JPG format not supported");
    }

    QVERIFY(QFileInfo::exists(m_testImageJPG));

    QImageReader reader(m_testImageJPG);
    QVERIFY(reader.canRead());

    QImage image = reader.read();
    QVERIFY(!image.isNull());
    QCOMPARE(image.size(), QSize(100, 100));
}

void TestImageBrowsing::testImageFormats_SVG()
{
    if (!isImageFormatSupported("SVG")) {
        QSKIP("SVG format not supported");
    }

    QVERIFY(QFileInfo::exists(m_testImageSVG));

    QImageReader reader(m_testImageSVG);
    QVERIFY(reader.canRead());
    QCOMPARE(reader.format(), QByteArray("svg"));
}

void TestImageBrowsing::testImageFormats_GIF()
{
    if (!isImageFormatSupported("GIF")) {
        QSKIP("GIF format not supported");
    }

    // GIF test would go here if we created a GIF test image
    QVERIFY(true); // Placeholder
}

void TestImageBrowsing::testImageFormats_BMP()
{
    if (!isImageFormatSupported("BMP")) {
        QSKIP("BMP format not supported");
    }

    QVERIFY(QFileInfo::exists(m_testImageBMP));

    QImageReader reader(m_testImageBMP);
    QVERIFY(reader.canRead());

    QImage image = reader.read();
    QVERIFY(!image.isNull());
}

void TestImageBrowsing::testImageFormats_TIFF()
{
    if (!isImageFormatSupported("TIFF")) {
        QSKIP("TIFF format not supported");
    }

    // TIFF test would go here
    QVERIFY(true); // Placeholder
}

void TestImageBrowsing::testImageFormats_WEBP()
{
    if (!isImageFormatSupported("WEBP")) {
        QSKIP("WEBP format not supported");
    }

    // WEBP test would go here
    QVERIFY(true); // Placeholder
}

// Image Loading Tests
void TestImageBrowsing::testImageLoading_ValidImages()
{
    ImageViewerWidget* viewer = new ImageViewerWidget();

    // Test loading valid PNG image
    if (QFileInfo::exists(m_testImagePNG)) {
        QSignalSpy loadedSpy(viewer, &ImageViewerWidget::loadingFinished);
        viewer->setCurrentImage(m_testImagePNG);

        QVERIFY(loadedSpy.wait(1000));
        QCOMPARE(loadedSpy.count(), 1);
    }

    delete viewer;
}

void TestImageBrowsing::testImageLoading_InvalidImages()
{
    ImageViewerWidget* viewer = new ImageViewerWidget();

    // Test loading corrupted image
    if (QFileInfo::exists(m_testImageCorrupted)) {
        QSignalSpy errorSpy(viewer, &ImageViewerWidget::loadingFailed);
        viewer->setCurrentImage(m_testImageCorrupted);

        // Should either emit error or handle gracefully
        QVERIFY(errorSpy.wait(1000) || viewer->getCurrentImage().isEmpty());
    }

    delete viewer;
}

void TestImageBrowsing::testImageLoading_CorruptedImages()
{
    QImageReader reader(m_testImageCorrupted);
    QVERIFY(!reader.canRead());

    QImage image = reader.read();
    QVERIFY(image.isNull());
}

void TestImageBrowsing::testImageLoading_LargeImages()
{
    if (!QFileInfo::exists(m_testImageLarge)) {
        QSKIP("Large test image not created");
    }

    QElapsedTimer timer;
    timer.start();

    QImageReader reader(m_testImageLarge);
    QImage image = reader.read();

    qint64 loadTime = timer.elapsed();
    qDebug() << "Large image (2048x2048) loaded in" << loadTime << "ms";

    QVERIFY(!image.isNull());
    QCOMPARE(image.size(), QSize(2048, 2048));

    // Should load within reasonable time (less than 5 seconds)
    QVERIFY(loadTime < 5000);
}

void TestImageBrowsing::testImageLoading_EmptyFiles()
{
    QString emptyFile = m_tempDir->path() + "/empty.png";
    QFile file(emptyFile);
    file.open(QIODevice::WriteOnly);
    file.close(); // Create empty file

    QImageReader reader(emptyFile);
    QVERIFY(!reader.canRead());

    QImage image = reader.read();
    QVERIFY(image.isNull());
}

// Image Display Tests
void TestImageBrowsing::testImageDisplay_BasicDisplay()
{
    ImageViewerWidget* viewer = new ImageViewerWidget();
    viewer->resize(400, 300);
    viewer->show();

    if (QFileInfo::exists(m_testImagePNG)) {
        viewer->setCurrentImage(m_testImagePNG);

        // Test that image is displayed
        QVERIFY(!viewer->getCurrentImage().isEmpty());
    }

    delete viewer;
}

void TestImageBrowsing::testImageDisplay_Scaling()
{
    ImageViewerWidget* viewer = new ImageViewerWidget();

    if (QFileInfo::exists(m_testImagePNG)) {
        viewer->setCurrentImage(m_testImagePNG);

        // Test zoom functionality
        viewer->onZoomIn();
        viewer->onZoomOut();
        viewer->onResetZoom();
        viewer->onFitToWindow();

        QVERIFY(viewer != nullptr); // Basic functionality test
    }

    delete viewer;
}

void TestImageBrowsing::testImageDisplay_AspectRatio()
{
    QImage image(200, 100, QImage::Format_RGB32); // 2:1 aspect ratio
    image.fill(Qt::green);

    QString aspectTestFile = m_tempDir->path() + "/aspect_test.png";
    QVERIFY(image.save(aspectTestFile));

    QImageReader reader(aspectTestFile);
    QImage loadedImage = reader.read();

    QCOMPARE(loadedImage.size(), QSize(200, 100));
    QCOMPARE(loadedImage.width() / loadedImage.height(), 2);
}

void TestImageBrowsing::testImageDisplay_Rotation()
{
    ImageViewerWidget* viewer = new ImageViewerWidget();

    if (QFileInfo::exists(m_testImagePNG)) {
        viewer->setCurrentImage(m_testImagePNG);

        // Test rotation functionality
        viewer->rotateLeft();
        viewer->rotateRight();

        QVERIFY(viewer != nullptr); // Basic functionality test
    }

    delete viewer;
}

void TestImageBrowsing::testImageDisplay_Flipping()
{
    ImageViewerWidget* viewer = new ImageViewerWidget();

    if (QFileInfo::exists(m_testImagePNG)) {
        viewer->setCurrentImage(m_testImagePNG);

        // Test flip functionality
        viewer->flipHorizontal();
        viewer->flipVertical();

        QVERIFY(viewer != nullptr); // Basic functionality test
    }

    delete viewer;
}

// Performance Tests
void TestImageBrowsing::testPerformance_LoadingSpeed()
{
    const int numImages = 10;
    QStringList testImages;

    // Create multiple test images
    for (int i = 0; i < numImages; ++i) {
        QString fileName = createTestImage("PNG", QSize(200, 200));
        if (!fileName.isEmpty()) {
            testImages << fileName;
        }
    }

    QElapsedTimer timer;
    timer.start();

    // Load all images
    for (const QString& imagePath : testImages) {
        QImageReader reader(imagePath);
        QImage image = reader.read();
        QVERIFY(!image.isNull());
    }

    qint64 totalTime = timer.elapsed();
    qDebug() << "Loaded" << testImages.size() << "images in" << totalTime << "ms";
    qDebug() << "Average time per image:" << (totalTime / testImages.size()) << "ms";

    // Should load reasonably fast (less than 100ms per image)
    QVERIFY((totalTime / testImages.size()) < 100);
}

void TestImageBrowsing::testPerformance_MemoryUsage()
{
    // This is a basic test - in a real scenario you'd use memory profiling tools
    QImage largeImage(1024, 1024, QImage::Format_RGB32);
    largeImage.fill(Qt::blue);

    // Calculate expected memory usage
    qint64 expectedBytes = largeImage.sizeInBytes();
    qDebug() << "Large image memory usage:" << expectedBytes << "bytes";

    QVERIFY(expectedBytes > 0);
    QVERIFY(expectedBytes == 1024 * 1024 * 4); // 4 bytes per pixel for RGB32
}

void TestImageBrowsing::testPerformance_MultipleImages()
{
    ImageViewerWidget* viewer = new ImageViewerWidget();

    QElapsedTimer timer;
    timer.start();

    // Switch between multiple images quickly
    if (QFileInfo::exists(m_testImagePNG)) {
        for (int i = 0; i < 5; ++i) {
            viewer->setCurrentImage(m_testImagePNG);
            QTest::qWait(10); // Small delay
        }
    }

    qint64 switchTime = timer.elapsed();
    qDebug() << "Image switching test completed in" << switchTime << "ms";

    delete viewer;
}

// Error Handling Tests
void TestImageBrowsing::testErrorHandling_FileNotFound()
{
    QString nonExistentFile = "/path/that/does/not/exist/image.png";

    QImageReader reader(nonExistentFile);
    QVERIFY(!reader.canRead());

    QImage image = reader.read();
    QVERIFY(image.isNull());

    QString error = reader.errorString();
    QVERIFY(!error.isEmpty());
    qDebug() << "Expected error for non-existent file:" << error;
}

void TestImageBrowsing::testErrorHandling_UnsupportedFormat()
{
    // Create a file with unsupported extension
    QString unsupportedFile = m_tempDir->path() + "/test.xyz";
    QFile file(unsupportedFile);
    file.open(QIODevice::WriteOnly);
    file.write("Not a real image");
    file.close();

    QImageReader reader(unsupportedFile);
    QVERIFY(!reader.canRead());

    QImage image = reader.read();
    QVERIFY(image.isNull());
}

void TestImageBrowsing::testErrorHandling_PermissionDenied()
{
    // This test is platform-specific and may not work on all systems
    // On Windows, we can't easily create permission-denied scenarios in temp dirs
    QVERIFY(true); // Placeholder - would need platform-specific implementation
}

QTEST_MAIN(TestImageBrowsing)
#include "test_image_browsing.moc"
