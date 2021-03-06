#pragma once
#include <QSqlTableModel>

class SqlConversationModel : public QSqlTableModel
{
    Q_OBJECT
    Q_PROPERTY( QString recipient READ recipient WRITE setRecipient NOTIFY recipientChanged )

public:
    SqlConversationModel( QObject *parent = nullptr );

    QString recipient() const;
    void setRecipient( const QString &recipient );

    QVariant data( const QModelIndex &index, int role ) const override;
    QHash< int, QByteArray > roleNames() const override;

    Q_INVOKABLE void sendMessage( const QString &recipient, const QString &message );
    Q_INVOKABLE void sendMessageAuthor( const QString &author, const QString &recipient, const QString &message );
    Q_INVOKABLE void updateModel();

signals:
    void recipientChanged();

private:
    QString recipient_;
};
