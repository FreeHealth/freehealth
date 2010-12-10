-- Manage missing molecule_names
-- select distinct molecule_code from TMP_COMPOSITION where molecule_name is null;
-- --> 1674 7055 7239 79482 87268
-- select molecule_name from TMP_COMPOSITION where (molecule_code=7055 and molecule_name is not null);

CREATE TABLE IF NOT EXISTS `TMP_MOLS` (
CODE integer,
NAME varchar(200)
);

INSERT INTO TMP_MOLS (CODE, NAME) SELECT DISTINCT MOLECULE_CODE, MOLECULE_NAME FROM TMP_COMPOSITION WHERE MOLECULE_NAME IS NOT NULL;

update TMP_COMPOSITION set molecule_name =
(
  select name from TMP_MOLS
  where TMP_MOLS.code=TMP_COMPOSITION.molecule_code and TMP_MOLS.name is not null limit 1
)
where (TMP_COMPOSITION.molecule_name is null);

DROP TABLE `TMP_MOLS`;

-- Manage COMPOSITION table
INSERT INTO `COMPOSITION`
   (`UID`, `MOLECULE_FORM`, `MOLECULE_CODE`, `MOLECULE_NAME`, `DOSAGE`, `DOSAGE_REF`, `NATURE`, `LK_NATURE`)
SELECT
   `UID`, `MOLECULE_FORM`, `MOLECULE_CODE`, `MOLECULE_NAME`, `DOSAGE`, `DOSAGE_REF`, `NATURE`, `LK_NATURE`
FROM TMP_COMPOSITION
WHERE (TMP_COMPOSITION.molecule_name IS NOT NULL);

-- Drop uneeded tables;
DROP TABLE TMP_COMPOSITION;

-- Insert Databases informations;
INSERT INTO `INFORMATIONS` (
    `VERSION`,
    `NAME`,
    `IDENTIFIANT` ,
    `COMPAT_VERSION`,
    `PROVIDER`,
    `WEBLINK`,
    `COMPLEMENTARY_WEBSITE`,
    `AUTHOR`,
    `LICENSE`,
    `LICENSE_TERMS`,
    `DATE`,
    `DRUG_UID_NAME`,
    `PACK_MAIN_CODE_NAME`,
    `ATC`,
    `INTERACTIONS`,
    `AUTHOR_COMMENTS`,
    `LANGUAGE_COUNTRY`,
    `DRUGS_NAME_CONSTRUCTOR`
    ) VALUES (
    "0.5.2",
    "xx=French drugs database from the AFSSAPS.
    fr=Base de données thérapeutique française (AFSSAPS)",
    "FR_AFSSAPS",
    "0.5.0",
    "AFSSAPS : Agence Française de Sécurité Sanitaire des Produits de Santé",
    "http://afssaps-prd.afssaps.fr/php/ecodex/index.php",
    "http://www.freemedforms.org",
    "Eric Maeker, MD <eric.maeker@gmail.com>",
    "Public domain",
    "Public domain",
    CURRENT_DATE,
    "CIS",
    "CIP",
    1,
    1,
    "This database is freely accessible on the AFSSAPS Website. Interactions are calculted using drugs molecules, some errors can be found. Please report any problem to the author.",
    "fr_FR",
    "NAME"
    );
