#include "padtoolsimpl.h"

#include "pad_analyzer.h"
#include "pad_highlighter.h"

using namespace PadTools;

QString PadToolsImpl::parse(const QString &templ, QMap<QString,QVariant> &tokens, QList<Core::PadAnalyzerError> &errors)
{
	Q_UNUSED(tokens); //TMP

	PadAnalyzer analyzer;
	Pad *pad = analyzer.analyze(templ);

	return pad->run(tokens);
}

QSyntaxHighlighter *PadToolsImpl::createSyntaxHighlighter(QTextEdit *textEdit, QMap<QString,QVariant> &tokens)
{
	Q_UNUSED(tokens); //TMP

	return new PadHighlighter(textEdit);
}
