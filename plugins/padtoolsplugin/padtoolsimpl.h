#ifndef PADTOOLS_IMPL_H
#define PADTOOLS_IMPL_H

#include <coreplugin/ipadtools.h>

namespace PadTools {

	class PadToolsImpl : public Core::IPadTools
	{
		Q_OBJECT

	public:
		PadToolsImpl(QObject *parent = 0) : Core::IPadTools(parent) {}
		~PadToolsImpl() {}

		QString parse(const QString &templ, QMap<QString,QVariant> &tokens);

		QSyntaxHighlighter *createSyntaxHighlighter(QTextEdit *textEdit, QMap<QString,QVariant> &tokens);

	private:

	};

}

#endif
