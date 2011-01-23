#include "padtoolsimpl.h"

#include "pad_highlighter.h"

using namespace PadTools;

QString PadToolsImpl::parse(const QString &templ, QMap<QString,QVariant> &tokens)
{
	Q_UNUSED(templ); //TMP
	Q_UNUSED(tokens); //TMP

	return "";
}

QSyntaxHighlighter *PadToolsImpl::createSyntaxHighlighter(QTextEdit *textEdit, QMap<QString,QVariant> &tokens)
{
	Q_UNUSED(tokens); //TMP

	return new PadHighlighter(textEdit);
}
