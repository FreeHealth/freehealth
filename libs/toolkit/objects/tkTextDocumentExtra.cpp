#include "tkTextDocumentExtra.h"

/**
  \class tkTextDocumentExtra
  \brief This class is used by tkPrinter for header/footer/watermark management.
  Xml structure looks like :\n
  <tkConstants::TKDOCUMENT_GENERAL_XML_TAG> </idem>\n
    <tkConstants::TKDOCUMENT_VERSION_XML_TAG> </idem>\n
    <tkConstants::TKDOCUMENT_PRESENCE_XML_TAG> </idem>\n
    <tkConstants::TKDOCUMENT_PRIORITY_XML_TAG> </idem>\n
    <tkConstants::TKDOCUMENT_EXTRA_XML_TAG> </idem>\n
    <tkConstants::TKDOCUMENT_HTML_XML_TAG> </idem>\n
  </tkConstants::TKDOCUMENT_GENERAL_XML_TAG>\n
  You can :
  \li get a pointer to the referenced QTextDocument using document().\n
  \li set the Presence of the document according to tkPrinter::Presence\n
  \li set the Priority of the document according to tkPrinter::Priority\n
  \li get/set to/from Xml using toXml() , fromXml()\n

  \sa tkPrinter
  \ingroup object_toolkit
*/

class tkTextDocumentExtraPrivate {
public:
    tkTextDocumentExtraPrivate() : m_Doc(0) {}

    tkPrinter::Presence  m_Presence;
    tkPrinter::Priority  m_Priority;
    QString xmlVersion;
    QString m_Html;
    mutable bool m_DocCreated;
    mutable QTextDocument *m_Doc;
};

tkTextDocumentExtra::tkTextDocumentExtra() : d(0)
{
    d = new tkTextDocumentExtraPrivate();
    d->xmlVersion = TKDOCUMENT_XML_ACTUALVERSION;
    d->m_Html = "";
    d->m_Priority = tkPrinter::First;
    d->m_Presence = tkPrinter::EachPages;
    d->m_DocCreated = false;
}

tkTextDocumentExtra::tkTextDocumentExtra( const QString &html, const int presence, const int priority, const QString &version) : d(0)
{
    d = new tkTextDocumentExtraPrivate();
    if (version.isEmpty())
        d->xmlVersion = TKDOCUMENT_XML_ACTUALVERSION;
    else
        d->xmlVersion = version;
    d->m_Priority = tkPrinter::Priority(priority);
    d->m_Presence = tkPrinter::Presence(presence);
    d->m_Html = html;
    d->m_DocCreated = false;
}

tkTextDocumentExtra::~tkTextDocumentExtra()
{
    if (d) delete d;
    d = 0;
}

void tkTextDocumentExtra::setPriority( tkPrinter::Priority p )
{ d->m_Priority = p; }

void tkTextDocumentExtra::setPresence( tkPrinter::Presence p )
{ d->m_Presence = p; }

void tkTextDocumentExtra::setHtml( const QString &html)
{
    d->m_Html = html;
    if (d->m_DocCreated)
        d->m_Doc->setHtml(html);
}

tkPrinter::Priority tkTextDocumentExtra::priority() const
{ return d->m_Priority; }

tkPrinter::Presence tkTextDocumentExtra::presence() const
{ return d->m_Presence; }

bool tkTextDocumentExtra::lessThan( const tkTextDocumentExtra *h1, const tkTextDocumentExtra *h2 )
{
    /** \todo  ? */
    return true;
}

QString tkTextDocumentExtra::toXml() const
{
    QHash<QString,QString> t;
    t.insert(TKDOCUMENT_VERSION_XML_TAG, d->xmlVersion);
    t.insert(TKDOCUMENT_PRESENCE_XML_TAG,QString::number(presence()));
    t.insert(TKDOCUMENT_PRIORITY_XML_TAG, QString::number(priority()));
    t.insert(TKDOCUMENT_EXTRA_XML_TAG, "");
    t.insert(TKDOCUMENT_HTML_XML_TAG, tkGlobal::toHtmlAccent(toHtml()));
    return tkGlobal::createXml(TKDOCUMENT_GENERAL_XML_TAG,t, 4, false);
}

tkTextDocumentExtra *tkTextDocumentExtra::fromXml(const QString &xml)
{
    if (!xml.contains(QString("<%1>").arg(TKDOCUMENT_GENERAL_XML_TAG)))
        return new tkTextDocumentExtra();
    QHash<QString, QString> t;
    if (!tkGlobal::readXml(xml, TKDOCUMENT_GENERAL_XML_TAG, t, false))
        return new tkTextDocumentExtra();
    return new tkTextDocumentExtra(t.value(TKDOCUMENT_HTML_XML_TAG),
                                   tkPrinter::Presence(t.value(TKDOCUMENT_PRESENCE_XML_TAG).toInt()),
                                   tkPrinter::Priority(t.value(TKDOCUMENT_PRIORITY_XML_TAG).toInt()),
                                   t.value(TKDOCUMENT_VERSION_XML_TAG));
}


QString tkTextDocumentExtra::toHtml() const
{
    if (d->m_Doc) {
        return document()->toHtml();
    }
    return d->m_Html;
}

QTextDocument *tkTextDocumentExtra::document() const
{
    if (!d->m_Doc) {
        d->m_Doc = new QTextDocument();
        d->m_Doc->setHtml(d->m_Html);
        d->m_DocCreated = true;
    }
    return d->m_Doc;
}

void tkTextDocumentExtra::setTextWidth( qreal width )
{
    document()->setTextWidth(width);
}
