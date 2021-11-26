#include "MyVideoSurface.h"

#include <QtWidgets>
#include <qabstractvideosurface.h>
#include <qvideosurfaceformat.h>

MyVideoSurface::MyVideoSurface( QObject* parent )
    : QAbstractVideoSurface( parent ), imageFormat( QImage::Format_Invalid )
{
}

QList<QVideoFrame::PixelFormat> MyVideoSurface::supportedPixelFormats(
    QAbstractVideoBuffer::HandleType handleType ) const
{
    Q_UNUSED( handleType );
    return QList<QVideoFrame::PixelFormat>()
           << QVideoFrame::Format_ARGB32 << QVideoFrame::Format_ARGB32_Premultiplied << QVideoFrame::Format_RGB32
           << QVideoFrame::Format_RGB24 << QVideoFrame::Format_RGB565 << QVideoFrame::Format_RGB555
           << QVideoFrame::Format_ARGB8565_Premultiplied << QVideoFrame::Format_BGRA32
           << QVideoFrame::Format_BGRA32_Premultiplied << QVideoFrame::Format_BGR32 << QVideoFrame::Format_BGR24
           << QVideoFrame::Format_BGR565 << QVideoFrame::Format_BGR555 << QVideoFrame::Format_BGRA5658_Premultiplied
           << QVideoFrame::Format_AYUV444 << QVideoFrame::Format_AYUV444_Premultiplied << QVideoFrame::Format_YUV444
           << QVideoFrame::Format_YUV420P << QVideoFrame::Format_YV12 << QVideoFrame::Format_UYVY
           << QVideoFrame::Format_YUYV << QVideoFrame::Format_NV12 << QVideoFrame::Format_NV21
           << QVideoFrame::Format_IMC1 << QVideoFrame::Format_IMC2 << QVideoFrame::Format_IMC3
           << QVideoFrame::Format_IMC4 << QVideoFrame::Format_Y8 << QVideoFrame::Format_Y16 << QVideoFrame::Format_Jpeg
           << QVideoFrame::Format_CameraRaw << QVideoFrame::Format_AdobeDng;
}

bool MyVideoSurface::isFormatSupported( const QVideoSurfaceFormat& format ) const
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat( format.pixelFormat() );
    const QSize size                 = format.frameSize();

    return imageFormat != QImage::Format_Invalid && !size.isEmpty()
           && format.handleType() == QAbstractVideoBuffer::NoHandle;
}

bool MyVideoSurface::start( const QVideoSurfaceFormat& format )
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat( format.pixelFormat() );
    const QSize size                 = format.frameSize();

    if ( imageFormat != QImage::Format_Invalid && !size.isEmpty() || true )
    {
        this->imageFormat = imageFormat;
        auto imageSize    = size;
        auto sourceRect   = format.viewport();

        QAbstractVideoSurface::start( format );

        return true;
    }
    else
    {
        return false;
    }
}

QImage imageFromVideoFrame( const QVideoFrame& f )
{
    QImage img;
    QVideoFrame& frame = const_cast<QVideoFrame&>( f );
    if ( !frame.isValid() || !frame.map( QAbstractVideoBuffer::ReadOnly ) )
        return img;
    img = QImage( frame.width(), frame.height(), QImage::Format_ARGB32 );
    for ( int y = 0; y < frame.height(); y++ )
    {
        for ( int x = 0; x < frame.width(); x++ )
        {
            const int xx = x >> 1;
            const int yy = y >> 1;
            const int Y  = frame.bits( 0 )[y * frame.bytesPerLine( 0 ) + x] - 16;
            const int U  = frame.bits( 1 )[yy * frame.bytesPerLine( 1 ) + xx] - 128;
            const int V  = frame.bits( 2 )[yy * frame.bytesPerLine( 2 ) + xx] - 128;
            const int r  = qBound( 0, ( 298 * Y + 409 * V + 128 ) >> 8, 255 );
            const int g  = qBound( 0, ( 298 * Y - 100 * U - 208 * V + 128 ) >> 8, 255 );
            const int b  = qBound( 0, ( 298 * Y + 516 * U + 128 ) >> 8, 255 );
            img.setPixel( x, y, qRgb( r, g, b ) );
        }
    }
    frame.unmap();
    return img;
}
bool MyVideoSurface::present( const QVideoFrame& frame )
{
    if ( frame.isValid() )
    {
        QVideoFrame cloneFrame( frame );
        cloneFrame.map( QAbstractVideoBuffer::ReadOnly );
        QImage image = imageFromVideoFrame( cloneFrame );

        emit frameAvailable( image ); // this is very important
        cloneFrame.unmap();

        return true;
    }
}
