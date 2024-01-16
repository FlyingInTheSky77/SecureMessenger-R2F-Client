#pragma once
#include "ClientStuff.h"

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include <QJsonArray>
#include <memory>

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool currentStatus READ getStatus NOTIFY statusChanged_signal)

public:
    explicit BackEnd( QObject *parent = nullptr );
    bool getStatus();
    Q_INVOKABLE void registerClicked( const QString &login, const QString &password );
    Q_INVOKABLE void setNameinConversationWithWriteNow( QString name );
    Q_INVOKABLE void liveConversationPage();
    Q_INVOKABLE void startConversationPage();
    Q_INVOKABLE void authorizationClicked( const QString &login, const QString &password );
    Q_INVOKABLE void connectClicked( const QString hostAddress, QString portNumber );
    Q_INVOKABLE void sendDirectlyToServerClicked( QString message );
    Q_INVOKABLE void sendMessageToRecipientClicked( const QString& recipient, const QString& message );

signals:
    void statusChanged_signal( bool newStatus );
    void someMessage_signal( QString message );
    void systemMessage_signal( QString message );

    void sendMessageToRecipient_signal( QString recipient, QString message );
    void someMessageFromAuthorInQML_signal( QString author, QString timestamp, QString message );

    void weAreConnected_signal();
    void inSystem_signal();
    void serverStoped_signal();
    void recipientOffline_signal( QString message );
    void contactOnline_signal();

    void sendLoginName_signal( const QString& name );
    void needNameinConversationWithNow_signal();

public slots:
    void setStatus_slot( bool newStatus );
    void disconnectClicked_slot();
    void recipientOffline_slot( QString message );
    void inSystem_slot();
    void weAreConnected_slot();
    void serverStoped_slot();

    void receivedSomething_slot( QString message );   
    void someMessageFromAuthorReadyToQML_slot( QString author, QString timestamp, QString message );
    void NeedNameinConversationWithNow_slot();

private:
    std::unique_ptr< ClientStuff > client_;

    void sendSystemInfoInQML_slot( QString system_message );
};
