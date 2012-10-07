#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QTextEdit>
#include <QToolBar>

class TextEditor : public QWidget
{
    Q_OBJECT
public:
    explicit TextEditor(QWidget *parent = 0);
    
protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *e);

    virtual bool eventFilter(QObject *object, QEvent *event);

private:
    QTextEdit *_edit;
    QToolBar *_bar;
};

#endif // TEXTEDITOR_H
