
SELECT top 300000* into tb_test_user
FROM [mobile_test].[dbo].[3YEAR]

SELECT * into tb_test_Realuser
FROM [mobile_test].[dbo].[3YEAR]


--DROP TABLE tb_test_user

SELECT * FROM tb_test_user

DECLARE @i INT SET @i = 0


SELECT * FROM tb_test_user 
ORDER BY NUM_NID ASC


SELECT * FROM tb_test_user 
ORDER BY VC_UID ASC 

CREATE INDEX tb_noncl_Idx
ON test_gold_tempdb.dbo.tb_test_user(VC_UID)

CREATE UNIQUE CLUSTERED INDEX tb_cluster_Idx
ON test_gold_tempdb.dbo.tb_test_user(NUM_NID)		

DROP INDEX tb_test_user.tb_cluster_Idx

CREATE INDEX Idx
ON test_gold_tempdb.dbo.tb_test_Realuser(NUM_NID,VC_UID)

CREATE INDEX tb_Idx
ON test_gold_tempdb.dbo.tb_test_user(NUM_NID,VC_UID)

SELECT * FROM tb_test_Realuser

SELECT * FROM tb_test_Realuser WHERE NUM_NID < 651365 ORDER BY VC_UID ASC

SELECT * FROM tb_test_Realuser WHERE VC_UID = 'JJH4241'

SELECT * FROM tb_test_Realuser WHERE NUM_NID > 651365 AND VC_UID = 'JJH4241'

SELECT * FROM tb_test_user

SELECT * FROM tb_test_user WHERE NUM_NID = 651365 

SELECT * FROM tb_test_user WHERE VC_UID = 'JJH4241'

SELECT * FROM tb_test_user WHERE NUM_NID = 651365 AND VC_UID = 'JJH4241'

EXEC sp_helpIndex tb_test_user

create proc test_ggg
as

SELECT * FROM tb_test_user



exec test_ggg

drop proc test_ggg

Administrator



베팅

CREATE TABLE log_bet_zone_user
(
	n_betzone_idx	int,		-- 베팅한곳
	n_bet_nid		int,
	n_bet_money		int,		
	dt_bet_time		datetime,	
)

create 
tb_acct
(
	
)






	
	


exec sp_helpdb

exec sp_who2