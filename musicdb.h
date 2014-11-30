#ifndef MUSICDB_H
#define MUSICDB_H

#include <QObject>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <string>
#include <iostream>

namespace Apollo {

class MusicDB : public QObject
{
public:
    MusicDB(const QString);
    ~MusicDB();

public:
    bool openDB(const QString);
    bool createSongTable();
    int insertSong(QString, QString);
    bool getSong(QString);
    bool deleteDB();
    QSqlError lastError();
    bool isValid();

private:
    QSqlDatabase m_db;
    QString m_path;
    bool m_valid;
};

} // namespace Apollo

#endif // MUSICDB_H
