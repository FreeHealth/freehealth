BEGIN TRANSACTION;

INSERT INTO bank_details VALUES(1,'AccountancyDefaultUser','BD LABEL1','BD OWNER1','BD ADRESS1','BDACCNUM1','BDIBAN1',1111.0,'2010/04/01','BD COMMENT1',0);
INSERT INTO bank_details VALUES(2,'AccountancyDefaultUser','BD LABEL2','BD OWNER2','BD ADRESS2','BDACCNUM2','BDIBAN2',2222.0,'2010/04/02','BD COMMENT2',1);

INSERT INTO medical_procedure VALUES(1,'AccountancyDefaultUser1','AccountancyDefaultUser','MP1','ABSTRACT MP 1','TYPE MP1',1.1,0.0,'2010/04/01');
INSERT INTO medical_procedure VALUES(2,'AccountancyDefaultUser2','AccountancyDefaultUser','MP2','ABSTRACT MP2','TYPE MP2',2.2,1.0,'2010/04/02');

COMMIT;
