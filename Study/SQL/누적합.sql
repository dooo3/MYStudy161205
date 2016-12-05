

--������ ����--

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
		END --��� �ϸ� ���� ���� �� ������ CASE������
		*/
FROM 
(
	SELECT X.nIdx, X.nCashType ,X.nCash--, sum(Y.nCash)
	FROM tb_Account_cash X INNER JOIN tb_Account_cash Y 
	ON   X.nIdx >= Y.nIdx									--�������� �ε����� �� �־�� �Ѵ�.
	WHERE X.szID = 'ID2' AND Y.szID = 'ID2'
	GROUP BY X.nIdx, X.nCashType ,X.nCash
	HAVING sum(Y.nCash) <= 0

)AS A


-- ���� : ����ĳ������ ����ؾ��Ѵ�
-- ������ : 



SELECT @nDelMaxIdx AS 'nDelMaxIdx' ,@nPaidCash AS 'nPaidCash', @nFreeCash AS 'nFreeCash'


SELECT * FROM tb_Account_cash WHERE szid = 'id2'
