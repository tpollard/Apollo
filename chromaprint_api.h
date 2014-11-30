#ifndef CHROMAPRINT_API_H
#define CHROMAPRINT_API_H

extern "C"
{
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavresample/avresample.h>

#include <chromaprint.h>
};

#include <iostream>
#include <sstream>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

std::string getFingerprint(const char * fileName);

#endif // CHROMAPRINT_API_H
