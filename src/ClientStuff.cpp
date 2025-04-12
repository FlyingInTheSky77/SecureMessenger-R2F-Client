#include "include/ClientStuff.h"
#include "include/messagecode.h"

#include <QSqlQuery>
#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlError>
#include <QtQml>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>


static void connectToDatabase( QString user_login )
{
    QSqlDatabase database = QSqlDatabase::database();
    if ( !database.isValid() )
    {
        database = QSqlDatabase::addDatabase( "QSQLITE" );
        if ( !database.isValid() )
            qFatal( "Cannot add database: %s", qPrintable( database.lastError().text() ) );
    }

    const QDir writeDir = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );
    if ( !writeDir.mkpath(".") )
        qFatal( "Failed to create writable directory at %s", qPrintable( writeDir.absolutePath() ) );

    // Ensure that we have a writable location on all devices.
    const QString fileName = writeDir.absolutePath() + "/R2FMessenger-database.sqlite3" + user_login;
    // When using the SQLite driver, open() will create the SQLite database if it doesn't exist.
    database.setDatabaseName( fileName );
    if ( !database.open() )
    {
        qFatal( "Cannot open database: %s", qPrintable( database.lastError().text() ) );
        QFile::remove( fileName );
    }
}

static void createTable()
{
    if ( QSqlDatabase::database().tables().contains(QStringLiteral( "Contacts" ) ) )
    {
        return;
    }
    QSqlQuery query;
    if ( !query.exec(
        "CREATE TABLE IF NOT EXISTS 'Contacts' ("
        "   'name' TEXT NOT NULL,"
        "   'activityStatus' TEXT NOT NULL,"
        "   PRIMARY KEY(name)"
        ")") ) {
        qFatal( "Failed to query database: %s", qPrintable( query.lastError().text() ) );
    }
}

ClientStuff::ClientStuff()
    : QObject()
    , tcpSocket_{ std::make_unique < QTcpSocket >() }
    , timeoutTimer_{ std::make_unique < QTimer >() }
    , status_{ false }
    , inConversationPage_{ false }
{
    timeoutTimer_->setSingleShot( true );
    connect( timeoutTimer_.get(), &QTimer::timeout, this, &ClientStuff::connectionTimeout_slot );
    connect( tcpSocket_.get(), &QTcpSocket::disconnected, this, &ClientStuff::closeConnection_slot );
}

void ClientStuff::setConnectionInfo(const Config &config)
{
    host_ = config.ip;
    port_ = config.port;
}

void ClientStuff::connect2host( const QString& hostAddress, const int port )
{
    timeoutTimer_->start( 5000 );
    host_ = hostAddress;
    port_ = port;
    tcpSocket_->connectToHost( host_, port_ );

    connect( tcpSocket_.get(), &QTcpSocket::connected, this, &ClientStuff::connected_slot );
    connect( tcpSocket_.get(), &QTcpSocket::readyRead, this, &ClientStuff::readyRead_slot );
}

void ClientStuff::connect2host()
{
    timeoutTimer_->start( 5000 );
    tcpSocket_->connectToHost( host_, port_ );

    connect( tcpSocket_.get(), &QTcpSocket::connected, this, &ClientStuff::connected_slot );
    connect( tcpSocket_.get(), &QTcpSocket::readyRead, this, &ClientStuff::readyRead_slot );
}

void ClientStuff::connectionTimeout_slot()
{
    if( tcpSocket_->state() == QAbstractSocket::ConnectingState )
    {
        tcpSocket_->abort();
        emit tcpSocket_->errorOccurred( QAbstractSocket::SocketTimeoutError );
    }

    if ( !status_ )
        emit sendSystemInfo_signal( tr( "server didn't answer for 5 sec. Try connect one more time or letter" ) );
}

void ClientStuff::connected_slot()
{
    status_ = true;
    emit statusChanged_signal( status_ );
}

bool ClientStuff::getStatus()
{
    return status_;
}

void ClientStuff::contactRequest()
{
    const QJsonObject obj{ { "code", static_cast< int >( Client_Code::contacts_request ) } };
    const QJsonDocument doc( obj );
    const QByteArray jByte( doc.toJson( QJsonDocument::Compact ) );
    tcpSocket_->write( jByte );
    emit sendSystemInfo_signal( tr( "send contact request to server" ) );
}

void ClientStuff::readyRead_slot()
{
    const QByteArray data{ tcpSocket_->readAll() };
    QJsonParseError parseError;
    parseError.error = QJsonParseError::NoError;
    const QJsonDocument doc = QJsonDocument::fromJson( data, &parseError );
    if ( parseError.error )
    {
      emit hasReadSome_signal( tr( "we got an error: QJsonParseError" ) );
      return;
    }
    const QJsonObject obj{ doc.object() };
    letsSeeWhatWeGotFromServer( obj );
}

void ClientStuff::letsSeeWhatWeGotFromServer( const QJsonObject& obj )
{
    const QJsonValue code_json_value{ obj.value( "code" ) };
    const int code_from_server_int{ code_json_value.toInt() };
    Server_Code server_code{ static_cast< Server_Code >( code_from_server_int ) };
    switch ( server_code )
    {
            case Server_Code::user_online:
            {
            const QString obj_in_Qstring{ obj.value( "object" ).toString() };
            const QJsonObject decrypt_obj{ decryptQJsonObjFromEncryptQString( obj_in_Qstring ) };

            updateUserStatus( decrypt_obj );
            emit sendSystemInfo_signal( tr( "contacts list was updated: one contact is online againe" ) );
            break;
        }
        case Server_Code::registred_new_user:
        {
            const QString obj_in_Qstring{ obj.value( "object" ).toString() };
            const QJsonObject decrypt_obj{ decryptQJsonObjFromEncryptQString( obj_in_Qstring ) };

            addNewUserToContactList( decrypt_obj );
            emit sendSystemInfo_signal( tr( "contacts list was updated: new user added" ) );
            break;
        }
        case Server_Code::user_offline:
        {
            const QString obj_in_Qstring{ obj.value( "object" ).toString() };
            const QJsonObject decrypt_obj{ decryptQJsonObjFromEncryptQString( obj_in_Qstring ) };

            updateUserStatus( decrypt_obj );
            emit sendSystemInfo_signal( tr( "contacts list was updated: one contact disconnected" ) );
            break;
        }
        case Server_Code::authorization_failed:
        {
            const QString obj_in_Qstring{ obj.value( "object" ).toString() };
            const QJsonObject decrypt_obj{ decryptQJsonObjFromEncryptQString( obj_in_Qstring ) };
            const QString message{ decrypt_obj.value( "message" ).toString() };
            emit hasReadSome_signal( message );
            break;
        }
        case Server_Code::server_stopped:
        {
            emit hasReadSome_signal( tr( "server stopped" ) );
            break;
        }
        case Server_Code::authorization_successful:
        {
            emit hasReadSome_signal( tr( "authorization was successful!" ) );
            emit inSystem_signal();
            connectToDatabase( login_ );
            createTable();
            this->contactRequest();
            break;
        }
        case Server_Code::connection_established:
        {
            emit hasReadSome_signal( tr( "server: connection established" ) );
            this->secureSessionFirstStep();
            break;
        }
        case Server_Code::registration_successful:
        {
            const QString obj_in_Qstring{ obj.value( "object" ).toString() };
            const QJsonObject decrypt_obj{ decryptQJsonObjFromEncryptQString( obj_in_Qstring ) };
            const QString message{ decrypt_obj.value( "message" ).toString() };
            emit hasReadSome_signal( message );
            emit inSystem_signal();
            connectToDatabase( login_ );
            createTable();
            this->contactRequest();
            break;
        }
        case Server_Code::registration_failed:
        {
            const QString obj_in_Qstring{ obj.value( "object" ).toString() };
            const QJsonObject decrypt_obj{ decryptQJsonObjFromEncryptQString( obj_in_Qstring ) };
            const QString message{ decrypt_obj.value( "message" ).toString() };
            emit hasReadSome_signal( message );
            break;
        }
        case Server_Code::contacts_list:
        {
            const QString obj_in_Qstring{ obj.value( "object" ).toString() };
            const QJsonObject decrypt_obj{ decryptQJsonObjFromEncryptQString( obj_in_Qstring ) };
            updateContactList( decrypt_obj );
            emit sendSystemInfo_signal( tr( "contacts list recieved" ) );
            break;
        }
        case Server_Code::message_to_recipient:
        {
            const QString obj_in_Qstring{ obj.value( "object" ).toString() };
            const QJsonObject decrypt_obj{ decryptQJsonObjFromEncryptQString( obj_in_Qstring ) };
            processMessageFromContact( decrypt_obj );
            break;
        }
        case Server_Code::recipient_offline:
        {
            const QString obj_in_Qstring{ obj.value( "object" ).toString() };
            const QJsonObject decrypt_obj{ decryptQJsonObjFromEncryptQString( obj_in_Qstring ) };
            const QString message{ decrypt_obj.value( "message" ).toString() };
            emit recipientOffline_signal( message );
            break;
        }
        case Server_Code::secure_session_server_step:
        {
            const int intermediate_number_from_server = obj.value( "second_step" ).toInt();
            key_.calculateSessionKey( intermediate_number_from_server );
            emit hasReadSome_signal( tr( "secure session is start" ) );
            emit weAreConnected_signal();
            break;
        }
        case Server_Code::mistake:
        {
            const QString obj_in_Qstring = obj.value( "object" ).toString();
            const QJsonObject decrypt_obj = decryptQJsonObjFromEncryptQString( obj_in_Qstring );
            const QString message = decrypt_obj.value( "message" ).toString();
            emit hasReadSome_signal( tr( "mistake message from server: " ) + message );
            break;
        }
        default:
        {
            emit hasReadSome_signal( tr( "it is not clear what we got from the server" ) );
            const QJsonObject obj { { "message", tr ("it is not clear what we got from the server" ) } };
            sendToServer( Client_Code::mistake, obj );
        }
    }
}

void ClientStuff::processMessageFromContact( const QJsonObject& encrypt_object )
{
    const QString author{ encrypt_object.value( "author" ).toString() };
    const QString recipient{ "Me" };
    const QString timestamp{ encrypt_object.value( "timestamp" ).toString() };
    const QString message{ encrypt_object.value( "message" ).toString() };

    emit needNameinConversationWithNow_signal();

    if ( !inConversationPage_ )
    {
        QSqlQuery query;
        query.prepare( "INSERT INTO Conversations (author, recipient, timestamp, message)"
                   "VALUES(:author, :recipient, :timestamp, :message)");
        query.bindValue( ":author", author);
        query.bindValue( ":recipient", recipient );
        query.bindValue( ":timestamp", timestamp );
        query.bindValue( ":message", message );
        if( !query.exec() )
        {
            qDebug() << __FILE__ << __LINE__  << "enter recived message failed";
        }
    }
    else
    {
        emit someMessageFromAuthor_signal( author, timestamp, message );
        emit contactOnline_signal();
    }
}

void ClientStuff::updateContactList( const QJsonObject& jObjList )
{
   qDebug() << "ClientStuff::updateContactList";
   QSqlQuery query;
   QSet<QString> currentContactsFromServer;

   foreach ( QJsonValue jVal, jObjList )
   {
       const QJsonObject obj{ jVal.toObject() };
       const QString name{ obj.value( "login" ).toString() };
       const QString activityStatus{ obj.value( "activityStatus" ).toString() };

       if ( name == login_ )
       {
           //do not insert the client's name into the contact list
           continue;
       }

       currentContactsFromServer.insert(name);

       QSqlQuery checkQuery;
       checkQuery.prepare("SELECT COUNT(*) FROM Contacts WHERE name = :name");
       checkQuery.bindValue(":name", name);
       checkQuery.exec();
       checkQuery.next();

       if (checkQuery.value(0).toInt() > 0) {
           query.prepare("UPDATE Contacts SET activityStatus = :activityStatus WHERE name = :name");
       } else {
           query.prepare("INSERT INTO Contacts (name, activityStatus) VALUES (:name, :activityStatus)");
       }

       query.bindValue(":name", name);
       query.bindValue(":activityStatus", activityStatus);
       if (!query.exec()) {
           qDebug() << __FILE__ << __LINE__ << "DB insert/update failed";
       }
   }

   QSqlQuery selectQuery("SELECT name FROM Contacts");
   while (selectQuery.next()) {
       QString name = selectQuery.value(0).toString();
       if (!currentContactsFromServer.contains(name)) {
           query.prepare("UPDATE Contacts SET activityStatus = 'deleted' WHERE name = :name");
           query.bindValue(":name", name);
           if (!query.exec()) {
               qDebug() << __FILE__ << __LINE__ << "Failed to mark contact as deleted";
           }
       }
   }
}

void ClientStuff::addNewUserToContactList ( const QJsonObject& obj )
{
    const QString name{ obj.value( "login" ).toString() };
    const QString activityStatus{ obj.value( "activityStatus" ).toString() };

    QSqlQuery query;

    QString insertQuery = "INSERT INTO Contacts (name, activityStatus) VALUES (:name, :activityStatus)";
    query.prepare(insertQuery);
    query.bindValue(":name", name);
    query.bindValue(":activityStatus", activityStatus);

    if (query.exec()) {
        qDebug() << "Data added successfully!";
    } else {
        qDebug() << "Failed to add data:" << query.lastError().text();
    }

    emit updateQMLModelView_sigal();
}

void ClientStuff::removeUserFromContactList( const QString& name )
{
    QSqlQuery query;
    QString deleteQuery = "DELETE FROM Contacts WHERE name = :name";

    query.prepare( deleteQuery );
    query.bindValue( ":name", name );

    if ( query.exec() ) {
        qDebug() << "User" << name << "removed successfully from Contacts.";
    } else {
        qDebug() << "Failed to remove user:" << query.lastError().text();
    }

    emit updateQMLModelView_sigal();
}

void ClientStuff::updateUserStatus(const QJsonObject &obj)
{
    const QString name{ obj.value("login").toString() };
    const QString activityStatus{ obj.value("activityStatus").toString() };

    QSqlQuery query;

    QString updateQuery = "UPDATE Contacts SET activityStatus = :activityStatus WHERE name = :name";
    query.prepare(updateQuery);
    query.bindValue(":activityStatus", activityStatus);
    query.bindValue(":name", name);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            qDebug() << "Status updated successfully!";
        } else {
            qDebug() << "No matching user found to update.";
        }
    } else {
        qDebug() << "Failed to update status:" << query.lastError().text();
    }

    emit updateQMLModelView_sigal();
}

void ClientStuff::closeConnection_slot()
{
    timeoutTimer_->stop();
    disconnect( tcpSocket_.get(), &QTcpSocket::connected, 0, 0 );
    disconnect( tcpSocket_.get(), &QTcpSocket::readyRead, 0, 0 );
    bool shouldEmit{ false };

    switch ( tcpSocket_->state() )
    {
        case 0:
            tcpSocket_->disconnectFromHost();
            shouldEmit = true;
            break;
        case 2:
            tcpSocket_->abort();
            shouldEmit = true;
            break;
        default:
            tcpSocket_->abort();
    }
    if ( shouldEmit )
    {
        status_ = false;
        emit statusChanged_signal( status_ );
        emit serverStoped_signal();
    }
}

void ClientStuff::requestinConversationWithNow( const QString& name )
{
    inConversationWith_ = name;
}

void ClientStuff::resetInConversationPage()
{
    inConversationPage_ = false;
}

void ClientStuff::startConversationPage()
{
    inConversationPage_ = true;
}

int ClientStuff::sendToServer( const Client_Code& cur_code, const QJsonObject& cur_object )
{
    const QString encrypt_obj_in_Qstring{ cryptQJsonObjAndPutItInQString( cur_object ) };
    const QJsonObject obj{ { "code", static_cast< int >( cur_code ) },{ "object", encrypt_obj_in_Qstring } };
    const QJsonDocument doc( obj );
    const QByteArray jByte( doc.toJson( QJsonDocument::Compact ) );
    return tcpSocket_->write( jByte );
}

void ClientStuff::registrationRequest( const QString &login, const QString &password )
{
    login_ = login;
    QJsonObject obj { { "login", login },{ "password", password } };
    sendToServer( Client_Code::registration_request, obj );
}

void ClientStuff::authorizationRequest(const QString &login, const QString &password)
{
    login_ = login;
    QJsonObject obj { { "login", login },{ "password", password } };
    sendToServer( Client_Code::authorization_request, obj );
}

void ClientStuff::sendMessageToRecipient( const QString& recipient, const QString& message)
{
    const QString timestamp{ QDateTime::currentDateTime().toString( Qt::ISODate ) };
    const QJsonObject messageToRecipient{ { "author", login_ }, { "recipient", recipient }, { "timestamp", timestamp }, { "message", message } };
    sendToServer( Client_Code::message_to_recipient, messageToRecipient );
}

void ClientStuff::sendDirectlyToServer(const QString& message)
{
    const QJsonObject obj{ { "message", message } };
    sendToServer( Client_Code::message_to_server, obj );
}

void ClientStuff::secureSessionFirstStep()
{
    const int intermediate_number{ key_.getIntermediateNumberForServer() };
    const QJsonObject obj { { "code", static_cast< int >( Client_Code::secure_session_client_step ) }, { "first_step", intermediate_number } };
    const QJsonDocument doc( obj );
    const QByteArray jByte( doc.toJson( QJsonDocument::Compact ) );
    tcpSocket_->write( jByte );
}

std::string ClientStuff::encryptString( const std::string& befor_encrypt_QString )
{
    const int key{ key_.getSessionKey() };
    std::string result_string{};
    for ( const auto& symbol: befor_encrypt_QString )
    {
        result_string += symbol ^ key;
    }
    return result_string;
}

QByteArray ClientStuff::cryptQByteArray( const QByteArray& jByte, const int& key )
{
    std::vector< int > key_in_vector = getVectorDigitsFromNumber( key );
    int vector_size = key_in_vector.size();
    int index{};
    QByteArray jByte_after_crypt;
    foreach( auto item, jByte )
    {
        item = item ^ key_in_vector[index];
        index++;
        if ( index == vector_size )
        {
            index = 0;
        }
        jByte_after_crypt.append( item );
    }
    return jByte_after_crypt;
}

QString ClientStuff::cryptQJsonObjAndPutItInQString( const QJsonObject& obj )
{
    QJsonDocument doc( obj );
    const QByteArray jByte( doc.toJson( QJsonDocument::Compact ) );
    const int key{ key_.getSessionKey() };
    const QByteArray crypt_jByte{ cryptQByteArray( jByte, key ) };
    const QString crypt_obj_in_QString( crypt_jByte );
    return crypt_obj_in_QString;
}

QJsonObject ClientStuff::decryptQJsonObjFromEncryptQString( const QString& encrypt_QString )
{
    const QByteArray jByte{ encrypt_QString.toUtf8() };
    const int key{ key_.getSessionKey() };
    const QByteArray decrypt_jByte{ cryptQByteArray( jByte, key ) };
    QJsonDocument doc{ QJsonDocument::fromJson( decrypt_jByte ) };
    QJsonObject obj{ doc.object() };
    return obj;
}

std::vector< int > ClientStuff::getVectorDigitsFromNumber( int number )
{
    int size{};
    int temp{ number };
    while ( temp > 0 )
    {
        temp = temp / 10;
        size++;
    }
    temp = number;
    int ostatok{};
    std::vector< int > number_in_vector;
    for ( int i = 0; i < size; ++i )
    {
        ostatok = temp % 10;
        temp = temp / 10;
        number_in_vector.push_back( ostatok );
    }
    return number_in_vector;
}
