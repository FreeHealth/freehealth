#ifndef XMLALERT_H
#define XMLALERT_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>


class XmlAlert : public QObject
{
    Q_OBJECT
    public:
        XmlAlert(QObject * parent = 0);
        ~XmlAlert();
};

class XmlWriter : public QXmlStreamWriter
{
    //Q_OBJECT
    public:
        XmlWriter();
        ~XmlWriter();
};

class XmlReader : public QXmlStreamReader
{
    //Q_OBJECT
    public:
        XmlReader();
        ~XmlReader();
};

#endif

