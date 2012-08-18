#ifndef PIXMAPBUTTON_H
#define PIXMAPBUTTON_H

#include <QPushButton>

namespace Patients {
/*!
 * \class PixmapButton
 * \brief This class provides a QPushButton with a displayed Pixmap on it.
 *
 * The normal Qt QPushButton only can store a QIcon and has ho pixmap property. This
 * can be unconvenient when using the button as display widget in a MVC pattern as
 * widget - a QDataWidgetMapper doesn't know how to handle a QPushbutton.
 * Here comes the PixmapButton. It still displays a QIcon, but stores a QPixmap underneath.
 * It also provides a \e pixmap property for easy interacting with a QDataWidgetMapper.
 *
 */
class PixmapButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)

public:
    explicit PixmapButton(QWidget* parent = 0);
    QPixmap pixmap() const;

signals:

public Q_SLOTS:
    void setPixmap(const QPixmap&);

private:
    QPixmap m_pixmap;
};

} // end Patients

#endif // PIXMAPBUTTON_H
