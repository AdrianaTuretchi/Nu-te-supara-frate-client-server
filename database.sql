BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "PLAYER" (
	"ID"	INTEGER,
	"NAME"	TEXT NOT NULL UNIQUE,
	"SCORE"	INT DEFAULT 0,
	"WINS"	INT DEFAULT 0,
	"LOSES"	INT DEFAULT 0,
	"SECONDS"	INT DEFAULT 0,
	"THIRDS"	INT DEFAULT 0,
	PRIMARY KEY("ID" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "GAMES" (
	"PLAYER_COUNT"	INT NOT NULL,
	"ID_1"	INT,
	"ID_2"	INT,
	"ID_3"	INT,
	"ID_4"	INT,
	"P1_1"	INT,
	"P1_2"	INT,
	"P1_3"	INT,
	"P1_4"	INT,
	"P2_1"	INT,
	"P2_2"	INT,
	"P2_3"	INT,
	"P2_4"	INT,
	"P3_1"	INT,
	"P3_2"	INT,
	"P3_3"	INT,
	"P3_4"	INT,
	"P4_1"	INT,
	"P4_2"	INT,
	"P4_3"	INT,
	"P4_4"	INT,
	"STATE"	text NOT NULL,
	FOREIGN KEY("ID_3") REFERENCES "PLAYER"("ID"),
	FOREIGN KEY("ID_4") REFERENCES "PLAYER"("ID"),
	FOREIGN KEY("ID_2") REFERENCES "PLAYER"("ID"),
	PRIMARY KEY("ID_1","ID_2","ID_3","ID_4"),
	FOREIGN KEY("ID_1") REFERENCES "PLAYER"("ID")
);
INSERT INTO "PLAYER" VALUES (1,'John',0,0,0,0,0);
INSERT INTO "PLAYER" VALUES (2,'Bob',0,0,0,0,0);
INSERT INTO "PLAYER" VALUES (3,'Alice',0,0,0,0,0);
INSERT INTO "PLAYER" VALUES (4,'Ana',0,0,0,0,0);
INSERT INTO "PLAYER" VALUES (5,'Ivy',0,0,0,0,0);
INSERT INTO "PLAYER" VALUES (6,'Relu',0,0,0,0,0);
INSERT INTO "PLAYER" VALUES (7,'Carlas',0,0,0,0,0);
COMMIT;
