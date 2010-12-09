-- remove Veterinary drugs
-- do the following for each of tables
-- form
-- route
-- ingred
-- ther
-- package

DELETE FROM form
WHERE
  DRUG_CODE
IN
(SELECT
  A1.DRUG_CODE
FROM
  status A1, drug A2
WHERE
  A1.DRUG_CODE = A2.DRUG_CODE AND
A2.Class = "Veterinary")
;

DELETE FROM route
WHERE
  DRUG_CODE
IN
(SELECT
  A1.DRUG_CODE
FROM
  route A1, drug A2
WHERE
  A1.DRUG_CODE = A2.DRUG_CODE AND
A2.Class = "Veterinary")
;

DELETE FROM ingred
WHERE
  DRUG_CODE
IN
(SELECT
  A1.DRUG_CODE
FROM
  ingred A1, drug A2
WHERE
  A1.DRUG_CODE = A2.DRUG_CODE AND
A2.Class = "Veterinary")
;

DELETE FROM ther
WHERE
  DRUG_CODE
IN
(SELECT
  A1.DRUG_CODE
FROM
  ther A1, drug A2
WHERE
  A1.DRUG_CODE = A2.DRUG_CODE AND
A2.Class = "Veterinary")
;

DELETE FROM package
WHERE
  DRUG_CODE
IN
(SELECT
  A1.DRUG_CODE
FROM
  package A1, drug A2
WHERE
  A1.DRUG_CODE = A2.DRUG_CODE AND
A2.Class = "Veterinary")
;

DELETE FROM drug
WHERE Class = "Veterinary"
;



-- add and populate column DIN to serve as an alternate key:

ALTER TABLE "form" ADD "DIN" VARCHAR2(8);
ALTER TABLE "route" ADD "DIN" VARCHAR2(8);
ALTER TABLE "ingred" ADD "DIN" VARCHAR2(8);
ALTER TABLE "ther" ADD "DIN" VARCHAR2(8);
ALTER TABLE "package" ADD "DIN" VARCHAR2(8);

UPDATE form
SET DIN =
(SELECT DIN
FROM drug A1
WHERE A1.DRUG_CODE=form.DRUG_CODE
);

UPDATE route
SET DIN =
(SELECT DIN
FROM drug A1
WHERE A1.DRUG_CODE=route.DRUG_CODE
);

UPDATE ingred
SET DIN =
(SELECT DIN
FROM drug A1
WHERE A1.DRUG_CODE=ingred.DRUG_CODE
);

UPDATE ther
SET DIN =
(SELECT DIN
FROM drug A1
WHERE A1.DRUG_CODE=ther.DRUG_CODE
);

UPDATE package
SET DIN =
(SELECT DIN
FROM drug A1
WHERE A1.DRUG_CODE=package.DRUG_CODE
);




-- Feed FreeDiams table DRUGS
-- these are Branded products
-- the drugs must be distinct on {drug or combination} plus strength)
-- note FD's DRUGS table needs its records pre-ordered ascending on column NAME

INSERT INTO DRUGS ("UID", "NAME")
SELECT DISTINCT
   A1.DIN,
   A1.BRAND_NAME
FROM drug A1
ORDER BY A1.BRAND_NAME;


-- account for products that package >1 form and/or can be used by multiple routes
UPDATE DRUGS
SET FORM=
(SELECT group_concat(PHARMACEUTICAL_FORM, ", ")
FROM form A1
WHERE A1.DIN=DRUGS.UID
GROUP BY DIN
LIMIT 10);

UPDATE DRUGS
SET ROUTE=
(SELECT group_concat(ROUTE_OF_ADMINISTRATION, ", ")
FROM route A1
WHERE A1.DIN=DRUGS.UID
GROUP BY DIN
LIMIT 10);

-- ensure that any Canadian drug NULL strengths are backfilled from related ingredients

UPDATE DRUGS
SET GLOBAL_STRENGTH=
(SELECT group_concat(STRENGTH || STRENGTH_UNIT, ";")
FROM ingred A1
WHERE A1.DIN=DRUGS.UID
GROUP BY DIN
LIMIT 10);

-- set the ATC
-- note Canada's "ther" table contains nondistinct ATCs (to support multiple AHFS values)

UPDATE DRUGS
SET ATC=
(SELECT DISTINCT TC_ATC_NUMBER
FROM ther A1
WHERE A1.DIN=DRUGS.UID
);



-- feed table COMPOSITION (molecular ingredients)
-- noticing some have DOSAGE_VALUE and others do not
-- also omitting in this step "MOLECULE_FORM"

INSERT INTO "COMPOSITION"
   ("UID", "MOLECULE_CODE", "MOLECULE_NAME", "DOSAGE", "DOSAGE_REF", "NATURE", "LK_NATURE")
SELECT DISTINCT
   DRUGS.UID,
   A1.ACTIVE_INGREDIENT_CODE,
   A1.INGREDIENT,
   A1.STRENGTH || A1.STRENGTH_UNIT || "/" || A1.DOSAGE_VALUE || A1.DOSAGE_UNIT,
   "",
   "SA",
   1
FROM DRUGS, ingred A1
WHERE
   (DRUGS.UID = A1.DIN) AND
   (A1.DOSAGE_VALUE != "")
   ;


INSERT INTO COMPOSITION
   ("UID", "MOLECULE_CODE", "MOLECULE_NAME", "DOSAGE", "DOSAGE_REF", "NATURE", "LK_NATURE")
SELECT DISTINCT
   DRUGS.UID,
   A1.ACTIVE_INGREDIENT_CODE,
   A1.INGREDIENT,
   A1.STRENGTH || A1.STRENGTH_UNIT,
   "",
   "SA",
   1
FROM DRUGS, ingred A1
WHERE
   (DRUGS.UID = A1.DIN) AND
   (A1.DOSAGE_VALUE = "");
   


INSERT INTO LK_MOL_ATC (
    MOLECULE_CODE,
    ATC
    )
SELECT DISTINCT
    A1.MOLECULE_CODE,
    A2.ATC
FROM COMPOSITION A1, DRUGS A2, drug A3
WHERE A1.UID = A2.UID AND
    (A1.UID = A3.DIN AND A3.NUMBER_OF_AIS = 1 AND
    LENGTH(A2.ATC)=7);
