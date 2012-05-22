

#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <qmap.h>
#include <qstring.h>
#include <qtextstream.h>

class AttrMap : public QMap<QString, QString>
{
public:
    AttrMap() { }
    AttrMap( const QString& name, const QString& value ) {
    insert( name, value );
    }
};

class XmlWriter
{
public:
    XmlWriter( QIODevice *device, QTextCodec *codec = 0 );
    ~XmlWriter();

    void writeRaw( const QString& xml );
    void writeString( const QString& string );
    void writeOpenTag( const QString &name, const AttrMap &attrs = AttrMap() );
    void writeCloseTag( const QString& name );
    void writeAtomTag( const QString& name, const AttrMap& attrs = AttrMap() );
    void writeTaggedString( const QString& name, const QString& string,
                const AttrMap& attrs = AttrMap() );
    void newLine();
    void setIndentSize( int size ) { indentSize = size; }
    void setAutoNewLine( bool on ) { autoNewLine = on; }

private:
    QString protect( const QString& string );
    QString opening( const QString& tag, const AttrMap& attrs = AttrMap() );
    void writePendingIndent();

    QTextStream out;
    QString indentStr;
    int indentSize;
    bool autoNewLine;
    bool atBeginningOfLine;
};

#endif

