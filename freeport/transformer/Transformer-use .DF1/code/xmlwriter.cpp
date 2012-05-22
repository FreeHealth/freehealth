#include <qtextcodec.h>
#include "xmlwriter.h"

XmlWriter::XmlWriter( QIODevice *device, QTextCodec *codec )
    : indentSize( 4 ), autoNewLine( false ), atBeginningOfLine( true )
{
    out.setDevice( device );
    if ( codec == 0 ) {
    out.setCodec(QTextCodec::codecForName("UTF-8"));
    } else {
    out.setCodec( codec );
    out << "<?xml version=\"1.0\" encoding=\""
        << protect( codec->name() ) << "\"?>\n";
    }
}

XmlWriter::~XmlWriter()
{
    if ( autoNewLine && !atBeginningOfLine )
    out << endl;
}

QString XmlWriter::protect( const QString& string )
{
    QString s = string;
    s.replace( "&", "&amp;" );
    s.replace( ">", "&gt;" );
    s.replace( "<", "&lt;" );
    s.replace( "\"", "&quot;" );
    s.replace( "\'", "&apos;" );
    return s;
}

QString XmlWriter::opening( const QString& tag, const AttrMap& attrs )
{
    QString s = "<" + tag;
    AttrMap::ConstIterator a = attrs.begin();
    while ( a != attrs.end() ) {
    s += " " + a.key() + "=\"" + protect( *a ) + "\"";
    ++a;
    }
    s += ">";
    return s;
}

void XmlWriter::writePendingIndent()
{
    if ( atBeginningOfLine ) {
    out << indentStr;
    atBeginningOfLine = false;
    }
}

void XmlWriter::newLine()
{
    out << endl;
    atBeginningOfLine = true;
}

void XmlWriter::writeRaw( const QString& xml )
{
    out << xml;
    atBeginningOfLine = false;
}

void XmlWriter::writeString( const QString &string )
{
    out << protect( string );
    atBeginningOfLine = false;
}

void XmlWriter::writeOpenTag(  const QString &name,  const AttrMap &attrs )
{
    writePendingIndent();
    out << opening( name, attrs );
    indentStr += QString().fill( ' ', indentSize );
    if ( autoNewLine )
    newLine();
}

void XmlWriter::writeCloseTag( const QString& name )
{
    indentStr = indentStr.mid( indentSize );
    writePendingIndent();
    out << opening( "/" + name );
    if ( autoNewLine )
    newLine();
}

void XmlWriter::writeAtomTag( const QString& name, const AttrMap& attrs )
{
    writePendingIndent();
    QString atom = opening( name, attrs );
    atom.insert( atom.length() - 1, "/" );
    out << atom;
    if ( autoNewLine )
    newLine();
}

void XmlWriter::writeTaggedString( const QString& name, const QString& string,
                   const AttrMap& attrs )
{
    writePendingIndent();
    out << opening( name, attrs );
    writeString( string );
    out << opening( "/" + name );
    if ( autoNewLine )
    newLine();
}

