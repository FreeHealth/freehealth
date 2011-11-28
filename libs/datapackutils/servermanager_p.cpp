#include <QDir>

#include "servermanager_p.h"

using namespace DataPack;

ServerManagerPrivate::ServerManagerPrivate() :
	networkAccessManager(new QNetworkAccessManager)
{
}

ServerManagerPrivate::~ServerManagerPrivate()
{
	delete networkAccessManager;
}

int ServerManagerPrivate::getServerIndex(const QUrl &url) const
{
	for (int i = 0; i < children().count(); i++)
		if (qobject_cast<Server*>(children()[i])->url() == url)
			return i;
    return -1;
}

bool ServerManagerPrivate::addServer(const QUrl &url)
{
    // check if a server already exists with the same URL
	foreach (QObject *child, children())
		if (qobject_cast<Server*>(child)->url() == url)
			return false;

	Server *server = new Server(url, this);
    return true;
}

void ServerManagerPrivate::connectAndUpdate(int index)
{
	qobject_cast<Server*>(children()[index])->connectAndUpdate();
}

