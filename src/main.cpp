#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "include/BackEnd.h"
#include "include/ConfigLoader.h"
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

    ConfigLoader configLoader( findConfigPath() );
    auto configOpt = configLoader.loadConfig();
    if ( !configOpt.has_value() ) {
        qWarning() << "Configuration is corrupted or missing";
        return -1;
    }

    const Config config = configOpt.value();

    BackEnd BackEndObject;
    BackEndObject.setConnectionInfo(config);

    QQmlApplicationEngine engine;

    QQmlContext *context = engine.rootContext();
    context->setContextProperty( "backend", &BackEndObject );

    #ifdef APP_VARIANT_BASIC
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    #else
    const QUrl url(QStringLiteral("qrc:/qml/mainCustom.qml"));
    #endif

    QObject::connect( &engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url]( QObject *obj, const QUrl &objUrl )
                     {
                            if ( !obj && url == objUrl )
                            QCoreApplication::exit( -1 );
                     }, Qt::QueuedConnection );
    engine.load( url );

    return app.exec();
}
