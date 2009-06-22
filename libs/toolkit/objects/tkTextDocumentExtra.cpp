#include "tkTextDocumentExtra.h"

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
        qWarning() << "creating";
        m_Doc = new QTextDocument(m_Html);
        m_DocCreated = true;
    }
    return m_Doc;
}

void tkTextDocumentExtra::setTextWidth( qreal width )
{
    document()->setTextWidth(width);
}
