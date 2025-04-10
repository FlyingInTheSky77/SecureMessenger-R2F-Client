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

void SqlContactModel::refresh()
{
    QSqlQuery query;

    if ( !query.exec( "SELECT name, activityStatus FROM Contacts" ) )
        qFatal( "Contacts SELECT query failed: %s", qPrintable(query.lastError().text() ) );

    setQuery( query );
    if ( lastError().isValid() )
        qFatal( "Cannot set query on SqlContactModel: %s", qPrintable(lastError().text() ) );
}

QHash<int, QByteArray> SqlContactModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Qt::UserRole + 1] = "name";
    roles[Qt::UserRole + 2] = "activityStatus";
    return roles;
}

QVariant SqlContactModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
            QString name = QSqlQueryModel::data(this->index(index.row(), 0)).toString();
            QString status = QSqlQueryModel::data(this->index(index.row(), 1)).toString();
            if ( status == "1" )  {
                status = "online";
            }
            else if ( status == "0" ) {
                status = "offline";
            }
            return QString("<b>%1</b> <span style='color:gray;'>[ status - %2 ]</span>").arg(name, status);
        }
    else if (role == Qt::UserRole + 1) {
        return QSqlQueryModel::data(this->index(index.row(), 0)); // name
    } else if (role == Qt::UserRole + 2) {
        return QSqlQueryModel::data(this->index(index.row(), 1)); // activityStatus
    }

    return QVariant();
}
