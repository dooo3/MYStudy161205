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

1. 셀프조인해서 나오는 로우셋중에서. max(nIdx) 이값 구하기. ( delete사용 )
2. 무료캐쉬, 유료캐쉬 몇개 delete될지 확인. ex)유료199개 무료100개 삭제될 예정.

SELECT * FROM tb_Account_cash WHERE szid = 'id2'

