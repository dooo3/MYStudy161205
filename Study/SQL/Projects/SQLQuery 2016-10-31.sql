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



DECLARE @nRet INT
EXEC proc_cash_add 'ID2',2,0,125,@nRet OUTPUT


EXEC proc_cash_add 'ID2',1,0,1000,@nRet OUTPUT




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

	SELECT @nRet = COUNT(0) FROM tb_Account WHERE szID = @szID --같은 아이디가 있는지
	IF @nRet > 0
	BEGIN
		SELECT @nRet = COUNT(0) FROM tb_Account WHERE szPW = @szPW --같은 비밀번호가 있는지
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
	
	SELECT @nCI = nCI FROM tb_CI WHERE szCI = @szCI
	SELECT @nRet = COUNT(0) FROM tb_Account WHERE nCI = @nCI
	
GO
-------------------------proc_account_get_numOfID
	

DECLARE @nNumAccount INT
EXEC proc_account_get_numOfID 'abcd',@nNumAccount OUTPUT
SELECT @nNumAccount




--------------------------2016-10-28 캐쉬시스템 추가


-------------------------------- proc_get_cash
ALTER PROCEDURE proc_cash_get
	@szID varchar(20),
	@cash_type INT, --1:유료 , 2:무료
	@nRet INT OUTPUT
AS

	IF @cash_type = 2 --무료
	BEGIN
		SELECT @nRet = ISNULL(SUM(nCash),0) FROM tb_Account_cash WHERE szID = @szID AND nCashType = @cash_type 
		--select top 10 * from tb_Account_cash
	END
	ELSE IF @cash_type = 1 --유료
 	BEGIN
		SELECT @nRet = ISNULL(SUM(nCash),0) FROM tb_Account_cash WHERE szID = @szID AND nCashType = @cash_type 
	END	
	ELSE 
	SET @nRet = -1
-------------------------------- proc_get_cash
DECLARE @nRet INT
EXEC proc_cash_get 'ID2',1,@nRet OUTPUT
SELECT @nRet



-------------------------------- proc_cash_use
ALTER PROCEDURE proc_cash_use
	@szID varchar(20),
	@nUseCash INT,
	@nRet INT OUTPUT -- 1:성공 0:실패 -1:free_cash 부족 -2:cash부족
AS

	SET @nRet = 0
	DECLARE @tmpTotalCash INT
	
	DECLARE @tmpPaidCash INT
	DECLARE @nRemindCash INT
	DECLARE @tmpFirstIdx_FreeCash INT
	DECLARE @tmpFirst_FreeCash INT
	
	DECLARE @tmpIdx INT
	DECLARE @tmpFreeCash INT
	DECLARE @tmpSumCash INT
	
	DECLARE @tmpSumFreeCash INT
	
	SET @tmpFirst_FreeCash = 0	
	SET @nRemindCash = @nUseCash
	--SELECT @nRemindCash AS '@nRemindCash'
	
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
	
	SELECT @tmpPaidCash AS 'tmpPaidCash',@tmpFreeCash AS 'tmpFreeCash' ,@tmpTotalCash AS 'tmpTotalCash'
	
	-- 1.무료 캐쉬갯수/유료 캐쉬갯수 가져오기
	-- 2.
	IF @nUseCash < 0
	BEGIN
		SELECT '음수입니다'
	END ELSE
	BEGIN
		IF @nUseCash <= @tmpTotalCash --총 보유캐시보다 낮은금액 사용
		BEGIN
		
			--DECLARE @tmpIdx INT
			--DECLARE @tmpFreeCash INT
			--DECLARE @tmpSumCash INT	
		
			SET @tmpIdx  =0
			SET @tmpFreeCash =0
			SET @tmpSumCash =0	
		
			SELECT @tmpIdx = X.nIdx , @tmpFreeCash = X.nCash, @tmpSumCash = ISNULL(sum(Y.nCash),0) 
			FROM tb_Account_cash X INNER JOIN tb_Account_cash Y 
			ON   X.nIdx >= Y.nIdx AND Y.nCashType = 1
			GROUP BY X.nIdx, X.nCashType ,X.nCash
			HAVING ISNULL(sum(Y.nCash),0) <= @nUseCash--@nUseCash
			
			
			SELECT @tmpIdx AS 'tmpIdx', @tmpFreeCash AS 'tmpFreeCash',  @tmpSumCash AS 'tmpSumCash'
			
			--만약 첫번째 행에 있는 free_Cash값이 @nUseCash값보다 더 크다면 업데이트 or 같다면 첫번째 행만 삭제
			SELECT TOP 1 @tmpFirst_FreeCash = nCash ,@tmpFirstIdx_FreeCash = nIdx FROM tb_Account_cash WHERE nCashType = 2 AND szID = @szID order BY dtFree_cash_lifeTime asc
				 
			SELECT @tmpFirst_FreeCash AS 'tmpFirst_FreeCash'
				
			IF @tmpFirst_FreeCash > @nUseCash  -- 무료캐시로 끝나는 상황(사용 할 양보다 많을 때) 
			BEGIN
				
				UPDATE tb_Account_cash SET nCash = nCash - @nUseCash  WHERE nIdx = @tmpFirstIdx_FreeCash And nCashType =2
					SET @nRemindCash = 0
			END 
			ELSE IF @tmpFirst_FreeCash = @nUseCash -- 무료 캐시로 끝나는 상황(사용 할 양과 같을 때)
			BEGIN
				DELETE FROM tb_Account_cash WHERE nIdx = @tmpFirstIdx_FreeCash AND nCashType = 2
				SET @nRemindCash = 0
				SET @nRet = @tmpFirst_FreeCash
			
			END 
			ELSE IF @tmpFirst_FreeCash < @nUseCash -- 유료 캐시로 넘어가야하는 상황(@nRemindCash를 유료캐시로 넘겨줘야 할 때)
			BEGIN
				
				SELECT @tmpFirst_FreeCash AS 'tmpFirst_FreeCash~~~~CheckNULL'
				
				
				IF @tmpFirst_FreeCash > 0
				BEGIN
					SELECT '@tmpFirst_FreeCash < @nUseCash'
				
					SELECT @nRemindCash AS 'RemindCash' 
					
					DECLARE @lastFreeCash INT
					DECLARE @tmpNextIdx_freeCash INT
					
					IF @tmpIdx = 0 
					BEGIN
						SET	@tmpIdx = @tmpFirstIdx_FreeCash
					END
					
					
					SELECT @tmpSumFreeCash = ISNULL(SUM(nCash),0) 
					FROM tb_Account_cash WHERE nIdx <= @tmpIdx AND nCashType = 2
					
					SELECT @tmpIdx AS '@tmpIdx   eee'
					
					DELETE FROM tb_Account_cash WHERE szID = @szID AND nIdx <= @tmpIdx AND nCashType = 2
					SELECT @tmpSumFreeCash AS 'tmpSumFreeCash'
					
					SET @nRemindCash = @nRemindCash - @tmpSumFreeCash
					
					SELECT @nRemindCash AS '마이너스 하고 난 후 nRemindCash'
					
					SELECT @tmpNextIdx_freeCash = nIdx FROM tb_Account_cash WHERE nIdx > @tmpIdx AND nCashType = 2
					
					SELECT @tmpNextIdx_freeCash AS '@tmpNextIdx_freeCash' 
					
					SELECT @lastFreeCash = nCash FROM tb_Account_cash WHERE nIdx = @tmpNextIdx_freeCash AND nCashType = 2
					
					SELECT @lastFreeCash AS 'lastFreeCash'
					SELECT @tmpIdx AS 'tmpIdx'
					
					IF @lastFreeCash > @nRemindCash
					BEGIN
						UPDATE tb_Account_cash SET nCash = nCash - @nRemindCash WHERE szID = @szID AND nIdx = @tmpNextIdx_freeCash AND nCashType = 2
						SET @nRemindCash = @nRemindCash - @lastFreeCash
					END ELSE IF @lastFreeCash = @nRemindCash
					BEGIN
						DELETE FROM tb_Account_cash WHERE szID = @szID AND nIdx = @tmpNextIdx_freeCash AND nCashType = 2	
						SET @nRemindCash = @nRemindCash - @lastFreeCash
					END ELSE IF @lastFreeCash < @nRemindCash
					BEGIN
						DELETE FROM tb_Account_cash WHERE szID = @szID AND nIdx = @tmpNextIdx_freeCash AND nCashType = 2
						SET @nRemindCash = @nRemindCash - @lastFreeCash
					END
					
					
					SELECT @nRemindCash AS 'RemindCash' , @tmpPaidCash 'tmpPaidCash'
				END
				
				
				IF  @nRemindCash > 0 AND @tmpPaidCash > @nRemindCash --유료 캐시로 결제 가능 상황 
				BEGIN
					
					SELECT '유료캐시결재하기'
					
					DECLARE @tmpFirstIdx_InPaidCash INT
					DECLARE @tmpFirst_PaidCash INT
					--------------------------------------------------
					--첫번째 nIdx가져와서 왜 가져올라고 했더라
					--첫번째 nIdx에 있는 nCash값으로 해결이 되는지 확인 작으면 업데이트 크면 삭제하는 IF문
					SELECT TOP 1 @tmpFirstIdx_InPaidCash = nIdx,@tmpFirst_PaidCash = nCash FROM tb_Account_cash 
					WHERE nCashType = 1 ORDER BY nIdx ASC --ORDER BY nIdx DESC
					
					--------------------------------------------------
					--SELECT @tmpFirstIdx_InPaidCash AS 'tmpFirstIdx_InPaidCash' , @tmpFirst_PaidCash AS 'tmpFirst_PaidCash'
					
					
					
					IF @tmpIdx = NULL
					BEGIN
						SET @tmpIdx = @tmpFirstIdx_InPaidCash
					END
					
					IF @nRemindCash = @tmpFirst_PaidCash
					BEGIN
						DELETE FROM tb_Account_cash WHERE szID = @szID AND nIdx = @tmpFirstIdx_InPaidCash 
						SET @nRemindCash = 0
					END ELSE IF @nRemindCash < @tmpFirst_PaidCash
					BEGIN
						--업데이트
						SELECT '업데이트 @nRemindCash < @tmpFirst_PaidCash '
						UPDATE tb_Account_cash SET nCash = nCash - @nRemindCash WHERE szID = @szID AND nIdx = @tmpFirstIdx_InPaidCash
						SET @nRemindCash = 0 
					END ELSE IF @nRemindCash > @tmpFirst_PaidCash
					BEGIN -- 첫번째 ROW로 해결이 안될 때
					
					
						--딜리트 하고 리마인드캐시 업데이트하ㄱ
						--리마인드 캐시하기위한 첫번째 인덱스 부터 유료캐시 총합도 필요함
						--마지막 인덱스 더해서 update하면 끝~
						DECLARE @tmpTotal_PaidCash INT
						DECLARE @tmpNextIdx INT
						
						--SELECT @tmpIdx AS 'tmpIdx' , @nRemindCash AS '@nRemindCash' , @tmpTotal_PaidCash AS 'tmpTotal_PaidCash'
						
						SELECT @tmpTotal_PaidCash = ISNULL(sum(nCash),0) FROM tb_Account_cash WHERE nIdx <= @tmpIdx AND nCashType = 1
						
						
						
						--SELECT @tmpTotal_PaidCash AS 'tmpTotal_PaidCash'
						
						IF @tmpTotal_PaidCash > 0
						BEGIN
							SET @nRemindCash = @nRemindCash - @tmpTotal_PaidCash
							--SELECT @nRemindCash AS '@nRemindCash'
						END
						
						DELETE FROM tb_Account_cash WHERE szID = @szID AND nIdx <= @tmpIdx AND  nCashType = 1
						
						--유료캐시값이 있는 인덱스 가져오기
						SELECT @tmpNextIdx = nIdx FROM tb_Account_cash WHERE nIdx > @tmpIdx AND nCashType = 1
						
						UPDATE tb_Account_cash SET nCash = nCash - @nRemindCash WHERE szID = @szID AND nIdx = @tmpNextIdx AND nCashType = 1
						
						
						
						SET @nRemindCash = 0
						--끝
						
						
					END
					----------------------
					------------------------------
					
					------------------------------
					
					
					
					
					
				END ELSE IF @tmpPaidCash = @nRemindCash --같은 금액 깔끔하게 삭제만 하면됨
				BEGIN 
					DELETE FROM tb_Account_cash WHERE szID = @szID AND nIdx <=@tmpIdx AND nCashType = 1
					SELECT '모든 캐시를 소진하였습니다'
				END
			
			END	
			
		END
		ELSE--보유 캐시가 사용 할 캐시보다 부족합니다
		BEGIN
			SELECT '보유 캐시가 부족합니다'
		END
	END
		
	-- 충분하다.
	
	
-------------------------------- proc_cash_use
DECLARE @nRet INT
EXEC proc_cash_use 'ID2',121,@nRet OUTPUT
SELECT @nRet AS '리턴값'

-------------------------------- proc_cash_add
ALTER PROCEDURE proc_cash_add
	@szID varchar(20),
	@cash_type INT,
	@pay_type INT,
	@val INT,
	@nRet INT OUTPUT -- 1:성공 ,0:실패 , -1:type_Error
	
AS
	DECLARE @price_type char(2)
	SET @nRet = 0
	
	DECLARE @lifeTime DATETIME
	SET @lifeTime = GETDATE()
	
	IF @cash_type = 2 --무료 
	BEGIN
		INSERT INTO tb_Account_cash(nCash,szID,nCashType,dtFree_cash_lifeTime) VALUES (@val,@szID,@cash_type,DATEADD(Month,6,@lifeTime))
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
		SELECT @nRet = nCash FROM tb_Account_cash WHERE szID = @szID		
	END
	ELSE
	SET @nRet = -1
	
	--nIdx
	
-------------------------------- proc_cash_add
DECLARE @nRet INT
EXEC proc_cash_add 'ID2',2,0,125,@nRet OUTPUT

DECLARE @nRet INT
EXEC proc_cash_add 'ID2',1,0,1000,@nRet OUTPUT

SELECT @nRet
--UPDATE tb_Account SET cash = 0 WHERE szID = 'ID2'

----------------------------------
---proc_cash_add안에서 쓰는 proc
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
		INSERT INTO tb_Account_cash(nCash,szID,nCashType,dtCash_lifeTime) VALUES (100,@szID,1,DATEADD(Year,5,@lifeTime))
		--SELECT @nRet = cash FROM tb_Account_cash WHERE szID = @szID
		SET @nRet = 1
	END
	ELSE IF @price_type = 'B'
	BEGIN
		INSERT INTO tb_Account_cash(nCash,szID,nCashType,dtCash_lifeTime) VALUES (500,@szID,1,DATEADD(Year,5,@lifeTime))
		--SELECT @nRet = cash FROM tb_Account_cash WHERE szID = @szID
		SET @nRet = 2
	END
	ELSE IF @price_type = 'C'
	BEGIN
		INSERT INTO tb_Account_cash(nCash,szID,nCashType,dtCash_lifeTime) VALUES (1000,@szID,1,DATEADD(Year,5,@lifeTime))
		--SELECT @nRet = cash FROM tb_Account_cash WHERE szID = @szID
		SET @nRet = 3
	END
	ELSE IF @price_type = 'D'
	BEGIN
		INSERT INTO tb_Account_cash(nCash,szID,nCashType,dtCash_lifeTime) VALUES (3000,@szID,1,DATEADD(Year,5,@lifeTime))
		--SELECT @nRet = cash FROM tb_Account_cash WHERE szID = @szID
		SET @nRet = 4
	END ELSE SET @nRet = -1

----------------------------------



----------------------------------------proc_lifeTime_check
ALTER PROCEDURE proc_validTime_check
	@szID varchar(20),
	@nRet INT OUTPUT
AS
	SET @nRet = 0
	DELETE FROM tb_Account_cash WHERE szID = @szID AND (GETDATE() > dtCash_lifeTime OR GETDATE() > dtFree_cash_lifeTime)
	
	
	--현제시각과 lifeTime이 같으면 DELETE해준다~
	--유료 5년 뒤, 무료 6개월뒤
----------------------------------------proc_lifeTime_check
EXEC proc_validTime_check @szID