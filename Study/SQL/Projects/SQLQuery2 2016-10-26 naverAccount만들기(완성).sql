USE test_glod_naverAccount

CREATE TABLE tb_CI
(
	nCI INT IDENTITY PRIMARY KEY
,	szCI varchar(88) NOT NULL 
)

CREATE TABLE tb_Account
(
	nID INT IDENTITY PRIMARY KEY
,	nCI INT 
,	szID varchar(20) NOT NULL UNIQUE
,	szPW varchar(20) NOT NULL	
,	pn INT NOT NULL	--����ȣ
,	sex	INT NOT NULL DEFAULT 1 --1����0����
,	birthYear INT NOT NULL -- �������
)


UPDATE tb_userInfo SET ID 


UPDATE tb_CI SET CI = 'ajsdnfkjnaskdfjnkajsdnfj'

INSERT INTO tb_CI(szCI) VALUES ('abc')

SELECT * FROM tb_CI
SELECT * FROM tb_userInfo











exec AddCI 'asdfasdfasdfasdf'

select * from tb_ci

----------------------------------------AddCI 
ALTER PROCEDURE AddCI
	@szCI varchar(88)
AS

declare @nTmpCnt INT
SELECT @nTmpCnt = COUNT(0) FROM tb_CI where szci= @szci
IF @nTmpCnt = 0
BEGIN
	INSERT INTO  tb_CI(szCI) VALUES(@szCI)
END

	
	
GO
----------------------------------------AddCI
EXEC AddCI @szCI = 'abcd' --���ν��� ����


----------------------------------------getnCI
ALTER PROCEDURE getnCI
	@szCI varchar(88)
AS
DECLARE @nCI INT

	SELECT @nCi = nCI FROM tb_CI WHERE szCI = @szCI
	if @@ROWCOUNT = 0
	BEGIN
		set @nCi = 0
	END
	
	RETURN @nCI
GO
----------------------------------------getnCI
DECLARE @nCI int

EXEC @nCI = getnCI @szCI = 'abc' -- ���ν��� ����

SELECT @nCI
----------------------------------------getCI
ALTER PROCEDURE getCI
	@nCI INT
AS
	DECLARE @nTmpCnt INT
	DECLARE @szCI varchar(88)
	
	SELECT @nTmpCnt = count(0) FROM tb_CI t WHERE t.nCI = @nCI
	SELECT @szCI = szCI FROM tb_CI t WHERE t.nCI = @nCI
	if @nTmpCnt = 0
	BEGIN
		PRINT '�ش��ϴ� zsCI����'
	END
	
	RETURN @szCI
GO
----------------------------------------getCI
DECLARE @outCI varchar(88) 
DECLARE @nCI INT

--EXEC @outCI = getCI @nCI = 1

EXEC getCI @nCI = 1
select @outCI 

--getCI�� �ϼ��������� ���� �� �ʿ� ����������~~~~~~



------------------------------------ableAddAccount
ALTER PROCEDURE ableAddAccount
	@nCI INT
AS
	DECLARE @RN INT
	 --userInfo�� nCi ROW�� ����� Ȯ���ؼ� � �ȵǸ� ���̵��Է¹������
	 SELECT @RN=COUNT(*)
	 FROM tb_Account t
	 WHERE t.nCI = @nCI
	 
	 RETURN @RN
GO
	


------------------------------------ableAddAccount
DECLARE @RN INT

EXEC @RN = ableAddAccount @nCI = 1

SELECT @RN


----------------------------------ȸ������ procedure



ALTER PROCEDURE proc_account_create
@szCI		varchar(88),
@szID		varchar(20),
@szPW		varchar(20),
@pn			int,
@sex		int,
@birthYear	int,
@nRet		INT OUTPUT	-- 1:�������� ���� -1:������ 5���̻��̶� �������� ����
AS

SET @nRet = -1

DECLARE @nCI int
EXEC @nCI = getnCI @szCI

DECLARE @nTmpCnt INT
SELECT @nTmpCnt = COUNT(0) FROM tb_Account WHERE nCI = @nCI
IF @nTmpCnt < 5 --5�� ���϶��
BEGIN
	INSERT INTO tb_Account(nCI,szID,szPW,pn,sex,birthYear) VALUES(@nCI,@szID,@szPW,@pn,@sex,@birthYear)	--account�� �����Է�
	SET @nRet = 1
END




�ε��� 
Ŭ������ �ε���		- �⺻Ű or CREATE CRUSTEREDINDEX	=> �ǵ����͸� ����(IN MDF).
��Ŭ������ �ε���	- CREATE NONCRUSTEREDINDEX			=> �ε��������� ����(IN LDF)




----------------------------------ȸ������ procedure	
DECLARE @nRet INT
EXEC proc_account_create 'abc','������','1234',1234,1,10,@nRet OUTPUT
SELECT @nRet



SELECT * FROM tb_Account



----------------------------------ȸ��Ż�� procedure
CREATE PROCEDURE deletAccount
	@szID varchar(20)
,	@szPW varchar(20)
AS
	DELETE FROM tb_Account WHERE tb_Account.szID = @szID AND tb_Account.szPW = @szPW
	

----------------------------------ȸ��Ż�� procedure
EXEC deletAccount @szID = '������',@szPW = '1234'
		
	
	
	
	
		
	
		