#pragma once

#include <QAbstractVideoSurface>

class MyVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT

public:
    MyVideoSurface( QObject* parent = 0 );

    QList<QVideoFrame::PixelFormat> supportedPixelFormats( QAbstractVideoBuffer::HandleType handleType
                                                           = QAbstractVideoBuffer::NoHandle ) const;
    bool isFormatSupported( const QVideoSurfaceFormat& format ) const;

    bool start( const QVideoSurfaceFormat& format );

    bool present( const QVideoFrame& frame );

private:
    QWidget* widget;
    QImage::Format imageFormat;

signals:
    void frameAvailable( QImage frame );
};
