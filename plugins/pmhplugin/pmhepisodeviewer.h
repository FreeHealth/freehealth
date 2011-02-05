#ifndef PMHEPISODEVIEWER_H
#define PMHEPISODEVIEWER_H

#include <QWidget>
class QModelIndex;

namespace PMH {
namespace Internal {
class PmhEpisodeViewerPrivate;
class PmhData;
}

namespace Ui {
    class PmhEpisodeViewer;
}

class PmhEpisodeViewer : public QWidget
{
    Q_OBJECT

public:
    explicit PmhEpisodeViewer(QWidget *parent = 0);
    ~PmhEpisodeViewer();

    void setPmhData(Internal::PmhData *pmh);

private Q_SLOTS:
    void itemActivated(const QModelIndex &item);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PmhEpisodeViewer *ui;
    Internal::PmhEpisodeViewerPrivate *d;
};

}  // End namespace PMH

#endif // PMHEPISODEVIEWER_H
