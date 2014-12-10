#include "musicdb.h"

namespace Apollo {

MusicDB::MusicDB(const QString path) :
    m_path(path),
    m_valid(false)
{
    if(!openDB(m_path)) {
        std::cerr << "Could not open database using path " << m_path.toStdString() << std::endl;
        std::cerr << lastError().text().toStdString() << std::endl;
        return;
    }

    if (!createSongTable()) {
        std::cerr << "Could not create song table" << std::endl;
        return;
    }

    m_valid = true;
}

MusicDB::~MusicDB() {
    // FIXME delete database for now to keep everything clean
    deleteDB();
}

bool MusicDB::openDB(const QString path) {
    m_path = path;
    // Find QSLite driver
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_path);

    // // NOTE: We have to store database file into user home folder in Linux
    // QString path(QDir::home().path());
    // path.append(QDir::separator()).append("my.db.sqlite");
    // path = QDir::toNativeSeparators(path);

    // Open databasee
    return m_db.open();
}

bool MusicDB::createSongTable() {
    bool ret = false;
    if (m_db.isOpen()) {
        QSqlQuery query;
        ret = query.exec("create table songs "
                "("
                    "filename  varchar(100), "
                    "title     varchar(100), "
                    "track_num varchar(100), "
                    "artist    varchar(100), "
                    "album     varchar(100), "
                    "album_art varchar(100) "
                ")");

    }
    return ret;
}

int MusicDB::insertSong(QString filename, QString title, QString track_num, QString artist, QString album, QString album_art) {
    int newId = -1;
    bool ret = false;

    if (m_db.isOpen()) {
        if (getSong(filename)) {
            std::cout << "File " << filename.toStdString() << " already added" << std::endl;
        } else {
            QSqlQuery query;
            ret = query.exec(QString("insert into songs values('%1','%2','%3','%4','%5','%6')").arg(filename).arg(title).arg(track_num).arg(artist).arg(album).arg(album_art));

            // Get database given autoincrement value
            if (ret) {
                // http://www.sqlite.org/c3ref/last_insert_rowid.html
                newId = query.lastInsertId().toInt();
            }
        }
    }
    return newId;
}

bool MusicDB::getSong(QString filename) {
    bool ret = false;

    QSqlQuery query(QString("select * from songs where filename = '%1'").arg(filename));
    if (query.next()) {
        std::cout << "filename " << query.value("filename").toString().toStdString() << std::endl;
        std::cout << "title " << query.value("title").toString().toStdString() << std::endl;
        std::cout << "track_num " << query.value("track_num").toString().toStdString() << std::endl;
        std::cout << "artist " << query.value("artist").toString().toStdString() << std::endl;
        std::cout << "album " << query.value("album").toString().toStdString() << std::endl;
        std::cout << "album_art " << query.value("album_art").toString().toStdString() << std::endl;
        ret = true;
    } else {
        //std::cout << "nothing found" << std::endl;
        //std::cerr << query.lastError().text().toStdString() << std::endl;
    }

    return ret;
}

QSqlError MusicDB::lastError() {
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return m_db.lastError();
}

bool MusicDB::deleteDB() {
    // Close database
    m_db.close();
    return QFile::remove(m_path);
}

bool MusicDB::isValid() {
    return m_valid;
}

} // namespace Apollo
