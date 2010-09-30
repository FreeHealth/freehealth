-- Creating temporary tables;
CREATE TABLE IF NOT EXISTS `products` (
    `APPNO`           VARCHAR(6) NOT NULL ,
    `PRODUCTNO`       VARCHAR(3) NOT NULL,
    `FORM`            VARCHAR(255),
    `DOSAGE`          VARCHAR(240),
    `STATUS`          INT(1),
    `TECODE`          VARCHAR(100),
    `REFERENCEDRUG`   INT(1),
    `DRUGNAME`        VARCHAR(125) NULL,
    `INGREDIEND`      VARCHAR(255) NULL
);


