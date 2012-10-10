#ifndef FINDFILE_H
#define FINDFILE_H

#include <QtGui>
#include <QtCore>

class FindFile : public QDialog{
  Q_OBJECT
  public :
    FindFile(QObject * object);
    ~FindFile();
     QString fileRecord();
     QString findFilePath();
     QDialog *dialog();
  private Q_SLOTS :
    void recupere(const QModelIndex &);
  private :
    QDialog          *m_dialog;
    QTreeView        *m_view ;
    QFileSystemModel *m_sys  ;
    QPushButton      *m_pushButton;
    QGridLayout      *m_grid;
    QString           m_fileName;
    QString           m_filePath;

};

#endif
