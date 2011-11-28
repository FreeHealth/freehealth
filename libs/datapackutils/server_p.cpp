#include <QDir>

#include "server_p.h"

using namespace DataPack;

ServerPrivate::ServerPrivate(const QUrl &url) : QObject()
{
	this->url = url;
}

void ServerPrivate::connectAndUpdate()
{
    // Forge the server config filename
    QUrl requestUrl = url;
    QDir dir(requestUrl.path());
    requestUrl.setPath(dir.filePath("server.conf.xml"));

    QNetworkRequest request(requestUrl); // TODO ADD THE FILES TO THE URL
    request.setRawHeader("User-Agent", "FreeMedForms"); // TODO specify the version?
    QNetworkReply *reply = networkAccessManager->get(request);
    connect(reply, SIGNAL(readyRead()), this, SLOT(requestReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(requestError(QNetworkReply::NetworkError)));
}


void ServerPrivate::requestReadyRead()
{
    // TODO
	qDebug("Ready to read");
}

void ServerPrivate::requestFinished()
{
	qDebug("Finished");
}

void ServerPrivate::requestError(QNetworkReply::NetworkError error)
{
    // TODO
    // Save the config file to the m_BackUpPath
	qDebug("error: %d", error);
}
