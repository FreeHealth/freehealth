#ifndef GIRSCORE_H
#define GIRSCORE_H

#include <QString>

namespace MedicalUtils {
namespace AGGIR {
class GirScorePrivate;

class GirScore
{
public:

    GirScore();
    ~GirScore();

    enum chaineGIR {
        GIR_coherence = 0,
        GIR_orientation,
        GIR_toilette,
        GIR_habillage,
        GIR_alimentation,
        GIR_elimination,
        GIR_transferts,
        GIR_interieur
    };

    bool isNull() const;
    bool isValid() const;

    // Variables
    void setValues(const QString &coherence,
                   const QString &orientation,
                   const QString &toilette,
                   const QString &habillage,
                   const QString &alimentation,
                   const QString &elimination,
                   const QString &transferts,
                   const QString &interieur);

    void setCoherence(const QString &val);
    void setOrientation(const QString &val);
    void setToilette(const QString &val);
    void setHabillage(const QString &val);
    void setAlimentation(const QString &val);
    void setElimination(const QString &val);
    void setTransferts(const QString &val);
    void setInterieur(const QString &val);

    // Descriptives
    void setExterieur(const QString &val);
    void setCommunication(const QString &val);

    int resultingGir() const;
    QString explanations(int girScore) const;

//protected:
    QString getGirString() const;

private:
    GirScorePrivate *d;
};

} // End AGGIR
} // End MedicalUtils

#endif  // End GIRSCORE_H

