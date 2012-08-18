#include "pixmapbutton.h"

using namespace Patients;
/*!
 * \brief PixmapButton::PixmapButton
 * \param parent Parent of the Button, reached through to QPushButton.
 *
 * Just calls the QPushButton constructor and initializes the internal Pixmap.
 */
PixmapButton::PixmapButton(QWidget *parent) :
    QPushButton(parent),
    m_pixmap(QPixmap())
{
}

QPixmap PixmapButton::pixmap() const
{
    return m_pixmap;
}

void PixmapButton::setPixmap(const QPixmap& pixmap)
{
    setIcon(QIcon(pixmap));
    m_pixmap = pixmap;
}
