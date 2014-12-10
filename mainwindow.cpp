#include "mainwindow.h"
#include "ui_mainwindow.h"

#define TITLE_COLUMN            0
#define TRACK_NUMBER_COLUMN     1
#define ARTIST_COLUMN           2
#define ALBUM_COLUMN            3
#define FILE_PATH_COLUMN        4

#define COLUMN_COUNT            5

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_mediaPlayer(new QMediaPlayer),
    m_metaSongMap(new std::map <std::string, int>),
    m_acoustidMgr(),
    m_db("/home/tommy/apollo.db")
{
    m_ui->setupUi(this);
    createMediaTable();
    m_ui->tableWidget_media->setFocus();
    m_mediaPlayer->setVolume(m_ui->horizontalSlider_volume->value());

    connect(m_ui->lineEdit_root, SIGNAL(editingFinished()), this, SLOT(updateMediaList()));
    connect(m_ui->tableWidget_media, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(playCurrentSong()));
    connect(m_ui->pushButton_play, SIGNAL(clicked()), this, SLOT(playCurrentSong()));
    connect(m_ui->pushButton_stop, SIGNAL(clicked()), m_mediaPlayer, SLOT(stop()));
    //connect(m_ui->actionRoot, SIGNAL(triggered()), )

    // updateAcoustIDInfo(), when the network request "finished()"
    connect(&m_acoustidMgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(updateAcoustIDInfo(QNetworkReply*)));

    updateMediaList();
    changeCurrentSong();
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_mediaPlayer;
}

void MainWindow::createMediaTable()
{


    m_ui->tableWidget_media->setColumnCount(COLUMN_COUNT);

    QStringList header;
    header << "Title" << "Track" << "Artist"<< "Album" << "File Path";
    m_ui->tableWidget_media->setHorizontalHeaderLabels(header);
    m_ui->tableWidget_media->setColumnWidth(TITLE_COLUMN, 250);
    m_ui->tableWidget_media->setSortingEnabled(true);
    m_ui->tableWidget_media->horizontalHeader()->setVisible(true);

}

void MainWindow::changeCurrentSong() {
    if (m_ui->tableWidget_media->rowCount() > 0) {
        QString fileName = m_ui->tableWidget_media->item(m_ui->tableWidget_media->currentRow(), FILE_PATH_COLUMN)->text();
        m_mediaPlayer->setMedia(QMediaContent(QUrl::fromLocalFile(fileName)));
        Apollo::Song song(fileName.toStdString());
        m_ui->label_albumArt->clear();
        m_ui->label_albumArt->setText("no album art");

        if ((song.isTagValid()) && (song.getTag()->getDataString("ALBUM_ART").size() != 0)) {
            QPixmap art = song.getTag()->getAlbumArt();
            art = art.scaledToHeight(m_ui->label_albumArt->height());
            art = art.scaledToWidth(m_ui->label_albumArt->width());
            m_ui->label_albumArt->setPixmap(art);
        }

        m_ui->label_artist->setText("unknown artist");
        m_ui->label_album->setText("unknown album");
        if ((song.isTagValid()) && (song.getTag()->getDataString("TITLE").size() != 0)) {
            m_ui->label_title->setText(QString::fromStdString(song.getTag()->getDataString("TITLE")));
        } else {
            m_ui->label_title->setText(m_ui->tableWidget_media->item(m_ui->tableWidget_media->currentRow(), TITLE_COLUMN)->text());
        }
        if ((song.isTagValid()) && (song.getTag()->getDataString("ARTIST").size() != 0)) {
            m_ui->label_artist->setText(QString::fromStdString(song.getTag()->getDataString("ARTIST")));
        }
        if ((song.isTagValid()) && (song.getTag()->getDataString("ALBUM").size() != 0)) {
            m_ui->label_album->setText(QString::fromStdString(song.getTag()->getDataString("ALBUM")));
        }

        requestAcoustIDInfo(fileName.toStdString().c_str());

        // FIXME
        m_db.getSong(fileName);
    }
}

void MainWindow::requestAcoustIDInfo(const char * fileName) {
    m_ui->label_aid_acoustid->setText("<updating>");
    m_ui->label_aid_album->setText("<updating>");
    m_ui->label_aid_artists->setText("<updating>");
    m_ui->label_aid_date->setText("<updating>");
    m_ui->label_aid_duration->setText("<updating>");
    m_ui->label_aid_score->setText("<updating>");
    m_ui->label_aid_title->setText("<updating>");

    std::string lookup = getFingerprint(fileName);
    if (lookup.empty()) {
        m_ui->label_aid_acoustid->setText("<unknown>");
        m_ui->label_aid_album->setText("<unknown>");
        m_ui->label_aid_artists->setText("<unknown>");
        m_ui->label_aid_date->setText("<unknown>");
        m_ui->label_aid_duration->setText("<unknown>");
        m_ui->label_aid_score->setText("<unknown>");
        m_ui->label_aid_title->setText("<unknown>");
        return;
    }

    // the HTTP request
    QNetworkRequest req(QUrl(QString(lookup.c_str())));
    m_acoustidMgr.get(req);

    return;
}

void MainWindow::updateAcoustIDInfo(QNetworkReply * reply) {
    QJsonObject jsonObj;
    if (reply->error() == QNetworkReply::NoError) {
        QString replyString = reply->readAll();
        //qDebug() << "Success" << replyString;

        QJsonDocument jsonResponse = QJsonDocument::fromJson(replyString.toUtf8());
        jsonObj = jsonResponse.object();

        m_ui->label_aid_acoustid->setText(jsonObj["results"].toArray()[0].toObject()["id"].toString());
        m_ui->label_aid_album->setText(jsonObj["results"].toArray()[0].toObject()["recordings"].toArray()[0].toObject()["releasegroups"].toArray()[0].toObject()["title"].toString());
        m_ui->label_aid_artists->setText(jsonObj["results"].toArray()[0].toObject()["recordings"].toArray()[0].toObject()["artists"].toArray()[0].toObject()["name"].toString());
        m_ui->label_aid_date->setText(
            QString::number(jsonObj["results"].toArray()[0].toObject()["recordings"].toArray()[0].toObject()["releasegroups"].toArray()[0].toObject()["releases"].toArray()[0].toObject()["date"].toObject()["month"].toInt()) + "/" +
            QString::number(jsonObj["results"].toArray()[0].toObject()["recordings"].toArray()[0].toObject()["releasegroups"].toArray()[0].toObject()["releases"].toArray()[0].toObject()["date"].toObject()["day"].toInt()) + "/" +
            QString::number(jsonObj["results"].toArray()[0].toObject()["recordings"].toArray()[0].toObject()["releasegroups"].toArray()[0].toObject()["releases"].toArray()[0].toObject()["date"].toObject()["year"].toInt())
        );
        m_ui->label_aid_duration->setText(QString::number(jsonObj["results"].toArray()[0].toObject()["recordings"].toArray()[0].toObject()["duration"].toDouble()));
        m_ui->label_aid_score->setText(QString::number(jsonObj["results"].toArray()[0].toObject()["score"].toDouble()));
        m_ui->label_aid_title->setText(jsonObj["results"].toArray()[0].toObject()["recordings"].toArray()[0].toObject()["title"].toString());
    } else {
        m_ui->label_aid_acoustid->setText("<unknown>");
        m_ui->label_aid_album->setText("<unknown>");
        m_ui->label_aid_artists->setText("<unknown>");
        m_ui->label_aid_date->setText("<unknown>");
        m_ui->label_aid_duration->setText("<unknown>");
        m_ui->label_aid_score->setText("<unknown>");
        m_ui->label_aid_title->setText("<unknown>");
    }
}

void MainWindow::playCurrentSong()
{
    changeCurrentSong();
    m_mediaPlayer->play();
}

void MainWindow::updateMediaList()
{
    QDir musicDir(m_ui->lineEdit_root->text());
    if (musicDir.exists())
    {
        m_ui->label_root_error->setText("");
        QList<QFileInfo> songFileList = scanDirIter(musicDir);
        for(int i = 0; i < songFileList.length(); i++)
        {
            // Update progress bar
            m_ui->fileStatusProgressBar->setValue(int(100*float(i)/float(songFileList.length()-1)));
            m_ui->fileStatusLabel->setText(QString::fromStdString(std::to_string(i+1)+" of "+std::to_string(songFileList.length())));

            std::pair<std::map<std::string,int>::iterator,bool> ret;
            // FIXME remove metaSonMap and use db instead
            ret = m_metaSongMap->insert(std::pair<std::string,int>(songFileList[i].filePath().toStdString(), 1));
            if (ret.second == true)
            {
                // This is a new file, so process it
                int rowCount = m_ui->tableWidget_media->rowCount();
                m_ui->tableWidget_media->insertRow(rowCount);

                Apollo::Song song(songFileList[i].filePath().toStdString());

                QString filename, title, track_num, artist, album, album_art;
                filename = songFileList[i].filePath();
                title = songFileList[i].fileName();

                if (song.isTagValid())
                {
                    if (song.getTag()->getDataString("TITLE").size() != 0) {
                        title = QString::fromStdString(song.getTag()->getDataString("TITLE"));
                    }
                    if (song.getTag()->getDataString("ARTIST").size() != 0)
                    {
                        artist = QString::fromStdString(song.getTag()->getDataString("ARTIST"));
                    }
                    if (song.getTag()->getDataString("ALBUM").size() != 0)
                    {
                        album = QString::fromStdString(song.getTag()->getDataString("ALBUM"));
                    }
                    if (song.getTag()->getDataString("TRACK_NUMBER").size() != 0)
                    {
                        track_num = QString::fromStdString(song.getTag()->getDataString("TRACK_NUMBER"));
                    }
                }

                m_ui->tableWidget_media->setItem(rowCount, FILE_PATH_COLUMN,    new QTableWidgetItem(filename));
                m_ui->tableWidget_media->setItem(rowCount, TITLE_COLUMN,        new QTableWidgetItem(title));
                m_ui->tableWidget_media->setItem(rowCount, ARTIST_COLUMN,       new QTableWidgetItem(artist));
                m_ui->tableWidget_media->setItem(rowCount, ALBUM_COLUMN,        new QTableWidgetItem(album));
                m_ui->tableWidget_media->setItem(rowCount, TRACK_NUMBER_COLUMN, new QTableWidgetItem(track_num));

                m_db.insertSong(filename, title, track_num, artist, album, album_art);
            }
            else
            {
                //std::cout << "Already added the song " << songFileList[i].filePath().toStdString() << std::endl;
            }
        }
        m_ui->tableWidget_media->setCurrentCell(0,0);
        m_ui->tableWidget_media->setEditTriggers(0);
    }
    else
    {
        m_ui->label_root_error->setText("ERROR: Directory does not exist");
    }
}

QList<QFileInfo> MainWindow::scanDirIter(QDir dir)
{
    QList<QFileInfo> retList;
    QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        if (!iterator.fileInfo().isDir()) {
            retList.push_back(iterator.fileInfo());
        }
    }

    return retList;
}

void MainWindow::on_horizontalSlider_volume_valueChanged(int value)
{
    m_mediaPlayer->setVolume(value);
}

void MainWindow::on_tableWidget_media_customContextMenuRequested(const QPoint &pos)
{
    std::cout << "Right click: " << pos.x() << "," << pos.y() << std::endl;
}
