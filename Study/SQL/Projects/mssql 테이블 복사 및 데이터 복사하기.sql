

-- �������̺�� �Ȱ��� ������ ���̺��� ����鼭 �����ͱ��� �ֱ�
=> SELECT * INTO �������̺�� FROM �������̺��

SELECT * INTO tb_test_user 
FROM [mobile_test].[dbo].[3YEAR]
WHERE NUM_NID = NULL AND VC_UID = NULL 

drop table tb_test_user

select * from tb_test_user

/*
--�ӽ����̺� ���鶧 
--Ư�� ��
SELECT Ư���� into tb_test_user
FROM [mobile_test].[dbo].[3YEAR]
WHERE Ư���� in ( ���� )


--�����ʹ� ���� ���̺� ( ������ ���� )
SELECT * into tb_test_user
FROM [mobile_test].[dbo].[3YEAR]
WHERE 1 = 2

--���̺��� �����ϸ鼭 �����͵� ���� ����

SELECT * INTO[���ο� ���̺��] FROM [���� ���̺��]

--������ ����
SELECT*INTO [���ο� ���̺�� FROM[���� ���̺��] WHERE 1=2

--���̺� �ִ� �����͸� ����
INSERT INTO [����� ���̺��] SELECT * FROM [���� ���̺��]

--Ư�� �����͸� ����
INSERT INTO [����� ���̺��]SELECT * FROM [���� ���̺��] WHERE ����
*/