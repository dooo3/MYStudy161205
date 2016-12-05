SELECT *
FROM sales
WHERE stor_id = '7067'

SELECT *
INTO #sales
FROM sales
WHERE stor_id = '7067'

SELECT * FROM #sales

CREATE TABLE PKExam
( 
	id INT
		CONSTRAINT PK_PKExam_id PRIMARY KEY
)

INSERT PKExam VALUES(1)
INSERT PKExam VALUES(null)

UPDATE	PKExam
SET id=11
WHERE id=1

SELECT * FROM PKExam

DROP TABLE PKExam


DBCC DROPCLEANBUFFERS

SET STATISTICS IO ON
SET STATISTICS TIME ON

SET STATISTICS IO OFF
SET STATISTICS TIME OFF

SELECT	title_id --AS '책번호'
,		qty --AS '수량'
,		CASE
			WHEN qty>= 50 THEN 'A'
			WHEN qty>= 30 THEN 'B'
			ELSE 'C'
		END --AS '등급'
		
FROM pubs..sales



BEGIN TRAN
	UPDATE pubs..titles SET price = 100 WHERE title_id = 'BU1032'
		BEGIN TRAN tx1
			UPDATE pubs..titles SET price = 300 WHERE title_id = 'BU1032'
		COMMIT TRAN tx1
	SELECT price FROM pubs..titles WHERE title_id =  'BU1032'
ROLLBACK TRAN
SELECT price FROM pubs..titles WHERE title_id =  'BU1032'


BEGIN TRAN
	UPDATE pubs..titles SET price = 100 WHERE title_id = 'BU1032'
		SAVE TRAN savePoint 
			SELECT price FROM pubs..titles WHERE title_id =  'BU1032'
			UPDATE pubs..titles SET price = 300 WHERE title_id = 'BU1032'
			SELECT price FROM pubs..titles WHERE title_id =  'BU1032'
		ROLLBACK TRAN savePoint
	SELECT price FROM pubs..titles WHERE title_id =  'BU1032'
	SELECT @@TRANCOUNT
COMMIT TRAN
SELECT price FROM pubs..titles WHERE title_id =  'BU1032'


BEGIN TRAN
	UPDATE pubs..titles SET price = 100 WHERE title_id = 'BU1032'
		BEGIN TRAN tx1
			UPDATE pubs..titles SET price = 300 WHERE title_id = 'BU1032'
			SELECT price FROM pubs..titles WHERE title_id =  'BU1032'
		ROLLBACK TRAN tx1
	SELECT price FROM pubs..titles WHERE title_id =  'BU1032'
	SELECT @@TRANCOUNT
COMMIT TRAN
SELECT price FROM pubs..titles WHERE title_id =  'BU1032'



UPDATE pubs..titles SET price = 19.99 WHERE title_id = 'BU1032'
	
DBCC USEROPTIONS

sp_lock


SELECT COUNT(*) FROM sales
GO

BEGIN TRAN
	DELETE sales
	SELECT COUNT(*) FROM sales
ROLLBACK TRAN
SELECT COUNT(*) FROM sales
GO

SELECT @@trancount
SELECT @@spid
EXEC sp_lock

SELECT db_name(1),db_name(5)

USE master
SELECT object_name(85575343)


BEGIN TRAN
	SELECT @@trancount
	UPDATE titles SET price = 0 WHERE title_id = 'BU1032'
	EXEC sp_lock @@spid
ROLLBACK TRAN


DROP TABLE
--2016-10-262016-10-262016-10-262016-10-262016-10-262016-10-262016-10-262016-10-262016-10-262016-10-26
SELECT @nTmpCnt = count(0) FROM tb_CI where szCI = @mCI
	if @nTmpCnt > 0
	begin
	end
	
SELECT @nCI = nCI FROM tb_ci WHERE szCi = @szci
IF @@ROWCOUNT > 0
BEGIN
	
END