#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "include/BackEnd.h"
#include "include/SqlContactModel.h"
#include "include/SqlConversationModel.h"

#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlError>
#include <QtQml>
#include <QQuickView>
#include <QQmlContext>

int main( int argc, char *argv[] )
{
    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
    QGuiApplication app( argc, argv );

    qmlRegisterType< BackEnd >( "io.qt.Backend", 1, 0, "BackEnd" );
    qmlRegisterType< SqlContactModel >( "r2f.SqlContactModel", 1, 0, "SqlContactModel" );
    qmlRegisterType< SqlConversationModel >( "r2f.SqlConversationModel", 1, 0, "SqlConversationModel" );

    BackEnd BackEndObject;

    QQmlApplicationEngine engine;

    QQmlContext *context = engine.rootContext();
    context->setContextProperty( "backend", &BackEndObject );

    const QUrl url( QStringLiteral( "qrc:/qml/main.qml" ) );
    QObject::connect( &engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url]( QObject *obj, const QUrl &objUrl )
                     {
                            if ( !obj && url == objUrl )
                            QCoreApplication::exit( -1 );
                     }, Qt::QueuedConnection );
    engine.load( url );

    return app.exec();
}
