#pragma once
#include "SessionKey.h"
#include "messagecode.h"

#include <QString>
#include <QTcpSocket>
#include <QDataStream>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QJsonArray>

class ClientStuff : public QObject
{
    Q_OBJECT

public:
    ClientStuff();
    bool getStatus();
    void contactRequest();
    void requestinConversationWithNow( const QString& name );
    void resetInConversationPage();
    void startConversationPage();

    void connect2host( const QString& hostAddress, int portNumber );

    void registrationRequest( const QString& login, const QString& password );
    void authorizationRequest( const QString& login, const QString& password );
    void sendMessageToRecipient( const QString& recipient, const QString& message );
    void sendDirectlyToServer( const QString& message);

    void secureSessionFirstStep();    
    int sendToServer( const Client_Code& cur_code, const QJsonObject& cur_object );

signals:
    void statusChanged_signal( bool newStatus );
    void statusLoginChanged_signal( bool newLoginStatus );
    void hasReadSome_signal( QString server_message );
    void sendSystemInfo_signal( QString system_message );
    void SendToServerSignalOfClientStuff_signal( QByteArray jByte );
    void someMessageFromAuthor_signal( QString author, QString timestamp, QString message );
    void inSystem_signal();
    void weAreConnected_signal();

    void needNameinConversationWithNow_signal();
    void serverStoped_signal();
    void contactOnline_signal();

    void recipientOffline_signal( QString message );

public slots:
    void closeConnection_slot();

private slots:
    void readyRead_slot();
    void connected_slot();
    void connectionTimeout_slot();

private:
    std::unique_ptr< QTcpSocket > tcpSocket_;
    std::unique_ptr< QTimer > timeoutTimer_;
    bool status_;
    QString login_;
    QString host_;
    int portNumber_;
    QString inConversationWith_;
    bool inConversationPage_;
    SessionKey key_;

    void processMessageFromContact( const QJsonObject& encrypt_object );
    void updateContactList( const QJsonObject& jObjList );
    void letsSeeWhatWeGotFromServer( const QJsonObject& obj );

    std::string encryptString( const std::string& befor_encrypt_QString );
    QByteArray cryptQByteArray ( const QByteArray& jByte, const int& key );
    QString cryptQJsonObjAndPutItInQString( const QJsonObject& obj );
    QJsonObject decryptQJsonObjFromEncryptQString( const QString& encrypt_QString );
    std::vector< int > getVectorDigitsFromNumber( int number );
};
