#include "include/SqlConversationModel.h"

#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>

static const char *conversationsTableName = "Conversations";

static void createTable()
{
    if ( QSqlDatabase::database().tables().contains( conversationsTableName ) )
    {
        // The table already exists; we don't need to do anything
        return;
    }

    QSqlQuery query;
    if ( !query.exec(
        "CREATE TABLE IF NOT EXISTS 'Conversations' ("
        "'author' TEXT NOT NULL,"
        "'recipient' TEXT NOT NULL,"
        "'timestamp' TEXT NOT NULL,"
        "'message' TEXT NOT NULL,"
        "FOREIGN KEY('author') REFERENCES Contacts ( name ),"
        "FOREIGN KEY('recipient') REFERENCES Contacts ( name )"
        ")") )
    {
        qFatal( "Failed to query database: %s", qPrintable( query.lastError().text() ) );
    }
}

SqlConversationModel::SqlConversationModel( QObject *parent ) :
    QSqlTableModel( parent )
{
    createTable();
    setTable( conversationsTableName );
    setSort( 2, Qt::DescendingOrder );
    // Ensures that the model is sorted correctly after submitting a new row.
    setEditStrategy( QSqlTableModel::OnManualSubmit );
}

QString SqlConversationModel::recipient() const
{
    return recipient_;
}

void SqlConversationModel::setRecipient( const QString &recipient )
{
    if ( recipient == recipient_ )
        return;

    recipient_ = recipient;

    const QString filterString = QString::fromLatin1(
        "(recipient = '%1' AND author = 'Me') OR (recipient = 'Me' AND author='%1')" ).arg( recipient_ );
    setFilter( filterString );
    select();

    emit recipientChanged();
}

QVariant SqlConversationModel::data( const QModelIndex &index, int role ) const
{
    if ( role < Qt::UserRole )
        return QSqlTableModel::data( index, role );

    const QSqlRecord sqlRecord = record( index.row() );
    return sqlRecord.value( role - Qt::UserRole );
}

QHash<int, QByteArray> SqlConversationModel::roleNames() const
{
    QHash< int, QByteArray > names;
    names[Qt::UserRole] = "author";
    names[Qt::UserRole + 1] = "recipient";
    names[Qt::UserRole + 2] = "timestamp";
    names[Qt::UserRole + 3] = "message";
    return names;
}

void SqlConversationModel::sendMessage( const QString &recipient, const QString &message )
{
    const QString timestamp = QDateTime::currentDateTime().toString( Qt::ISODate );

    QSqlRecord newRecord = record();
    newRecord.setValue( "author", "Me" );
    newRecord.setValue( "recipient", recipient );
    newRecord.setValue( "timestamp", timestamp );
    newRecord.setValue( "message", message );
    if ( !insertRecord( rowCount(), newRecord ) )
    {
        qDebug() << __FILE__ << __LINE__  << "Failed to send message:" << lastError().text();
        return;
    }

    submitAll();
}

void SqlConversationModel::sendMessageAuthor( const QString &author, const QString &recipient, const QString &message )
{
    const QString timestamp = QDateTime::currentDateTime().toString( Qt::ISODate );

    QSqlRecord newRecord = record();
    newRecord.setValue( "author", author );
    newRecord.setValue( "recipient", recipient );
    newRecord.setValue( "timestamp", timestamp );
    newRecord.setValue( "message", message );
    if ( !insertRecord( rowCount(), newRecord ) )
    {
        qDebug() << __FILE__ << __LINE__ << "Failed to send message:" << lastError().text();
        return;
    }

    submitAll();
}

void SqlConversationModel::updateModel()
{
    QSqlDatabase::database().tables().contains( conversationsTableName );
    QSqlQuery query;
}
