/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2014 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#ifndef FORMGLOBALMODEL_H
#define FORMGLOBALMODEL_H

#include <formmanagerplugin/iformitemdatafactory.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QAbstractItemModel>

namespace Form {
class FakeDataFactory;

class FakeData : public Form::IFormItemData
{
    friend class Form::FakeDataFactory;
public:
    virtual ~FakeData() {}

    virtual Form::FormItem *parentItem() const
    {
        Q_ASSERT(qobject_cast<Form::FormItem*>(parent()));
        return qobject_cast<Form::FormItem*>(parent());
    }

    virtual bool isModified() const {return false;}
    void setModified(bool modified) {Q_UNUSED(modified);}

    void setReadOnly(bool readOnly) {Q_UNUSED(readOnly);}
    bool isReadOnly() const {return false;}

    virtual void setData(const QVariant &data)  { m_data = data;}
    virtual QVariant data() const { return m_data; }

    virtual void setWidgetData(const QVariant &data) { m_data = data;}
    virtual QVariant widgetData() const {return m_data;}

    virtual void setStorableData(const QVariant &data) {}
    virtual QVariant storableData() const { return QVariant(); }

    virtual void setScriptData(const QVariant &data) {}
    virtual QVariant scriptData() const { return QVariant(); }

protected:
    explicit FakeData(Form::FormItem *parent) : IFormItemData(parent) {}

private:
    QVariant m_data;
};

class FakeDataFactory : public Form::IFormItemDataFactory
{
    Q_OBJECT
public:
    FakeDataFactory(QObject *parent = 0) : IFormItemDataFactory(parent) {}
    virtual ~FakeDataFactory() {}

    virtual bool initialize(const QStringList &arguments, QString *errorString) { return true; }
    virtual bool extensionInitialized() { return true; }
    virtual bool isInitialized() const { return true; }

    virtual QStringList providedItemDatas() const { return QStringList() << "fake"; }
    virtual Form::FakeData *createItemData(Form::FormItem *parent) { return new FakeData(parent); }
};

class FormGlobalModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    FormGlobalModel(QObject *parent = 0);
    virtual ~FormGlobalModel();

//    virtual bool initialize(const QStringList &arguments, QString *errorString);
//    virtual bool extensionInitialized();

    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

//    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex & index) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex & index) const;

//    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole);
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

//    bool insertColumn(int column, const QModelIndex & parent = QModelIndex());
//    bool insertColumns(int column, int count, const QModelIndex & parent = QModelIndex());
//    bool insertRow(int row, const QModelIndex & parent = QModelIndex());
//    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());

//    bool removeColumn(int column, const QModelIndex & parent = QModelIndex());
//    bool removeColumns(int column, int count, const QModelIndex & parent = QModelIndex());
//    bool removeRow(int row, const QModelIndex & parent = QModelIndex());
//    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

//    virtual FormIdentifiantModel *createIdentifiantModel();
private:
    FakeDataFactory *plugs;
    Form::FormItem *root;
};

//class CORE_EXPORT FormIdentifiantModel : public QAbstractItemModel
//{
//    Q_OBJECT
//public:
//    FormIdentifiantModel(QObject *parent = 0) : QAbstractItemModel(parent) {}
//    virtual ~FormIdentifiantModel() {}
//
//    int columnCount(const QModelIndex & parent = QModelIndex()) const;
//    int rowCount(const QModelIndex & parent = QModelIndex()) const;
//
//    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
//    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
//
//    Qt::ItemFlags flags(const QModelIndex & index) const;
//    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
//    QModelIndex parent(const QModelIndex & index) const;
//
//    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole);
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
//
//    bool insertColumn(int column, const QModelIndex & parent = QModelIndex());
//    bool insertColumns(int column, int count, const QModelIndex & parent = QModelIndex());
//    bool insertRow(int row, const QModelIndex & parent = QModelIndex());
//    bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
//
//    bool removeColumn(int column, const QModelIndex & parent = QModelIndex());
//    bool removeColumns(int column, int count, const QModelIndex & parent = QModelIndex());
//    bool removeRow(int row, const QModelIndex & parent = QModelIndex());
//    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
//
////    virtual FormRecordModel *createRecordModel();
//
//};

//class CORE_EXPORT FormRecordModel : public QAbstractItemModel
//{
//    Q_OBJECT
//public:
//    FormRecordModel(QObject *parent = 0) : QObject(parent) {}
//    virtual ~FormRecordModel() {}
//
//
////    virtual FormItemModel *createItemModel();
//
//};

//class CORE_EXPORT FormItemModel : public QAbstractItemModel
//{
//    Q_OBJECT
//public:
//    FormItemModel(QObject *parent = 0) : QObject(parent) {}
//    virtual ~FormItemModel() {}
//
//
//};

} // namespace Form

#endif // FORMGLOBALMODEL_H
