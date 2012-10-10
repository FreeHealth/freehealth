-- $Source: localize-bc_CA.sql
-- $Revision: 1 $
-- Authors: Jim Busser, FreeDiams and GNUmed projects

.mode csv
.separator "|"

DROP TABLE IF EXISTS "pnp";
DROP TABLE IF EXISTS "drugmast";


CREATE TABLE IF NOT EXISTS "pnp" (
    "CDIC" VARCHAR2(8) NOT NULL ,
    "LCA" VARCHAR2(1),
    "TRIAL" VARCHAR2(1),
    "MAXPRIC" FLOAT(9),
    "GENPRIC" FLOAT(9),
    "DAYPRIC" FLOAT(9)
);


CREATE TABLE IF NOT EXISTS "drugmast" (
    "ALT" VARCHAR2(1) NOT NULL ,
    "CDIC" VARCHAR2(8),
    "BEN" VARCHAR2(16)
);

.import pnp.csv pnp
.import drugmast.csv drugmast

ALTER TABLE "pnp" ADD "COVERAGE" VARCHAR2(40);

UPDATE pnp
SET COVERAGE = 
	"Max " || CAST(MAXPRIC AS TEXT) || " vs. generic " || CAST(GENPRIC AS TEXT) || "; Day max " || CAST(DAYPRIC AS TEXT)
WHERE COVERAGE IS NULL AND GENPRIC > 0 AND DAYPRIC > 0
;

UPDATE pnp
SET COVERAGE = 
	"Max " || CAST(MAXPRIC AS TEXT) || " vs. generic " || CAST(GENPRIC AS TEXT)
WHERE COVERAGE IS NULL AND GENPRIC > 0 AND DAYPRIC = 0
;

UPDATE pnp
SET COVERAGE = 
	"Max " || CAST(MAXPRIC AS TEXT) || "; Day max " || CAST(DAYPRIC AS TEXT)
WHERE COVERAGE IS NULL AND GENPRIC = 0 AND DAYPRIC > 0
;

UPDATE pnp
SET COVERAGE = 
	"Max " || CAST(MAXPRIC AS TEXT)
WHERE COVERAGE IS NULL AND GENPRIC = 0 AND DAYPRIC = 0
;

UPDATE pnp
SET COVERAGE = 
  COVERAGE || " (Plans: "
  || TRIM(
  (SELECT DISTINCT BEN
  FROM drugmast A1
  WHERE A1.CDIC=pnp.CDIC)
  )
  || ")"
;

UPDATE pnp
SET COVERAGE = 
  COVERAGE || " Trial"
WHERE TRIAL LIKE "Y"
;

-- **************************************************
-- Localize DRUGS.AUTHORIZATION
-- **************************************************
-- Assumes schema has been updated so that DRUGS.AUTHORIZATION has length ~ 75 not 1
UPDATE DRUGS
SET AUTHORIZATION =
(SELECT DISTINCT COVERAGE FROM pnp WHERE pnp.CDIC = DRUGS.UID)
;

-- **************************************************
-- Localize INFORMATIONS
-- **************************************************

-- UPDATE INFORMATIONS
-- SET IDENTIFIANT = "CA_HCDPD@BC"
-- ;

UPDATE INFORMATIONS
SET NAME = "en=Canadian Drug Product Database / British Columbia
fr=Base de données thérapeutique Canadienne / British Columbia"
;

-- **************************************************
-- Localize "Help and Infos" button content to BC.CA
-- **************************************************

DELETE FROM `SEARCH_ENGINES`;
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "BC PharmaCare benefits search", "http://www.health.gov.bc.ca/pharmacare/benefitslookup/faces/Search.jsp");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "BC PharmaCare program plans listings", "http://www.health.gov.bc.ca/pharmacare/plans/index.html#");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "BC PharmaCare program policies", "http://www.health.gov.bc.ca/pharmacare/policy.html");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Health Canada: Adverse drug reaction – guidance", "http://www.hc-sc.gc.ca/dhp-mps/prodnatur/legislation/docs/arr-dri_hcpc-fscc-eng.php#2");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Health Canada: Adverse drug reaction – report form", "https://webprod4.hc-sc.gc.ca/medeffect-medeffet/index-eng.jsp");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Health Canada: Advisories, warnings and recalls", "http://www.hc-sc.gc.ca/dhp-mps/medeff/advisories-avis/index-eng.php");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Health Canada: Drug information (search DIN)", "http://webprod.hc-sc.gc.ca/dpd-bdpp/index-eng.jsp");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Health Canada: Special Access program", "http://www.hc-sc.gc.ca/dhp-mps/acces/drugs-drogues/index-eng.php");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Meta coverage lookup (drugcoverage.ca search)", "http://www.drugcoverage.ca/");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Pill identifier (NLM Pillbox beta)", "http://pillbox.nlm.nih.gov/bin-release/PillBox.php");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "Renal dosing at U Heidelberg", "http://www.dosing.de/Niere/nierelst.htm");
INSERT INTO `SEARCH_ENGINES` VALUES (NULL, "WHO classification (auto-query ATC)", "http://www.whocc.no/atc_ddd_index/?&code=[[ONE_ATC_CODE]]&showdescription=yes");

-- clean up

-- DROP TABLE pnp;
-- DROP TABLE drugmast;
VACUUM;


-- ALTER TABLE "pnp" ADD "BENEFIT_PLANS" VARCHAR2(16);
-- 
-- UPDATE pnp
-- SET BENEFIT_PLANS =
-- (SELECT DISTINCT BEN
-- FROM drugmast A1
-- WHERE A1.CDIC=pnp.CDIC
-- );

