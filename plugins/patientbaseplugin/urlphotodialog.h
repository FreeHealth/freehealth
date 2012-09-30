#ifndef PATIENTS_URLPHOTODIALOG_H
#define PATIENTS_URLPHOTODIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QBuffer>
#include <QNetworkReply>

class QPushButton;

namespace Patients {

namespace Ui {
class UrlPhotoDialog;
}

class UrlPhotoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UrlPhotoDialog(QWidget *parent = 0);
    ~UrlPhotoDialog();

    QPixmap photo() const;

private:
    Ui::UrlPhotoDialog *ui;
    QPushButton *m_OkButton;
    int m_httpGetId;
    QNetworkAccessManager *m_netAccMan;
    QNetworkReply *m_reply;
    QByteArray m_picture;
    bool m_alreadyDownloading, m_alreadyUrlChecking;

private Q_SLOTS:
    void on_urlChanged(const QString &url);
    void downloadRequested();
    void requestFinished(QNetworkReply *reply);
    void progress(qint64 done, qint64 total);
};


} // namespace Patients
#endif // PATIENTS_URLPHOTODIALOG_H
