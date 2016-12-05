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
,	pn INT NOT NULL	--폰번호
,	sex	INT NOT NULL DEFAULT 1 --1남자0여자
,	birthYear INT NOT NULL -- 생년월일
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
EXEC AddCI @szCI = 'abcd' --프로시저 사용법


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

EXEC @nCI = getnCI @szCI = 'abc' -- 프로시저 사용법

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
		PRINT '해당하는 zsCI없음'
	END
	
	RETURN @szCI
GO
----------------------------------------getCI
DECLARE @outCI varchar(88) 
DECLARE @nCI INT

--EXEC @outCI = getCI @nCI = 1

EXEC getCI @nCI = 1
select @outCI 

--getCI는 완성못했지만 딱히 쓸 필요 없을꺼같다~~~~~~



------------------------------------ableAddAccount
ALTER PROCEDURE ableAddAccount
	@nCI INT
AS
	DECLARE @RN INT
	 --userInfo에 nCi ROW가 몇개인지 확인해서 몇개 안되면 아이디를입력받으면됨
	 SELECT @RN=COUNT(*)
	 FROM tb_Account t
	 WHERE t.nCI = @nCI
	 
	 RETURN @RN
GO
	


------------------------------------ableAddAccount
DECLARE @RN INT

EXEC @RN = ableAddAccount @nCI = 1

SELECT @RN


----------------------------------회원가입 procedure



ALTER PROCEDURE proc_account_create
@szCI		varchar(88),
@szID		varchar(20),
@szPW		varchar(20),
@pn			int,
@sex		int,
@birthYear	int,
@nRet		INT OUTPUT	-- 1:계정생성 성공 -1:계정이 5개이상이라 계정생성 실패
AS

SET @nRet = -1

DECLARE @nCI int
EXEC @nCI = getnCI @szCI

DECLARE @nTmpCnt INT
SELECT @nTmpCnt = COUNT(0) FROM tb_Account WHERE nCI = @nCI
IF @nTmpCnt < 5 --5개 이하라면
BEGIN
	INSERT INTO tb_Account(nCI,szID,szPW,pn,sex,birthYear) VALUES(@nCI,@szID,@szPW,@pn,@sex,@birthYear)	--account에 정보입력
	SET @nRet = 1
END




인덱스 
클러스터 인덱스		- 기본키 or CREATE CRUSTEREDINDEX	=> 실데이터를 정렬(IN MDF).
넌클러스터 인덱스	- CREATE NONCRUSTEREDINDEX			=> 인덱스페이지 생성(IN LDF)




----------------------------------회원가입 procedure	
DECLARE @nRet INT
EXEC proc_account_create 'abc','유동금','1234',1234,1,10,@nRet OUTPUT
SELECT @nRet



SELECT * FROM tb_Account



----------------------------------회원탈퇴 procedure
CREATE PROCEDURE deletAccount
	@szID varchar(20)
,	@szPW varchar(20)
AS
	DELETE FROM tb_Account WHERE tb_Account.szID = @szID AND tb_Account.szPW = @szPW
	

----------------------------------회원탈퇴 procedure
EXEC deletAccount @szID = '유동금',@szPW = '1234'
		
	
	
	
	
		
	
		