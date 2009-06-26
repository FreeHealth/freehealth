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
tkTextDocumentExtra::~tkTextDocumentExtra()
{
    if (m_Doc) delete m_Doc;
    m_Doc = 0;
}

QString tkTextDocumentExtra::toHtml() const
{
    if (m_Doc) {
        return document()->toHtml();
    }
    return m_Html;
}

QTextDocument *tkTextDocumentExtra::document() const
{
    if (!m_Doc) {
        m_Doc = new QTextDocument();
        m_Doc->setHtml(m_Html);
        m_DocCreated = true;
    }
    return m_Doc;
}

void tkTextDocumentExtra::setTextWidth( qreal width )
{
    document()->setTextWidth(width);
}
