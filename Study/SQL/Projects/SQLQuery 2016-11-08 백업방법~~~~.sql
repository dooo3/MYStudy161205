--파일로 백업
SET LANGUAGE '한국어'
BACKUP DATABASE test_gold_tempdb 
TO DISK = 'C:\BAK_TEST.bak' 
WITH INIT
--로그백업
BACKUP LOG test_gold_tempdb TO DISK = 'C:\Log' 
 


UPDATE a SET szName = '첫번째백업' WHERE nId = 1


UPDATE a SET szName = '데이터파괴' WHERE nId = 2

UPDATE a SET szName = '세번째버전' WHERE nId = 3


UPDATE a SET szName = '실수'

SELECT TOP 3* FROM a 

SELECT GETDATE()

2016-11-08 17:04:54.280
문제발생시각

--즉시 로그를 백업
BACKUP LOG test_gold_tempdb TO DISK = 'c:\LogNoTrunc_ver2' WITH NO_TRUNCATE, INIT
--복구과정--

--풀백업본을 리스토어
RESTORE DATABASE test_gold_tempdb FROM DISK = 'C:\BAK_TEST.bak'
WITH NORECOVERY

--첫번째 로그 백업 복구 noRecovery로! stopat으로 진짜 복원할것이 남아있기 때문에
RESTORE LOG test_gold_tempdb FROM DISK = 'C:\Log' 
WITH FILE = 1 ,NORECOVERY 


RESTORE LOG test_gold_tempdb FROM DISK = 'C:\Log' 
WITH RECOVERY 


RESTORE LOG test_gold_tempdb FROM DISK = 'C:\LogNoTrunc' 
WITH NORECOVERY
 
RESTORE LOG test_gold_tempdb FROM DISK = 'C:\LogNoTrunc_ver2' 
WITH FILE = 1 ,NORECOVERY 

RESTORE LOG test_gold_tempdb FROM DISK = 'C:\LogNoTrunc_ver2' 
WITH FILE = 2 ,RECOVERY 




RESTORE LOG test_gold_tempdb FROM DISK = 'C:\Log_ver_3' 
WITH NORECOVERY 


--중요한 stopat으로 복구 
--발생 시간 빼기 -8초 복구
RESTORE LOG test_gold_tempdb FROM DISK = 'c:\LogNoTrunc_ver2'
WITH STOPAT = '2016-11-08 17:04: .280' , RECOVERY



--복원 순서
/*
	1.master로 바꿈
	
	2.조회 가능 확인(안되면 시작)
	
	3-1 데이터베이스 복구 
	3-2 현재풀백업본을복구하는것이다
	3-3 이후LOG를복구할게더있으므로옵션으로
	3-4 WITH NORECOVERY를 붙인다
*/
USE master
--USE test_gold_tempdb

SELECT TOP 1 szName FROM a

UPDATE a SET szName = '데이터파괴' WHERE nId = 1

restore database test_gold_tempdb FROM DISK = 'C:\test_gold_tempdb_BAK_TEST.bak'
WITH NORECOVERY

RESTORE LOG test_gold_tempdb FROM DISK = 'C:\gold_Log' 
WITH RECOVERY 

EXEC sp_helpdb test_gold_tempdb
 
EXEC sp_who test_gold_tempdb