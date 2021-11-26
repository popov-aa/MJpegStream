#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QLabel>
#include <QMediaPlayer>
#include <QVideoSurfaceFormat>
#include <QVideoWidget>

#include "MyVideoSurface.h"

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    QCommandLineParser parser;
    QCommandLineOption inputOption( "input", "file or stream", "input", "file" );
    QCommandLineOption outputOption( "output", "widget or surface", "output", "widget" );

    parser.addOptions( { inputOption, outputOption } );
    parser.process( app.arguments() );

    QMediaPlayer* receiver = new QMediaPlayer;

    // Prepare input

    QUrl url;
    if ( parser.value( inputOption ) == "file" )
    {
        url.setUrl( "file://" + app.applicationDirPath() + "/cam7.avi" );
    }
    else if ( parser.value( inputOption ) == "stream" )
    {
        //        url.setUrl( "gst-pipeline: udpsrc port=20101 ! \
//                  application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! \
//                  xvimagesink name=qtvideosink" );

        url.setUrl( "gst-pipeline: udpsrc port=20101 ! \
                    application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! \
                    xvimagesink name=qtvideosink" );
    }
    else
    {
        qFatal( "Unknown input: %s", parser.value( inputOption ).toStdString().data() );
    }

    // Prepare output

    receiver->setMedia( url );

    if ( parser.value( outputOption ) == "widget" )
    {
        QVideoWidget* widget = new QVideoWidget();
        widget->setWindowTitle( "QVideoWidget" );
        widget->show();
        receiver->setVideoOutput( widget );
    }
    else if ( parser.value( outputOption ) == "surface" )
    {
        QLabel* widget = new QLabel();
        widget->setWindowTitle( "Output widget" );
        widget->show();
        MyVideoSurface* myVideoSurface = new MyVideoSurface( widget );
        receiver->setVideoOutput( myVideoSurface );
        QObject::connect( myVideoSurface, &MyVideoSurface::frameAvailable, [widget]( QImage frame ) {
            widget->setPixmap( QPixmap::fromImage( frame ) );
        } );
    }
    else
    {
        qFatal( "Unknown output: %s", parser.value( outputOption ).toStdString().data() );
    }

    receiver->play();
    return app.exec();
}
