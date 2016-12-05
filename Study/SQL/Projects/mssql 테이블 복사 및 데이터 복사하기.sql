

-- 원본테이블과 똑같은 형태의 테이블을 만들면서 데이터까지 넣기
=> SELECT * INTO 만들테이블명 FROM 원본테이블명

SELECT * INTO tb_test_user 
FROM [mobile_test].[dbo].[3YEAR]
WHERE NUM_NID = NULL AND VC_UID = NULL 

drop table tb_test_user

select * from tb_test_user

/*
--임시테이블 만들때 
--특정 열
SELECT 특정열 into tb_test_user
FROM [mobile_test].[dbo].[3YEAR]
WHERE 특정열 in ( 조건 )


--데이터는 없는 테이블 ( 구조만 복사 )
SELECT * into tb_test_user
FROM [mobile_test].[dbo].[3YEAR]
WHERE 1 = 2

--테이블을 생성하면서 데이터도 같이 복사

SELECT * INTO[새로운 테이블명] FROM [기존 테이블명]

--구조만 복사
SELECT*INTO [새로운 테이블명 FROM[기존 테이블명] WHERE 1=2

--테이블에 있는 데이터만 복사
INSERT INTO [복사될 테이블명] SELECT * FROM [기존 테이블명]

--특정 데이터만 복사
INSERT INTO [복사될 테이블명]SELECT * FROM [기존 테이블명] WHERE 조건
*/