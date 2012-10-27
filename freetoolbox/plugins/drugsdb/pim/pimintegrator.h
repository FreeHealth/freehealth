#ifndef PIMINTEGRATOR_H
#define PIMINTEGRATOR_H

#include <QWidget>
#include <utils/qabstractxmltreemodel.h>
#include <coreplugin/itoolpage.h>

namespace DrugInfos {
namespace Internal {
class PimTreeModelPrivate;
}

class PimsTreePage : public Core::IToolPage
{
public:
    PimsTreePage(QObject *parent = 0) : Core::IToolPage(parent) { setObjectName("PimsTreePage"); }

    virtual QString id() const {return "PimsTreePage";}
    virtual QString name() const {return "PimsTreePage";}
    virtual QString category() const;
    virtual QIcon icon() const {return QIcon();}

    // widget will be deleted after the show
    virtual QWidget *createPage(QWidget *parent = 0);
};


class PimTreeModel : public Utils::QAbstractXmlTreeModel
{
public:
    enum DataRepresentation {
        NameOrTranslatedText = 0,
        TypeOrLanguage,
        Level,
        Review,
        ColumnCount
    };

    PimTreeModel(QObject *parent = 0);
    ~PimTreeModel();

    int columnCount(const QModelIndex &) const;

    QVariant data(const QModelIndex &index, int role) const;

};


namespace Ui {
    class PimIntegrator;
}

class PimIntegrator : public QWidget
{
    Q_OBJECT

public:
    explicit PimIntegrator(QWidget *parent = 0);
    ~PimIntegrator();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PimIntegrator *ui;
};

}  // End namespace DrugInfos

#endif // PIMINTEGRATOR_H
