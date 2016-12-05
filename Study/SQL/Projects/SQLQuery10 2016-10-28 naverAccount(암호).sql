CREATE TABLE tb_CI
(
	nCI INT IDENTITY PRIMARY KEY
,	szCI varchar(88) NOT NULL UNIQUE
)


/*
ALTER TABLE (테이블)
ADD CONSTRAINT PRIMARY KEY (열)

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
,	pn varchar(11) NOT NULL	--폰번호
,	sex	TINYINT NOT NULL DEFAULT 1 --1남자0여자
,	birthYear SMALLDATETIME NOT NULL -- 생년월일
)




----------------------------------------proc_CI_add 
CREATE PROCEDURE proc_CI_add
	@szCI varchar(88),
	@nRet INT OUTPUT --0:새로운 CI추가,1:이미 CI가 존재
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
EXEC proc_CI_add 'abc',@nRet OUTPUT --프로시저 사용법
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
		PRINT '해당하는 zsCI없음'
	END
	
GO
----------------------------------------proc_get_CI
DECLARE @outCI varchar(88) 
DECLARE @nCI INT

EXEC proc_get_CI  1,@outCI OUTPUT

select @outCI 


/*
매일 00시
생성 log_game20161028
백업 log_game20161028 < -log_game
삭제 log_game
*/


-- 용량문제
-- 
CREATE TABLE log_account_create
(
	nLogIdx bigint identity(1,1),
	dtTime	datetime not null,
	szID varchar(20),
	nRet int not null
)


-- 웹사이트 로그인할때. 


---------------------------------------- proc_account_create
ALTER PROCEDURE proc_account_create
	@szCI		varchar(88),
	@szID		varchar(20),
	@szPW		varchar(20),
	@pn			varchar(11),
	@sex		TINYINT,
	@birthYear	SMALLDATETIME,
	@nRet		INT OUTPUT	-- 1:계정생성 성공 -1:계정이 5개이상이라 계정생성 실패, 2:동일한 아이디가있다
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
		IF @nTmpCnt < 5 --5개 이하라면
		BEGIN
			INSERT INTO tb_Account(nCI,szID)VALUES(@nCI,@szID)
			EXEC proc_encryption @szID,@nCode OUTPUT 
			INSERT INTO tb_real_userInfo(nCode,szPW,pn,sex,birthYear) VALUES(@nCode,@szPW,@pn,@sex,@birthYear)	--account에 정보입력
			SET @nRet = 1
		END
	END
		
GO

/*
인덱스 
클러스터 인덱스		- 기본키 or CREATE CRUSTEREDINDEX	=> 실데이터를 정렬(IN MDF).
넌클러스터 인덱스	- CREATE NONCRUSTEREDINDEX			=> 인덱스페이지 생성(IN LDF)
*/


---------------------------------------- proc_account_create
DECLARE @nRet INT
EXEC proc_account_create 'abc','유1','비번1','01011111111',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 
DECLARE @nRet INT
EXEC proc_account_create 'abc','유2','비번2','01022222222',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 
DECLARE @nRet INT
EXEC proc_account_create 'abc','유3','비번3','01033333333',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 
DECLARE @nRet INT
EXEC proc_account_create 'abc','유4','비번4','01044444444',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 



---------------------------------- proc_account_delete
ALTER PROCEDURE proc_account_delete
	@szID varchar(20),
	@szPW varchar(20),
	@nRet INT OUTPUT -- 1:일치하는 ID존재,-11:비밀번호 틀림 0:일치하는 아이디 없음 
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
	--삭제과정
	--@szID에서 nID추출
	--nID 암호화해서 nEncode추출
	--nEncode로 tb_real_userInfo에 접근 @szPW도 같으면 삭제
	-----pw가 다를경우 -11리턴
	--삭제에 성공했으면 nEncode를 Decoding해서
	--tb_Account의 정보도 삭제한다	
	EXEC proc_get_code_key @szID,@keyCode OUTPUT
	
	SELECT @nID = nID FROM tb_Account WHERE szID = @szID 
	IF @nID > 0
	BEGIN
		EXEC proc_encryption_nID @nID,@nEncode OUTPUT
		SELECT @checkPW = COUNT(*) FROM tb_real_userInfo WHERE nCode = @nEncode AND szPW = @szPW
		IF @checkPW > 0 --만약 비밀번호까지 같다면
		BEGIN
			DELETE FROM tb_real_userInfo WHERE nCode = @nEncode AND szPW = @szPW --삭제
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

EXEC proc_account_delete '유2','비번2',@nRet OUTPUT

SELECT @nRet
		
	

	
-------------------------proc_account_get_numOfID
CREATE PROCEDURE proc_account_get_numOfID
	@szCI varchar(88)
,	@nRet INT OUTPUT -- 0:해당 CI에 대한 아이디가 없음,1이상:존재하는 아이디 개수
AS

	DECLARE @nCI INT
	
	SELECT @nCI = nCI FROM tb_CI WHERE tb_CI.szCI = @szCI
	SELECT @nRet = COUNT(0) FROM tb_Account WHERE tb_Account.nCI = @nCI
	
GO
-------------------------proc_account_get_numOfID
	

DECLARE @nNumAccount INT
EXEC proc_account_get_numOfID 'abc',@nNumAccount OUTPUT
SELECT @nNumAccount


--------------------------2016-10-27 암호화 하기


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
	
	
	--------------암호화 방식---------------
	
	SET @nCode = @nID + @keyCode 
	--------------암호화 방식---------------
	
	
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
	--------------암호화 방식---------------
	
	SET @nEncode = @nCode + @keyCode 
	--------------암호화 방식---------------
	
------------------------------------ proc_encryption_nID

------------------------------------ proc_decryption
ALTER PROCEDURE proc_decryption
@nCode INT,
@KeyCode INT,
@nDecode INT OUTPUT
AS
	--------------복호화 방식---------------
	--nCode - 100
	SET @nDecode = @nCode - @keyCode 
	
	--SET @nDecode = @keyCode - @nCode  
	--------------복호화 방식---------------
	
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
EXEC proc_get_code_key '유동금',@len OUTPUT
SELECT @len

