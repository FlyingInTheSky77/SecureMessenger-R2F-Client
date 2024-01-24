#pragma once
#include <QSqlQueryModel>

class SqlContactModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    SqlContactModel( QObject *parent = nullptr );
    Q_INVOKABLE void refresh();

};
