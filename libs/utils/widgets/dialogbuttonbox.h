#ifndef DIALOGBUTTONBOX_H
#define DIALOGBUTTONBOX_H

#include <QDialogButtonBox>

class DialogButtonBox : public QDialogButtonBox
{
    Q_OBJECT
public:
    explicit DialogButtonBox(QWidget *parent = 0);

//    QAbstractButton *addButton(StandardButton button, const QString &trContext, const QString &unTrText, const QString &themedIconName);
    QAbstractButton *addButton(const QString &trContext, const QString &unTrText, QIcon &icon, ButtonRole role);

//    void addActionsFromMenu(const QString &menuUid, StandardButton button);

signals:

public slots:

};

#endif // DIALOGBUTTONBOX_H
