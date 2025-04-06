#include "include/BackEnd.h"

BackEnd::BackEnd( QObject *parent )
    : QObject( parent )
    , client_ ( std::make_unique<ClientStuff>() )
{
    setStatus_slot( client_->getStatus() );

    connect( client_.get(), &ClientStuff::hasReadSome_signal, this, &BackEnd::receivedSomething_slot );
    connect( client_.get(), &ClientStuff::statusChanged_signal, this, &BackEnd::setStatus_slot );
    connect( client_.get(), &ClientStuff::sendSystemInfo_signal,this,&BackEnd::sendSystemInfoInQML_slot );
    connect( client_.get(), &ClientStuff::someMessageFromAuthor_signal, this, &BackEnd::someMessageFromAuthorReadyToQML_slot );
    connect( client_.get(), &ClientStuff::needNameinConversationWithNow_signal, this, &BackEnd::NeedNameinConversationWithNow_slot );
    connect( client_.get(), &ClientStuff::serverStoped_signal, this, &BackEnd::serverStoped_slot );
    connect( client_.get(), &ClientStuff::recipientOffline_signal, this, &BackEnd::recipientOffline_slot );
    connect( client_.get(), &ClientStuff::inSystem_signal, this, &BackEnd::inSystem_slot );
    connect( client_.get(), &ClientStuff::weAreConnected_signal, this, &BackEnd::weAreConnected_slot );
    connect( client_.get(), &ClientStuff::contactOnline_signal, this, &BackEnd::contactOnline_signal );
    connect( client_.get(), &ClientStuff::updateQMLModelView_sigal, this, &BackEnd::updateQMLModelView_sigal );
}

void BackEnd::setConnectionInfo( const Config& config )
{
    client_ -> setConnectionInfo( config );
}

void BackEnd::weAreConnected_slot()
{
    emit weAreConnected_signal();
}

void BackEnd::sendMessageToRecipientClicked( const QString& recipient, const QString& message )
{
    client_->sendMessageToRecipient( recipient, message );
}

bool BackEnd::getStatus()
{
    return client_->getStatus();
}

void BackEnd::setStatus_slot( bool newStatus )
{
    emit statusChanged_signal( newStatus );
}

void BackEnd::receivedSomething_slot( QString message )
{
    emit someMessage_signal( message );
    if ( message == tr ("server stopped" ) )
        disconnectClicked_slot();
}

void BackEnd::connectClicked( const QString hostAddress, QString port )
{
    int portInt = port.toInt();
    client_->connect2host( hostAddress, portInt );
}

void BackEnd::connectClicked()
{
    client_->connect2host();
}

void BackEnd::sendDirectlyToServerClicked( QString message )
{
    client_->sendDirectlyToServer( message );
}

void BackEnd::disconnectClicked_slot()
{
    emit serverStoped_signal();
    client_->closeConnection_slot();
}

void BackEnd::registerClicked( const QString &login, const QString &password )
{
    client_->registrationRequest( login, password );
}

void BackEnd::authorizationClicked( const QString &login, const QString &password )
{
    client_->authorizationRequest( login, password );
}

void BackEnd::setNameinConversationWithWriteNow( QString name )
{
    client_->requestinConversationWithNow( name );
}
void BackEnd::liveConversationPage()
{
    client_->resetInConversationPage();
}
void BackEnd::startConversationPage()
{
    client_->startConversationPage();
}

void BackEnd::someMessageFromAuthorReadyToQML_slot( QString author, QString timestamp, QString message )
{
    emit someMessageFromAuthorInQML_signal( author, timestamp, message );
}

void BackEnd::sendSystemInfoInQML_slot( QString system_message )
{
    emit systemMessage_signal( system_message );
}

void BackEnd::recipientOffline_slot( QString message )
{
    emit recipientOffline_signal( message );
}

void BackEnd::inSystem_slot()
{
  emit inSystem_signal();
}

void BackEnd::serverStoped_slot()
{
    emit serverStoped_signal();
}

void BackEnd::NeedNameinConversationWithNow_slot()
{
    emit needNameinConversationWithNow_signal();
}
