--���Ϸ� ���
SET LANGUAGE '�ѱ���'
BACKUP DATABASE test_gold_tempdb 
TO DISK = 'C:\BAK_TEST.bak' 
WITH INIT
--�α׹��
BACKUP LOG test_gold_tempdb TO DISK = 'C:\Log' 
 


UPDATE a SET szName = 'ù��°���' WHERE nId = 1


UPDATE a SET szName = '�������ı�' WHERE nId = 2

UPDATE a SET szName = '����°����' WHERE nId = 3


UPDATE a SET szName = '�Ǽ�'

SELECT TOP 3* FROM a 

SELECT GETDATE()

2016-11-08 17:04:54.280
�����߻��ð�

--��� �α׸� ���
BACKUP LOG test_gold_tempdb TO DISK = 'c:\LogNoTrunc_ver2' WITH NO_TRUNCATE, INIT
--��������--

--Ǯ������� �������
RESTORE DATABASE test_gold_tempdb FROM DISK = 'C:\BAK_TEST.bak'
WITH NORECOVERY

--ù��° �α� ��� ���� noRecovery��! stopat���� ��¥ �����Ұ��� �����ֱ� ������
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


--�߿��� stopat���� ���� 
--�߻� �ð� ���� -8�� ����
RESTORE LOG test_gold_tempdb FROM DISK = 'c:\LogNoTrunc_ver2'
WITH STOPAT = '2016-11-08 17:04: .280' , RECOVERY



--���� ����
/*
	1.master�� �ٲ�
	
	2.��ȸ ���� Ȯ��(�ȵǸ� ����)
	
	3-1 �����ͺ��̽� ���� 
	3-2 ����Ǯ������������ϴ°��̴�
	3-3 ����LOG�������ҰԴ������Ƿοɼ�����
	3-4 WITH NORECOVERY�� ���δ�
*/
USE master
--USE test_gold_tempdb

SELECT TOP 1 szName FROM a

UPDATE a SET szName = '�������ı�' WHERE nId = 1

restore database test_gold_tempdb FROM DISK = 'C:\test_gold_tempdb_BAK_TEST.bak'
WITH NORECOVERY

RESTORE LOG test_gold_tempdb FROM DISK = 'C:\gold_Log' 
WITH RECOVERY 

EXEC sp_helpdb test_gold_tempdb
 
EXEC sp_who test_gold_tempdb