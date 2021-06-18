#include "include/SqlContactModel.h"

#include <QDebug>
#include <QString>
#include <QSqlError>
#include <QSqlQuery>

SqlContactModel::SqlContactModel( QObject *parent ) :
    QSqlQueryModel( parent )
{
    QSqlQuery query;

    if ( !query.exec( "SELECT name, activityStatus FROM Contacts" ) )
        qFatal( "Contacts SELECT query failed: %s", qPrintable(query.lastError().text() ) );

    setQuery( query );
    if ( lastError().isValid() )
        qFatal( "Cannot set query on SqlContactModel: %s", qPrintable(lastError().text() ) );
}
