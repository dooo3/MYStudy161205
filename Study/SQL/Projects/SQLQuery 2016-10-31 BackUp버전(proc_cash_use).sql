USE test_glod_naverAccount

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


---------------------------------자료형 변환해야함
CREATE TABLE tb_Account
(
	nID BIGINT IDENTITY PRIMARY KEY
,	nCI INT 
,	szID varchar(20) NOT NULL UNIQUE
,	szPW varchar(20) NOT NULL	
,	szPhone varchar(11) NOT NULL	--폰번호
,	nSex TINYINT NOT NULL DEFAULT 1 --1남자0여자
,	szBirthYear SMALLDATETIME NOT NULL -- 생년월일
,	dtRegTime SMALLDATETIME 
,	dtLastLoginTime SMALLDATETIME 

)
---------------------------------자료형 변환해야함



SELECT GETDATE()
SELECT fn_time2int(GETDATE())
SELECT fn_int2time(1477640626)
1477640626


--DROP TABLE tb_Account_cash

CREATE TABLE tb_Account_cash
(
	nIdx INT IDENTITY PRIMARY KEY--key 하나 잡아서 lifeTime DESC해서 대소비교 안하고 위에꺼부터 차근차근 삭제해나가면 됨
,	szID varchar(20) NOT NULL
,	nCashType INT				-- 1:유료 2:무료
,	nCash INT
,	dtCash_lifeTime DATETIME --           현제시각과 비교해서 같아지면 cash 삭제         고유하고 적은순으로 
,	dtFree_cash_lifeTime DATETIME --      현제시각과 비교해서 같아지면 free_cash 삭제
)

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
	@nRet INT OUTPUT --0:새로운 CI추가,1:이미 CI가 존재
AS

	SET @nRet = 0





	SELECT @nRet = COUNT(0) FROM tb_CI where szCI = @szCI

	IF @nRet = 0
	BEGIN
		INSERT INTO  tb_CI(szCI) VALUES(@szCI)
	END

GO
----------------------------------------proc_CI_add
DECLARE @nRet INT
SET @nRet = 0
EXEC proc_CI_add 'abcd',@nRet OUTPUT --프로시저 사용법
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
		PRINT '해당하는 zsCI없음'
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
	@nRet		INT OUTPUT	-- 1:계정생성 성공 -1:계정이 5개이상이라 계정생성 실패
AS

	SET @nRet = -1
	DECLARE @nCI int
	SET @nCI = 0
	EXEC proc_get_nCI @szCI,@nCI OUTPUT
	DECLARE @nTmpCnt INT

	SELECT @nTmpCnt = COUNT(0) FROM tb_Account WHERE nCI = @nCI
	IF @nTmpCnt < 5 --5개 이하라면
	BEGIN
		INSERT INTO tb_Account(nCI,szID,szPW,pn,sex,birthYear) VALUES(@nCI,@szID,@szPW,@pn,@sex,@birthYear)	--account에 정보입력
		SET @nRet = 1
	END

GO

/*
인덱스 
클러스터 인덱스		- 기본키 or CREATE CRUSTEREDINDEX	=> 실데이터를 정렬(IN MDF).
넌클러스터 인덱스	- CREATE NONCRUSTEREDINDEX			=> 인덱스페이지 생성(IN LDF)
*/


---------------------------------------- proc_account_create
DECLARE @nRet INT
EXEC proc_account_create 'abcd','ID1','비번1','01055738058',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 

EXEC proc_account_create 'abcd','ID2','비번2','01055738058',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 

EXEC proc_account_create 'abcd','ID3','비번3','01055738058',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 

EXEC proc_account_create 'abcd','ID4','비번4','01055738058',1,"1990-12-20",@nRet OUTPUT
SELECT @nRet 




---------------------------------- proc_account_delete
ALTER PROCEDURE proc_account_delete
	@szID varchar(20),
	@szPW varchar(20),
	@nRet INT OUTPUT -- 1:삭제성공 0:일치하는 아이디 없음 -1:비밀번호 일치하지않음
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
EXEC proc_account_delete 'ID1','비번1',@nRet OUTPUT
EXEC proc_account_delete 'ID2','비번2',@nRet OUTPUT
EXEC proc_account_delete 'ID3','비번3',@nRet OUTPUT
EXEC proc_account_delete 'ID4','비번4',@nRet OUTPUT

SELECT @nRet
		
	


-------------------------proc_account_get_numOfID
ALTER PROCEDURE proc_account_get_numOfID
	@szCI varchar(88)
,	@nRet INT OUTPUT -- 0:해당 CI에 대한 아이디가 없음,1이상:존재하는 아이디 개수
AS

	DECLARE @nCI INT
	
	SELECT @nCI = nCI FROM tb_CI WHERE tb_CI.szCI = @szCI
	SELECT @nRet = COUNT(0) FROM tb_Account WHERE tb_Account.nCI = @nCI
	
GO
-------------------------proc_account_get_numOfID
	

DECLARE @nNumAccount INT
EXEC proc_account_get_numOfID 'abcd',@nNumAccount OUTPUT
SELECT @nNumAccount


--------------------------2016-10-27 암호화 하기


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



SELECT	title_id --AS '책번호'
,		qty --AS '수량'
,		CASE
			WHEN qty>= 50 THEN 'A'
			WHEN qty>= 30 THEN 'B'
			ELSE 'C'
		END --AS '등급'
		


--------------------------2016-10-28 캐쉬시스템 추가


-------------------------------- proc_get_cash
ALTER PROCEDURE proc_cash_get
	@szID varchar(20),
	@cash_type INT, --int형으로 바꾸기
	@nRet INT OUTPUT
AS

	
	
	IF @cash_type = 2 --무료
	BEGIN
		SELECT @nRet = nCash FROM tb_Account_cash WHERE szID = @szID
		--select top 10 * from tb_Account_cash
	END
	ELSE IF @cash_type = 1 --유료
 	BEGIN
		SELECT @nRet = nCash FROM tb_Account_cash WHERE szID = @szID
	END	
	ELSE 
	SET @nRet = -1
-------------------------------- proc_get_cash
DECLARE @nRet INT
EXEC proc_cash_get 'ID2','free_cash',@nRet OUTPUT
SELECT @nRet



-------------------------------- proc_cash_use
ALTER PROCEDURE proc_cash_use
	@szID varchar(20),
	@nUseCash INT,
	@nRet INT OUTPUT -- 1:성공 0:실패 -1:free_cash 부족 -2:cash부족
AS

	SET @nRet = 0
	DECLARE @tmpTotalCash INT
	DECLARE @tmpIdx INT
	DECLARE @tmpFreeCash INT
	DECLARE @tmpPaidCash INT
	DECLARE @nRemindCash INT
	
	
	
	-- 라이프 타임이 지난 것들 삭제
	DELETE FROM tb_Account_cash WHERE szID = @szID AND (GETDATE() > dtFree_cash_lifeTime OR GETDATE() > dtCash_lifeTime)
	
	--유료캐시 총합
	SELECT @tmpPaidCash = ISNULL(sum(nCash),0)
	FROM tb_Account_cash																			
	WHERE szID = @szID	AND nCashType = 1 
	--무료캐시 총합
	SELECT @tmpFreeCash = ISNULL(sum(nCash),0)
	FROM tb_Account_cash																			
	WHERE szID = @szID	AND nCashType = 2 
	
	
	SET @tmpTotalCash = @tmpFreeCash + @tmpPaidCash
	
	-- 1.무료 캐쉬갯수/유료 캐쉬갯수 가져오기
	-- 2.
	IF @nUseCash < @tmpTotalCash --총 보유캐시보다 낮은금액 사용
	BEGIN
		IF @tmpFreeCash > @nUseCash  -- 뮤료캐시로 끝나는 상황(사용 할 양보다 많을 때) 
		BEGIN
			--만약 첫번째 행에 있는 free_Cash값이 @nUseCash값보다 더 크다면 업데이트 or 같다면 첫번째 행만 삭제
			SELECT TOP 1 @tmpFreeCash = nFree_cash ,@tmptbCount = nTbCount FROM tb_Account_cash WHERE szID = @szID order BY free_cash_lifeTime asc
			
			
		END 
		ELSE IF @tmpFreeCash = @nUseCash -- 무료 캐시로 끝나는 상황(사용 할 양과 같을 때)
		BEGIN
		
		END 
		ELSE IF @tmpFreeCash < @nUseCash -- 유료 캐시로 넘어가야하는 상황(@nRemindCash를 유료캐시로 넘겨줘야 할 때)
		BEGIN
			--유료 캐시 처리시작 (@nRemindCash 사용 시작)
			IF @tmpPaidCash > @nRemindCash --유료 캐시로 결제 가능 상황 
			BEGIN
			END ELSE IF @tmpPaidCash = @nRemindCash --같은 금액 깔끔하게 삭제만 하면됨
			BEGIN
			
			END
		
		END	
		
	END
	ELSE--보유 캐시가 사용 할 캐시보다 부족합니다
	BEGIN
		SELECT '보유 캐시가 부족합니다'
	END
	-- 충분하다.
	
	
-------------------------------- proc_cash_use
DECLARE @nRet INT
EXEC proc_cash_use 'ID2',101,@nRet OUTPUT
SELECT @nRet AS '리턴값'


-------------------------------- proc_cash_add
ALTER PROCEDURE proc_cash_add
	@szID varchar(20),
	@cash_type INT,
	@pay_type INT,
	@val INT, -- free_cash에만 쓸 valuse
	@nRet INT OUTPUT -- 1:성공 ,0:실패 , -1:type_Error
	
AS
	DECLARE @price_type char(2)
	SET @nRet = 0
	
	DECLARE @lifeTime DATETIME
	SET @lifeTime = GETDATE()
	
	IF @cash_type = 2 --무료 
	BEGIN
		INSERT INTO tb_Account_cash(nCash,szID,dtFree_cash_lifeTime) VALUES (@val,@szID,DATEADD(Month,6,@lifeTime))
		SELECT @nRet = nCash FROM tb_Account_cash WHERE szID = @szID -- AND nCashType = @cash_type
	END 
	ELSE IF @cash_type = 1 --유료
	BEGIN
	
		IF @val = 1000
		BEGIN
			SET @price_type = 'A'
		END
		IF @val = 5000
		BEGIN
			SET @price_type = 'B'
		END
		IF @val = 10000
		BEGIN
			SET @price_type = 'C'
		END
		IF @val = 30000
		BEGIN
			SET @price_type = 'D'
		END
		
		EXEC proc_cash_payment @szID ,@pay_type ,@price_type,@nRet OUTPUT
		--결제수단 2가지
		--결제상품 1000/5000/10000/30000
		--1000원 결제시 유료캐쉬 100충전
		SELECT @nRet = cash FROM tb_Account_cash WHERE szID = @szID		
	END
	ELSE
	SET @nRet = -1
	
	--nIdx
	
-------------------------------- proc_cash_add
DECLARE @nRet INT
EXEC proc_cash_add 'ID2','free_cash',0,200,@nRet OUTPUT

DECLARE @nRet INT
EXEC proc_cash_add 'ID2','cash',0,1000,@nRet OUTPUT

SELECT @nRet
--UPDATE tb_Account SET cash = 0 WHERE szID = 'ID2'

----------------------------------
ALTER PROCEDURE proc_cash_payment
	@szID varchar(20),
	@pay_type INT, -- 0 : 신용카드, 1: 휴대폰
	@price_type char(2), -- ABCDN A:1000, B:5000,C:10000,D:30000,N:무료
	@nRet INT OUTPUT     -- 0: 실패  1:A,2:B,3:C,4:D,-1:type_Error
AS
	DECLARE @tmpCash INT
	SET @tmpCash = 0
	SELECT @tmpCash = nCash FROM tb_Account_cash WHERE szID = @szID
	
	SET @nRet = 0
	
	
	DECLARE @lifeTime DATETIME
	
	SET @lifeTime = GETDATE()
	
	--SELECT fn_time2int(@lifeTime)
	--SELECT @lifeTime
	
	
	IF @price_type = 'A'
	BEGIN
		INSERT INTO tb_Account_cash(nCash,szID,dtCash_lifeTime) VALUES (100,@szID,DATEADD(Year,5,@lifeTime))
		--SELECT @nRet = cash FROM tb_Account_cash WHERE szID = @szID
		SET @nRet = 1
	END
	ELSE IF @price_type = 'B'
	BEGIN
		INSERT INTO tb_Account_cash(nCash,szID,dtCash_lifeTime) VALUES (500,@szID,DATEADD(Year,5,@lifeTime))
		--SELECT @nRet = cash FROM tb_Account_cash WHERE szID = @szID
		SET @nRet = 2
	END
	ELSE IF @price_type = 'C'
	BEGIN
		INSERT INTO tb_Account_cash(nCash,szID,dtCash_lifeTime) VALUES (1000,@szID,DATEADD(Year,5,@lifeTime))
		--SELECT @nRet = cash FROM tb_Account_cash WHERE szID = @szID
		SET @nRet = 3
	END
	ELSE IF @price_type = 'D'
	BEGIN
		INSERT INTO tb_Account_cash(nCash,szID,dtCash_lifeTime) VALUES (3000,@szID,DATEADD(Year,5,@lifeTime))
		--SELECT @nRet = cash FROM tb_Account_cash WHERE szID = @szID
		SET @nRet = 4
	END ELSE SET @nRet = -1


/*
 tb_Account 
SET cash = cash + CASE 
				WHEN @price_type = 'A' THEN  1000 --SET @nRet = 1
				WHEN @price_type = 'B' THEN  5000 --SET @nRet = 2
				WHEN @price_type = 'C' THEN  10000 --SET @nRet = 3 
				WHEN @price_type = 'D' THEN  30000 --SET @nRet = 4
		   END
WHERE szID = @szID
*/

----------------------------------
DECLARE @nRet INT
EXEC proc_cash_payment 'ID1',0,'A',@nRet OUTPUT
SELECT @nRet
	


----------------------------------------proc_lifeTime_check
CREATE PROCEDURE proc_validTime_check
	@szID varchar(20),
	@nRet INT OUTPUT
AS
	
	DECLARE cash_LT 
	DECLARE free_cash_LT
	
	SELECT cash_LT = dtCash_lifeTime FROM tb_Account WHERE szID = @szID
	SELECT free_cash_LT = dtFree_cash_lifeTime FROM tb_Account WHERE szID = @szID
	
	
	--현제시각과 lifeTime이 같으면 DELETE해준다~
	--유료 5년 뒤, 무료 6개월뒤
----------------------------------------proc_lifeTime_check
