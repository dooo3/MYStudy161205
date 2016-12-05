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




CREATE TABLE tb_Account
(
	nID INT IDENTITY PRIMARY KEY
,	nCI INT 
,	szID varchar(20) NOT NULL UNIQUE
)



CREATE TABLE tb_real_userInfo
(
	nCode INT PRIMARY KEY 
,	szPW varchar(20) NOT NULL	
,	pn varchar(11) NOT NULL	--����ȣ
,	sex	TINYINT NOT NULL DEFAULT 1 --1����0����
,	birthYear SMALLDATETIME NOT NULL -- �������
)




----------------------------------------proc_CI_add 
CREATE PROCEDURE proc_CI_add
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
EXEC proc_CI_add 'abc',@nRet OUTPUT --���ν��� ����
SELECT @nRet




----------------------------------------proc_get_nCI
CREATE PROCEDURE proc_get_nCI
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


/*
���� 00��
���� log_game20161028
��� log_game20161028 < -log_game
���� log_game
*/


-- �뷮����
-- 
CREATE TABLE log_account_create
(
	nLogIdx bigint identity(1,1),
	dtTime	datetime not null,
	szID varchar(20),
	nRet int not null
)


-- ������Ʈ �α����Ҷ�. 


---------------------------------------- proc_account_create
ALTER PROCEDURE proc_account_create
	@szCI		varchar(88),
	@szID		varchar(20),
	@szPW		varchar(20),
	@pn			varchar(11),
	@sex		TINYINT,
	@birthYear	SMALLDATETIME,
	@nRet		INT OUTPUT	-- 1:�������� ���� -1:������ 5���̻��̶� �������� ����, 2:������ ���̵��ִ�
AS

	SET @nRet = -1
	DECLARE @nCI int
	SET @nCI = 0
	EXEC proc_get_nCI @szCI,@nCI OUTPUT
	DECLARE @nTmpCnt INT
	
	DECLARE @nCode INT
	SET @nCode = 0
	
	
	IF @nCI > 0
	BEGIN
		SELECT @nTmpCnt = COUNT(0) FROM tb_Account WHERE nCI = @nCI
		IF @nTmpCnt < 5 --5�� ���϶��
		BEGIN
			INSERT INTO tb_Account(nCI,szID)VALUES(@nCI,@szID)
			EXEC proc_encryption @szID,@nCode OUTPUT 
			INSERT INTO tb_real_userInfo(nCode,szPW,pn,sex,birthYear) VALUES(@nCode,@szPW,@pn,@sex,@birthYear)	--account�� �����Է�
			SET @nRet = 1
		END
	END
		
GO

/*
�ε��� 
Ŭ������ �ε���		- �⺻Ű or CREATE CRUSTEREDINDEX	=> �ǵ����͸� ����(IN MDF).
��Ŭ������ �ε���	- CREATE NONCRUSTEREDINDEX			=> �ε��������� ����(IN LDF)
*/


---------------------------------------- proc_account_create
DECLARE @nRet INT
EXEC proc_account_create 'abc','��1','���1','01011111111',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 
DECLARE @nRet INT
EXEC proc_account_create 'abc','��2','���2','01022222222',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 
DECLARE @nRet INT
EXEC proc_account_create 'abc','��3','���3','01033333333',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 
DECLARE @nRet INT
EXEC proc_account_create 'abc','��4','���4','01044444444',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 



---------------------------------- proc_account_delete
ALTER PROCEDURE proc_account_delete
	@szID varchar(20),
	@szPW varchar(20),
	@nRet INT OUTPUT -- 1:��ġ�ϴ� ID����,-11:��й�ȣ Ʋ�� 0:��ġ�ϴ� ���̵� ���� 
AS

	SET @nRet = 0;
	DECLARE @nCode INT
	DECLARE @nDecode INT
	DECLARE @nEncode INT
	DECLARE @nID INT
	SET @nEncode = 0
	SET @nDecode = 0
	SET @nCode = 0
	SET @nID = 0
	
	DECLARE @checkPW INT
	SET @checkPW = 0
	
	DECLARE @keyCode INT
	--��������
	--@szID���� nID����
	--nID ��ȣȭ�ؼ� nEncode����
	--nEncode�� tb_real_userInfo�� ���� @szPW�� ������ ����
	-----pw�� �ٸ���� -11����
	--������ ���������� nEncode�� Decoding�ؼ�
	--tb_Account�� ������ �����Ѵ�	
	EXEC proc_get_code_key @szID,@keyCode OUTPUT
	
	SELECT @nID = nID FROM tb_Account WHERE szID = @szID 
	IF @nID > 0
	BEGIN
		EXEC proc_encryption_nID @nID,@nEncode OUTPUT
		SELECT @checkPW = COUNT(*) FROM tb_real_userInfo WHERE nCode = @nEncode AND szPW = @szPW
		IF @checkPW > 0 --���� ��й�ȣ���� ���ٸ�
		BEGIN
			DELETE FROM tb_real_userInfo WHERE nCode = @nEncode AND szPW = @szPW --����
			EXEC proc_decryption @nEncode,@keyCode,@nDecode OUTPUT
			DELETE FROM tb_Account WHERE tb_Account.szID = @szID AND tb_Account.nID = @nDecode
		END
		ELSE
		SET @nRet = -11
		
	END 
	ELSE
	SET	@nRet = -1
	
	
GO
---------------------------------- proc_account_delete

DECLARE @nRet INT
SET @nRet = 0 

EXEC proc_account_delete '��2','���2',@nRet OUTPUT

SELECT @nRet
		
	

	
-------------------------proc_account_get_numOfID
CREATE PROCEDURE proc_account_get_numOfID
	@szCI varchar(88)
,	@nRet INT OUTPUT -- 0:�ش� CI�� ���� ���̵� ����,1�̻�:�����ϴ� ���̵� ����
AS

	DECLARE @nCI INT
	
	SELECT @nCI = nCI FROM tb_CI WHERE tb_CI.szCI = @szCI
	SELECT @nRet = COUNT(0) FROM tb_Account WHERE tb_Account.nCI = @nCI
	
GO
-------------------------proc_account_get_numOfID
	

DECLARE @nNumAccount INT
EXEC proc_account_get_numOfID 'abc',@nNumAccount OUTPUT
SELECT @nNumAccount


--------------------------2016-10-27 ��ȣȭ �ϱ�


------------------------------------ proc_encryption
ALTER PROCEDURE proc_encryption
@szID varchar(20),
@nCode INT OUTPUT
AS
	DECLARE @nID INT
	DECLARE @keyCode INT
	SET @keyCode = 0
	EXEC proc_get_code_key @szID,@keyCode OUTPUT
	
	SELECT @nID = nID FROM tb_Account WHERE szID = @szID
	
	
	--------------��ȣȭ ���---------------
	
	SET @nCode = @nID + @keyCode 
	--------------��ȣȭ ���---------------
	
	
------------------------------------ proc_encryption
DECLARE @nCode INT
EXEC proc_encryption 'ID3',@nCode OUTPUT
SELECT @nCode

------------------------------------ proc_encryption_nID
ALTER PROCEDURE proc_encryption_nID
@nCode INT,
@nEncode INT OUTPUT
AS
	DECLARE @szID varchar(20)
	DECLARE @keyCode INT
	SET @keyCode = 0
	
	SELECT @szID = szID FROM tb_Account WHERE nID = @nCode
	EXEC proc_get_code_key @szID,@keyCode OUTPUT
	--------------��ȣȭ ���---------------
	
	SET @nEncode = @nCode + @keyCode 
	--------------��ȣȭ ���---------------
	
------------------------------------ proc_encryption_nID

------------------------------------ proc_decryption
ALTER PROCEDURE proc_decryption
@nCode INT,
@KeyCode INT,
@nDecode INT OUTPUT
AS
	--------------��ȣȭ ���---------------
	--nCode - 100
	SET @nDecode = @nCode - @keyCode 
	
	--SET @nDecode = @keyCode - @nCode  
	--------------��ȣȭ ���---------------
	
------------------------------------ proc_decryption

DECLARE @nDecode INT
EXEC proc_decryption 6,3,@nDecode OUTPUT
SELECT @nDecode

-------2016-10-28-------2016-10-28-------2016-10-28-------2016-10-28
CREATE PROCEDURE proc_get_code_key
	@szID varchar(20)
,	@keyCode INT OUTPUT
AS
	SELECT @keyCode = DATALENGTH(szID) FROM tb_Account WHERE szID = @szID
	
-------2016-10-28-------2016-10-28-------2016-10-28-------2016-10-28

DECLARE @len INT
SET @len = 0
EXEC proc_get_code_key '������',@len OUTPUT
SELECT @len

