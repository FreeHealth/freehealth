/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class MedicalUtils::EbmData
  \brief Provide access to Evidence Based Medicine data (publications).
*/

#include "ebmdata.h"

#include <QString>
#include <QDomDocument>
#include <QStringList>

#include <QDebug>

using namespace MedicalUtils;

namespace {

    const char * const  ROOT_TAG = "PubmedArticle";

    const char * const  MEDLINE_CITATION = "MedlineCitation";
    const char * const      PMID_TAG = "PMID";
    const char * const      DATECREATED_TAG = "DateCreated";
    const char * const      DATECOMPLETED_TAG = "DateCompleted";
    const char * const      DATEREVISED_TAG = "DateRevised";

    const char * const      ARTICLE_TAG = "Article";
    const char * const          JOURNAL_TAG = "Journal";
    const char * const              JOURNAL_ISSN_TAG = "ISSN";
    const char * const              JOURNALISSUE_TAG = "JournalIssue";
    const char * const                  VOLUME_TAG = "Volume";
    const char * const                  ISSUE_TAG = "Issue";
    const char * const                  PUBDATE_TAG = "PubDate";
    const char * const                      PUBDATE_YEAR_TAG = "Year";
    const char * const                      PUBDATE_MONTH_TAG = "Month";
    const char * const                  TITLE_TAG = "Title";
    const char * const                  ISO_ABBREV_TITLE_TAG = "ISOAbbreviation";

    const char * const          ARTICLETITLE_TAG = "ArticleTitle";
    const char * const          PAGIN_TAG = "Pagination";
    const char * const              MEDLINEPAGIN_TAG = "MedlinePgn";
    const char * const          ABSTRACT_TAG = "Abstract";
    const char * const              ABSTRACT_TEXT_TAG = "AbstractText";

    const char * const          AUTHORLIST_TAG = "AuthorList";
    const char * const              AUTHOR_TAG = "Author";
    const char * const              AUTHOR_VALID_ATTRIB = "ValidYN";
    const char * const                  AUTHORLASTNAME_TAG = "LastName";
    const char * const                  AUTHORFORENAME_TAG = "ForeName";
    const char * const                  AUTHORINITIALS_TAG = "Initials";

}


//    const char * const          PMID_TAG = "PMID";
//    const char * const          PMID_TAG = "PMID";

//    const char * const  SECOND_ROOT_TAG = "PubmedArticle";
//    const char * const  SECOND_ROOT_TAG = "PubmedArticle";


// PubMed XML Sample

//<?xml version='1.0' encoding='utf-8'?>
//<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>
//<pre>
//&lt;PubmedArticle>
//    &lt;MedlineCitation Owner="NLM" Status="MEDLINE">
//        &lt;PMID Version="1">11497241&lt;/PMID>
//        &lt;>
//            &lt;Year>2001&lt;/Year>
//            &lt;Month>08&lt;/Month>
//            &lt;Day>10&lt;/Day>
//        &lt;/DateCreated>
//        &lt;DateCompleted>
//            &lt;Year>2001&lt;/Year>
//            &lt;Month>09&lt;/Month>
//            &lt;Day>06&lt;/Day>
//        &lt;/DateCompleted>
//        &lt;DateRevised>
//            &lt;Year>2006&lt;/Year>
//            &lt;Month>11&lt;/Month>
//            &lt;Day>15&lt;/Day>
//        &lt;/DateRevised>
//        &lt;Article PubModel="Print">
//            &lt;Journal>
//                &lt;ISSN IssnType="Print">1420-682X&lt;/ISSN>
//                &lt;JournalIssue CitedMedium="Print">
//                    &lt;Volume>58&lt;/Volume>
//                    &lt;Issue>7&lt;/Issue>
//                    &lt;PubDate>
//                        &lt;Year>2001&lt;/Year>
//                        &lt;Month>Jun&lt;/Month>
//                    &lt;/PubDate>
//                &lt;/JournalIssue>
//                &lt;Title>Cellular and molecular life sciences : CMLS&lt;/Title>
//                &lt;ISOAbbreviation>Cell. Mol. Life Sci.&lt;/ISOAbbreviation>
//            &lt;/Journal>
//
//            &lt;ArticleTitle>From MDR to MXR: new understanding of multidrug resistance systems, their properties and clinical significance.&lt;/ArticleTitle>
//            &lt;Pagination>
//                &lt;MedlinePgn>931-59&lt;/MedlinePgn>
//            &lt;/Pagination>
//            &lt;Abstract>
//                &lt;AbstractText>The ATP binding cassette (ABC) superfamily of membrane transporters is one of the largest protein classes known, and counts numerous proteins involved in the trafficking of biological molecules across cell membranes. The first known human ABC transporter was P-glycoprotein (P-gp), which confers multidrug resistance (MDR) to anticancer drugs. In recent years, we have obtained an increased understanding of the mechanism of action of P-gp as its ATPase activity, substrate specificity and pharmacokinetic interactions have been investigated. This review focuses on the functional characterization of P-gp, as well as other ABC transporters involved in MDR: the family of multidrug-resistance-associated proteins (MRP1-7), and the recently discovered ABC half-transporter MXR (also known as BCRP, ABCP and ABCG2). We describe recent progress in the analysis of protein structure-function relationships, and consider the conceptual problem of defining and identifying substrates and inhibitors of MDR. An in-depth discussion follows of how coupling of nucleotide hydrolysis to substrate transport takes place, and we propose a scheme for the mechanism of P-gp function. Finally, the clinical correlations, both for reversal of MDR in cancer and for drug delivery, are discussed.&lt;/AbstractText>
//            &lt;/Abstract>

//            &lt;Affiliation>Department of Medical Physiology, The Panum Institute, University of Copenhagen, Denmark. t.litman@mfi.ku.dk&lt;/Affiliation>

//            &lt;AuthorList CompleteYN="Y">
//                &lt;Author ValidYN="Y">
//                    &lt;LastName>Litman&lt;/LastName>
//                    &lt;ForeName>T&lt;/ForeName>
//                    &lt;Initials>T&lt;/Initials>
//                &lt;/Author>
//                &lt;Author ValidYN="Y">
//                    &lt;LastName>Druley&lt;/LastName>
//                    &lt;ForeName>T E&lt;/ForeName>
//                    &lt;Initials>TE&lt;/Initials>
//                &lt;/Author>
//                &lt;Author ValidYN="Y">
//                    &lt;LastName>Stein&lt;/LastName>
//                    &lt;ForeName>W D&lt;/ForeName>
//                    &lt;Initials>WD&lt;/Initials>
//                &lt;/Author>
//                &lt;Author ValidYN="Y">
//                    &lt;LastName>Bates&lt;/LastName>
//                    &lt;ForeName>S E&lt;/ForeName>
//                    &lt;Initials>SE&lt;/Initials>
//                &lt;/Author>
//            &lt;/AuthorList>
//            &lt;Language>eng&lt;/Language>
//            &lt;PublicationTypeList>
//                &lt;PublicationType>Journal Article&lt;/PublicationType>
//                &lt;PublicationType>Research Support, Non-U.S. Gov't&lt;/PublicationType>
//                &lt;PublicationType>Review&lt;/PublicationType>
//            &lt;/PublicationTypeList>
//        &lt;/Article>
//        &lt;MedlineJournalInfo>
//            &lt;Country>Switzerland&lt;/Country>
//            &lt;MedlineTA>Cell Mol Life Sci&lt;/MedlineTA>
//            &lt;NlmUniqueID>9705402&lt;/NlmUniqueID>
//            &lt;ISSNLinking>1420-682X&lt;/ISSNLinking>
//        &lt;/MedlineJournalInfo>
//        &lt;ChemicalList>
//            &lt;Chemical>
//                &lt;RegistryNumber>0&lt;/RegistryNumber>
//                &lt;NameOfSubstance>ABCG2 protein, human&lt;/NameOfSubstance>
//            &lt;/Chemical>
//            &lt;Chemical>
//                &lt;RegistryNumber>0&lt;/RegistryNumber>
//                &lt;NameOfSubstance>ATP-Binding Cassette Transporters&lt;/NameOfSubstance>
//            &lt;/Chemical>
//            &lt;Chemical>
//                &lt;RegistryNumber>0&lt;/RegistryNumber>
//                &lt;NameOfSubstance>Antineoplastic Agents&lt;/NameOfSubstance>
//            &lt;/Chemical>
//            &lt;Chemical>
//                &lt;RegistryNumber>0&lt;/RegistryNumber>
//                &lt;NameOfSubstance>Neoplasm Proteins&lt;/NameOfSubstance>
//            &lt;/Chemical>
//            &lt;Chemical>
//                &lt;RegistryNumber>0&lt;/RegistryNumber>
//                &lt;NameOfSubstance>P-Glycoprotein&lt;/NameOfSubstance>
//            &lt;/Chemical>
//            &lt;Chemical>
//                &lt;RegistryNumber>65271-80-9&lt;/RegistryNumber>
//                &lt;NameOfSubstance>Mitoxantrone&lt;/NameOfSubstance>
//            &lt;/Chemical>
//        &lt;/ChemicalList>
//        &lt;CitationSubset>IM&lt;/CitationSubset>
//        &lt;MeshHeadingList>
//            &lt;MeshHeading>
//                &lt;DescriptorName MajorTopicYN="N">ATP-Binding Cassette Transporters&lt;/DescriptorName>
//                &lt;QualifierName MajorTopicYN="N">antagonists &amp;amp; inhibitors&lt;/QualifierName>
//                &lt;QualifierName MajorTopicYN="N">metabolism&lt;/QualifierName>
//                &lt;QualifierName MajorTopicYN="Y">physiology&lt;/QualifierName>
//            &lt;/MeshHeading>
//            &lt;MeshHeading>
//                &lt;DescriptorName MajorTopicYN="N">Animals&lt;/DescriptorName>
//            &lt;/MeshHeading>
//            &lt;MeshHeading>
//                &lt;DescriptorName MajorTopicYN="N">Antineoplastic Agents&lt;/DescriptorName>
//                &lt;QualifierName MajorTopicYN="N">pharmacology&lt;/QualifierName>
//            &lt;/MeshHeading>
//            &lt;MeshHeading>
//                &lt;DescriptorName MajorTopicYN="Y">Drug Resistance, Multiple&lt;/DescriptorName>
//            &lt;/MeshHeading>
//            &lt;MeshHeading>
//                &lt;DescriptorName MajorTopicYN="N">Forecasting&lt;/DescriptorName>
//            &lt;/MeshHeading>
//            &lt;MeshHeading>
//                &lt;DescriptorName MajorTopicYN="N">Humans&lt;/DescriptorName>
//            &lt;/MeshHeading>
//            &lt;MeshHeading>
//                &lt;DescriptorName MajorTopicYN="N">Mitoxantrone&lt;/DescriptorName>
//                &lt;QualifierName MajorTopicYN="N">pharmacology&lt;/QualifierName>
//            &lt;/MeshHeading>
//            &lt;MeshHeading>
//                &lt;DescriptorName MajorTopicYN="Y">Neoplasm Proteins&lt;/DescriptorName>
//            &lt;/MeshHeading>
//            &lt;MeshHeading>
//                &lt;DescriptorName MajorTopicYN="N">P-Glycoprotein&lt;/DescriptorName>
//                &lt;QualifierName MajorTopicYN="N">antagonists &amp;amp; inhibitors&lt;/QualifierName>
//                &lt;QualifierName MajorTopicYN="N">chemistry&lt;/QualifierName>
//                &lt;QualifierName MajorTopicYN="N">metabolism&lt;/QualifierName>
//                &lt;QualifierName MajorTopicYN="Y">physiology&lt;/QualifierName>
//            &lt;/MeshHeading>
//        &lt;/MeshHeadingList>
//        &lt;NumberOfReferences>235&lt;/NumberOfReferences>
//    &lt;/MedlineCitation>
//    &lt;PubmedData>
//        &lt;History>
//            &lt;PubMedPubDate PubStatus="pubmed">
//                &lt;Year>2001&lt;/Year>
//                &lt;Month>8&lt;/Month>
//                &lt;Day>11&lt;/Day>
//                &lt;Hour>10&lt;/Hour>
//                &lt;Minute>0&lt;/Minute>
//            &lt;/PubMedPubDate>
//            &lt;PubMedPubDate PubStatus="medline">
//                &lt;Year>2001&lt;/Year>
//                &lt;Month>9&lt;/Month>
//                &lt;Day>8&lt;/Day>
//                &lt;Hour>10&lt;/Hour>
//                &lt;Minute>1&lt;/Minute>
//            &lt;/PubMedPubDate>
//            &lt;PubMedPubDate PubStatus="entrez">
//                &lt;Year>2001&lt;/Year>
//                &lt;Month>8&lt;/Month>
//                &lt;Day>11&lt;/Day>
//                &lt;Hour>10&lt;/Hour>
//                &lt;Minute>0&lt;/Minute>
//            &lt;/PubMedPubDate>
//        &lt;/History>
//        &lt;PublicationStatus>ppublish&lt;/PublicationStatus>
//        &lt;ArticleIdList>
//            &lt;ArticleId IdType="pubmed">11497241&lt;/ArticleId>
//        &lt;/ArticleIdList>
//    &lt;/PubmedData>
//&lt;/PubmedArticle>

//</pre>

EbmData::EbmData()
{
}

EbmData::~EbmData()
{
}

bool EbmData::setPubMedXml(const QString &xml)
{
    m_PubMedXml=xml;
    m_Doc.clear();
    QString error;
    int line, col;
    if (!m_Doc.setContent(xml, &error, &line, &col)) {
        qWarning() << "ERROR" << error << line << col;
        return false;
    }
    //        ARTICLE_TAG = "Article";
    //            JOURNAL_TAG = "Journal";
    //                JOURNAL_ISSN_TAG = "ISSN";
    //                JOURNALISSUE_TAG = "JournalIssue";
    //                    VOLUME_TAG = "Volume";
    //                    ISSUE_TAG = "Issue";
    //                    PUBDATE_TAG = "PubDate";
    //                        PUBDATE_YEAR_TAG = "Year";
    //                        PUBDATE_MONTH_TAG = "Month";
    //                    TITLE_TAG = "Title";
    //                    ISO_ABBREV_TITLE_TAG = "ISOAbbreviation";
    //                    ISSUE_TAG = "Issue";

    //            ARTICLETITLE_TAG = "ArticleTitle";
    //            PAGIN_TAG = "Pagination";
    //                MEDLINEPAGIN_TAG = "MedlinePgn";
    //            ABSTRACT_TAG = "Abstract";
    //                ABSTRACT_TEXT_TAG = "AbstractText";

    //            AUTHORLIST_TAG = "AuthorList";
    //                AUTHOR_TAG = "Author";
    //                AUTHOR_VALID_ATTRIB = "ValidYN";
    //                    AUTHORLASTNAME_TAG = "LastName";
    //                    AUTHORFORENAME_TAG = "ForeName";
    //                    AUTHORINITIALS_TAG = "Initials";

    // extract complete reference
    QDomElement art = m_Doc.firstChildElement(::ROOT_TAG);
    art = art.firstChildElement(::MEDLINE_CITATION);
    art = art.firstChildElement(::ARTICLE_TAG);
    // go journal
    QDomElement w = art.firstChildElement(::JOURNAL_TAG);
    QString journalFull = w.firstChildElement(::TITLE_TAG).text();
    QString journalIso = w.firstChildElement(::ISO_ABBREV_TITLE_TAG).text();
    // go journalIssue
    w = w.firstChildElement(::JOURNALISSUE_TAG);
    QString vol = w.firstChildElement(::VOLUME_TAG).text();
    QString issue = w.firstChildElement(::ISSUE_TAG).text();
    // go pubDate
    w = w.firstChildElement(::PUBDATE_TAG);
    QString year = w.firstChildElement(::PUBDATE_YEAR_TAG).text();
    QString month = w.firstChildElement(::PUBDATE_MONTH_TAG).text();
    // go Article/Pagination
    w = art.firstChildElement(::PAGIN_TAG);
    QString pagin = w.firstChildElement(::MEDLINEPAGIN_TAG ).text();
    // Journal abbrev. Year month;Vol(Issue):pages.
    m_ShortRef = QString("%1 %2 %3;%4(%5):%6")
            .arg(journalFull)
            .arg(year)
            .arg(month)
            .arg(vol)
            .arg(issue)
            .arg(pagin);

    // Full reference
    // Authors. Journal full title. Year month;Vol(Issue):pages.
    QDomElement authors = art.firstChildElement(::AUTHORLIST_TAG);
    //            AUTHORLIST_TAG = "AuthorList";
    //                AUTHOR_TAG = "Author";
    //                AUTHOR_VALID_ATTRIB = "ValidYN";
    //                    AUTHORLASTNAME_TAG = "LastName";
    //                    AUTHORFORENAME_TAG = "ForeName";
    //                    AUTHORINITIALS_TAG = "Initials";
    w = authors.firstChildElement(::AUTHOR_TAG); // if ValidYN == Y go on
    QStringList authorsList;
    while (!w.isNull()) {
        if (w.attribute(::AUTHOR_VALID_ATTRIB)=="Y") {
            QString name = w.firstChildElement(::AUTHORLASTNAME_TAG).text();
            QString foreName = w.firstChildElement(::AUTHORFORENAME_TAG).text();
//            QString initials = w.firstChildElement(::AUTHORINITIALS_TAG).text();
            authorsList.append(name + " " + foreName);
        }
        w = w.nextSiblingElement(::AUTHOR_TAG);
    }
    m_Authors = authorsList.join(". ");

    m_Title = art.firstChildElement(::ARTICLETITLE_TAG).text();

    m_Ref = QString("%1. %2\n   %3 %4 %5;%6(%7):%8")
            .arg(authorsList.join("; "))
            .arg(m_Title)
            .arg(journalIso)
            .arg(year)
            .arg(month)
            .arg(vol)
            .arg(issue)
            .arg(pagin);

    // extract abstract
    m_Abstract = art.firstChildElement(::ABSTRACT_TAG).firstChildElement(::ABSTRACT_TEXT_TAG).text();

    return true;
}

QString EbmData::data(const int reference) const
{
    if (reference >= ReferencesCount)
        return QString();
    switch (reference) {
    case Link: return m_Link;
    case PMID: return QString();
    case ShortReferences:
    {
        return m_ShortRef;
    }
    case AbstractPlainText:
    {
        if (!m_Abstract.isEmpty()) {
            return m_Abstract;
        }
        QDomElement art = m_Doc.firstChildElement(::ROOT_TAG);
        art = art.firstChildElement(::MEDLINE_CITATION);
        art = art.firstChildElement(::ARTICLE_TAG);
        art = art.firstChildElement(::ABSTRACT_TAG);
        return art.firstChildElement(ABSTRACT_TEXT_TAG).text();
    }
    case CompleteReferences:
    {
        return m_Ref;
    }
    case Title: return m_Title;
    case Authors: return m_Authors;
    }
    return QString();
}

QDebug operator<<(QDebug dbg, const MedicalUtils::EbmData &c)
{
    dbg.nospace() << QString("EbmData(%1; Ref: %2chars; Abstract: %3chars; Xml: %4chars\n")
                     .arg(c.data(MedicalUtils::EbmData::ShortReferences)).arg(c.references().size()).arg(c.abstract().size()).arg(c.xmlEncoded().size())
                  << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const MedicalUtils::EbmData *c)
{
    if (!c) {
        dbg.nospace() << "EbmData(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}
