#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <QMainWindow>
#include "song.h"
#include <QDir>
#include <QDirIterator>
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <map>
#include <chromaprint.h>
#include <QAudioDecoder>
#include "chromaprint_api.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void playCurrentSong();
    void updateMediaList();
    void updateAcoustIDInfo(QNetworkReply *);
    void on_horizontalSlider_volume_valueChanged(int value);

    void on_tableWidget_media_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow * m_ui;
    QMediaPlayer * m_mediaPlayer;
    std::map <std::string, int> * m_metaSongMap;
    QNetworkAccessManager m_acoustidMgr;

    QList<QFileInfo> scanDirIter(QDir dir);
    void changeCurrentSong();
    void createMediaTable();
    void requestAcoustIDInfo(const char *);
};

#endif // MAINWINDOW_H
