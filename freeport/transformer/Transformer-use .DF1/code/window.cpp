#include <QFileDialog>
#include <QtGui>
#include "window.h"
#include <QDebug>
#include <QMainWindow>
#include "saxhandler.h"
#include "patient.h"
#include "xmlwriter.h"

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{

    browseButton2 = createButton(tr("&Browse..."), SLOT(browseDest()));
    OKButton = createButton(tr("&OK"), SLOT(printStartEnd()));

    directoryLabel = new QLabel(tr("<i><b>/!\\ The \"AXISANTE.DF1\" File must be in the same Directory than the Transformer </b></i>"));

    destinationLabel = new QLabel(tr("Where to save the result File ? "));
    destinationComboBox = createComboBox(QDir::currentPath());

    QHBoxLayout *chosenfileLayout = new QHBoxLayout;
    chosenfileLayout->addWidget(directoryLabel);

    QHBoxLayout *OKLayout = new QHBoxLayout;
    OKLayout->addStretch();
    OKLayout->addWidget(OKButton);

    QHBoxLayout *destinationfileLayout = new QHBoxLayout;
    destinationfileLayout->addWidget(destinationLabel);
    destinationfileLayout->addWidget(destinationComboBox);
    destinationfileLayout->addWidget(browseButton2);

    QWidget* q = new QWidget();
    setCentralWidget(q);
    QVBoxLayout * mainLayout = new QVBoxLayout();
      q->setLayout(mainLayout);
      mainLayout->addLayout(chosenfileLayout);
      mainLayout->addLayout(destinationfileLayout);
      mainLayout->addLayout(OKLayout);

    setWindowTitle(tr("The Axisante4 Tranformer"));
    resize(500, 200);

    //About
    createActions();
    createMenus();

}

void Window::browseDest()
{
    {
      QString filename = QFileDialog::getExistingDirectory(
                                            this,
                                            tr("Open Document"),
                                            QDir::currentPath(),
                                            QFileDialog::ShowDirsOnly );
      if( !filename.isNull() )
      {
          if (destinationComboBox->findText(filename) == -1)
          destinationComboBox->addItem(filename);
          destinationComboBox->setCurrentIndex(destinationComboBox->findText(filename));

      }
    }
}


QPushButton *Window::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

QComboBox *Window::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}

    //About
void Window::createActions()
{
    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}

void Window::createMenus()
{
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    }

void Window::about()
{
    QMessageBox::about(this, tr("About Transformer"),
                       tr("<h2>Axisante4 Transformer 2.0</h2>"
                          "<p>First version achieved in May, 2012"
                          "<p>The Transformer allows to convert Patient Files Data"
                          " from the recovery file Axisante.df* into "
                          "a XML file that can be integrated into FreeMedForms."
                          "This 2.0 version allows to create the xml file directly"
                          " from AXISANTE.DF*, without xml version needed before"
                          "<p>For more details, go to FreeMedForms Website."
                          "<p>Fredman, MD. May, 2012"));
}

void Window::printStartEnd()
{
    QFile file(destinationComboBox->currentText()+"\\FMF_FPatient.xml");

    //QFile file("C:\\Qt-Book\\Test5.xml");
    file.open(QIODevice::ReadWrite );
    XmlWriter xw( &file );
    xw.setAutoNewLine( true );
    xw.writeRaw("<?xml version=\"1.0\" encoding =\"UTF-8\"?>");
    xw.newLine();
    xw.writeRaw( "<!DOCTYPE FreeMedForms>" );
    xw.newLine();
    xw.writeOpenTag("DataIntegrator");

    //Patient Block :
    Patient patienttest;
    SaxHandler handler(&patienttest, &file);
    bool r = handler.readFile();

    xw.writeCloseTag("DataIntegrator");
    if (r){
        QMessageBox::information(this, tr("axisante4 Transformer"),
                                 tr("Conversion successful ! "),
                                 QMessageBox::Ok | QMessageBox::Default);}
        else {
            QMessageBox::information(this, tr("axisante4 Transformer"),
                                     tr("There was a problem during the conversion \n"
                                        "Check the result file and the Command window. "),
                                     QMessageBox::Ok | QMessageBox::Default);
        }

}
