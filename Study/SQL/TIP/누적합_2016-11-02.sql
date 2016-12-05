

--누적합 응용--

DECLARE @nDelMaxIdx INT
DECLARE @nPaidCash INT
DECLARE @nFreeCash INT


SELECT  @nDelMaxIdx = max(nIdx), 
		@nPaidCash = ISNULL(SUM(CASE WHEN nCashType = 1 THEN nCash ELSE 0 END),0) , 
		@nFreeCash = ISNULL(SUM(CASE WHEN nCashType = 2 THEN nCash ELSE 0 END),0) 
		/*
		CASE 
			WHEN nCashType = 1 THEN @nPaidCash = @nPaidCash + nCash
			WHEN nCashType = 2 THEN @nFreeCash = @nFreeCash + nCash  
		END --어떻게 하면 합을 넣을 수 있을까 CASE문으로
		*/
FROM 
(
	SELECT X.nIdx, X.nCashType ,X.nCash--, sum(Y.nCash)
	FROM tb_Account_cash X INNER JOIN tb_Account_cash Y 
	ON   X.nIdx >= Y.nIdx									--누적합은 인덱스가 꼭 있어야 한다.
	WHERE X.szID = 'ID2' AND Y.szID = 'ID2'
	GROUP BY X.nIdx, X.nCashType ,X.nCash
	HAVING sum(Y.nCash) <= 0

)AS A


-- 조건 : 무료캐쉬먼저 사용해야한다
-- 누적합 : 



SELECT @nDelMaxIdx AS 'nDelMaxIdx' ,@nPaidCash AS 'nPaidCash', @nFreeCash AS 'nFreeCash'


SELECT * FROM tb_Account_cash WHERE szid = 'id2'
