#ifndef XMLFORMCONTENTREADER_H
#define XMLFORMCONTENTREADER_H

#include <QString>
#include <QStringList>
#include <QCache>
#include <QHash>
#include <QDomDocument>
#include <QDomElement>

/**
 * \file xmlformcontentreader.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 10 May 2011
*/


namespace Form {
class FormItem;
class FormMain;
class IFormWidgetFactory;
class FormIODescription;
class FormIOQuery;
}

namespace XmlForms {

namespace Internal {

class XmlFormContentReader
{
    XmlFormContentReader();
public:
    static XmlFormContentReader *instance();
    ~XmlFormContentReader();

    void refreshPluginFactories();

    bool isInCache(const QString &formUid) const;

    void warnXmlReadError(bool muteUserWarnings, const QString &file, const QString &msg, const int line = -1, const int col = -1) const;

    bool checkFormFileContent(const QString &formUidOrFullAbsPath, const QString &contents) const;

    Form::FormIODescription *readFileInformations(const QString &formUidOrFullAbsPath);

    QList<Form::FormIODescription *> getFormFileDescriptions(const Form::FormIOQuery &query);

    bool loadForm(const QString &file, Form::FormMain *rootForm);

    bool loadElement(Form::FormItem *item, QDomElement &rootElement);
    bool createElement(Form::FormItem *item, QDomElement &element);

    bool populateValues(Form::FormItem *item, const QDomElement &root);
    bool populateScripts(Form::FormItem *item, const QDomElement &root);

    bool createItemWidget(Form::FormItem *item, QWidget *parent = 0);
    bool createFormWidget(Form::FormMain *form);
    bool createWidgets(const Form::FormMain *rootForm);

private:
    static XmlFormContentReader *m_Instance;
    QHash<QString, Form::IFormWidgetFactory *> m_PlugsFactories;
    mutable QStringList m_Error;
    bool m_Mute;
    Form::FormMain *m_ActualForm;

    // Caching some data for speed improvements
    mutable QHash<QString, bool> m_ReadableForms;
    mutable QCache<QString, QDomDocument> m_DomDocFormCache;

    // XML helpers
    QHash<QString, int> m_ScriptsTypes;
    QHash<QString, int> m_ValuesTypes;
    QHash<QString, int> m_SpecsTypes;
    QHash<QString, int> m_PatientDatas;
};

}  // End namespace Internal
}  // End namespace XmlForms

#endif // XMLFORMCONTENTREADER_H
