USE test_glod_naverAccount

CREATE TABLE tb_CI
(
	nCI INT IDENTITY PRIMARY KEY
,	szCI varchar(88) NOT NULL UNIQUE
)

/*
ALTER TABLE (���̺�)
ADD CONSTRAINT PRIMARY KEY (��)

ALTER TABLE (tb_Account)
ADD CONSTRAINT PRIMARY KEY (nID)

ALTER TABLE tb_Account
ADD CONSTRAINT PK__tb_Account__3D5E1FD2 NONCLUSTERED nID
*/


sp_helpINDEX tb_Account



---------------------------------�ڷ��� ��ȯ�ؾ���
CREATE TABLE tb_Account
(
	nID BIGINT IDENTITY PRIMARY KEY
,	nCI INT 
,	szID varchar(20) NOT NULL UNIQUE
,	szPW varchar(20) NOT NULL	
,	pn varchar(11) NOT NULL	--����ȣ
,	sex	TINYINT NOT NULL DEFAULT 1 --1����0����
,	birthYear SMALLDATETIME NOT NULL -- �������
,	dtRegTime SMALLDATETIME 
,	dtLastLoginTime SMALLDATETIME 
,	cash INT
,	free_cash INT
)
---------------------------------�ڷ��� ��ȯ�ؾ���

UPDATE tb_userInfo SET ID 

UPDATE tb_CI SET CI = 'ajsdnfkjnaskdfjnkajsdnfj'

INSERT INTO tb_CI(szCI) VALUES ('abc')

SELECT * FROM tb_CI
SELECT * FROM tb_userInfo






-------------------2016-10-28-------------------2016-10-28

CREATE TABLE log_account_create
(
	nLogIdx bigint identity(1,1),
	dtTime	datetime not null,
	szID varchar(20),
	nRet int not null
)


-------------------2016-10-28-------------------2016-10-28




----------------------------------------proc_CI_add 
ALTER PROCEDURE proc_CI_add
	@szCI varchar(88),
	@nRet INT OUTPUT --0:���ο� CI�߰�,1:�̹� CI�� ����
AS

	SET @nRet = 0

	SELECT @nRet = COUNT(0) FROM tb_CI where tb_CI.szCI = @szCI

	IF @nRet = 0
	BEGIN
		INSERT INTO  tb_CI(szCI) VALUES(@szCI)
	END

GO
----------------------------------------proc_CI_add
DECLARE @nRet INT
SET @nRet = 0
EXEC proc_CI_add 'abcd',@nRet OUTPUT --���ν��� ����
SELECT @nRet




----------------------------------------proc_get_nCI
ALTER PROCEDURE proc_get_nCI
	@szCI varchar(88),
	@nCI INT OUTPUT
AS

SELECT @nCi = nCI FROM tb_CI WHERE szCI = @szCI
if @@ROWCOUNT = 0
BEGIN
	set @nCI = 0
END

GO
----------------------------------------proc_get_nCI
DECLARE @nCI int

EXEC proc_get_nCI 'abc',@nCI OUTPUT

SELECT @nCI
----------------------------------------proc_get_CI
CREATE PROCEDURE proc_get_CI
	@nCI INT,
	@szCI varchar(88) OUTPUT
AS

	DECLARE @nTmpCnt INT

	SELECT @nTmpCnt = count(0) FROM tb_CI t WHERE t.nCI = @nCI
	SELECT @szCI = szCI FROM tb_CI t WHERE t.nCI = @nCI
	if @nTmpCnt = 0
	BEGIN
		PRINT '�ش��ϴ� zsCI����'
	END
	
GO
----------------------------------------proc_get_CI
DECLARE @outCI varchar(88) 
DECLARE @nCI INT

EXEC proc_get_CI  1,@outCI OUTPUT

select @outCI 




---------------------------------------- proc_account_create
ALTER PROCEDURE proc_account_create
	@szCI		varchar(88),
	@szID		varchar(20),
	@szPW		varchar(20),
	@pn			varchar(11),
	@sex		TINYINT,
	@birthYear	SMALLDATETIME,
	@nRet		INT OUTPUT	-- 1:�������� ���� -1:������ 5���̻��̶� �������� ����
AS

	SET @nRet = -1
	DECLARE @nCI int
	SET @nCI = 0
	EXEC proc_get_nCI @szCI,@nCI OUTPUT
	DECLARE @nTmpCnt INT

	SELECT @nTmpCnt = COUNT(0) FROM tb_Account WHERE nCI = @nCI
	IF @nTmpCnt < 5 --5�� ���϶��
	BEGIN
		INSERT INTO tb_Account(nCI,szID,szPW,pn,sex,birthYear) VALUES(@nCI,@szID,@szPW,@pn,@sex,@birthYear)	--account�� �����Է�
		SET @nRet = 1
	END

GO

/*
�ε��� 
Ŭ������ �ε���		- �⺻Ű or CREATE CRUSTEREDINDEX	=> �ǵ����͸� ����(IN MDF).
��Ŭ������ �ε���	- CREATE NONCRUSTEREDINDEX			=> �ε��������� ����(IN LDF)
*/


---------------------------------------- proc_account_create
DECLARE @nRet INT
EXEC proc_account_create 'abcd','ID1','���1','01055738058',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 

EXEC proc_account_create 'abcd','ID2','���2','01055738058',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 

EXEC proc_account_create 'abcd','ID3','���3','01055738058',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 

EXEC proc_account_create 'abcd','ID4','���4','01055738058',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 




---------------------------------- proc_account_delete
ALTER PROCEDURE proc_account_delete
	@szID varchar(20),
	@szPW varchar(20),
	@nRet INT OUTPUT -- 1:�������� 0:��ġ�ϴ� ���̵� ���� -1:��й�ȣ ��ġ��������
AS

	SET @nRet = 0;

	SELECT @nRet = COUNT(0) FROM tb_Account WHERE tb_Account.szID = @szID
	IF @nRet > 0
	BEGIN
		SELECT @nRet = COUNT(0) FROM tb_Account WHERE szPW = @szPW
		IF @nRet > 0
		BEGIN
			DELETE FROM tb_Account WHERE tb_Account.szID = @szID AND tb_Account.szPW = @szPW
		END
		ELSE
		SET @nRet = -1
	END
	

GO
---------------------------------- proc_account_delete

DECLARE @nRet INT
SET @nRet = 0 

EXEC proc_account_delete 'ID1','���1',@nRet OUTPUT

SELECT @nRet
		
	

	
-------------------------proc_account_get_numOfID
ALTER PROCEDURE proc_account_get_numOfID
	@szCI varchar(88)
,	@nRet INT OUTPUT -- 0:�ش� CI�� ���� ���̵� ����,1�̻�:�����ϴ� ���̵� ����
AS

	DECLARE @nCI INT
	
	SELECT @nCI = nCI FROM tb_CI WHERE tb_CI.szCI = @szCI
	SELECT @nRet = COUNT(0) FROM tb_Account WHERE tb_Account.nCI = @nCI
	
GO
-------------------------proc_account_get_numOfID
	

DECLARE @nNumAccount INT
EXEC proc_account_get_numOfID 'abcd',@nNumAccount OUTPUT
SELECT @nNumAccount


--------------------------2016-10-27 ��ȣȭ �ϱ�


------------------------------------ proc_encryption
CREATE PROCEDURE proc_encryption
@szID varchar(20),
@nCode INT OUTPUT
AS
	DECLARE @nID INT
	SELECT @nID = nID FROM tb_Account WHERE szID = @szID
	
	SET @nCode = @nID + 100
	
------------------------------------ proc_encryption
DECLARE @nCode INT
EXEC proc_encryption 'ID3',@nCode OUTPUT
SELECT @nCode

------------------------------------ proc_decryption
CREATE PROCEDURE proc_decryption
@nCode INT,
@nDecode INT OUTPUT
AS
	SET @nDecode = @nCode - 100
------------------------------------ proc_decryption

DECLARE @nDecode INT
EXEC proc_decryption 101,@nDecode OUTPUT
SELECT @nDecode


