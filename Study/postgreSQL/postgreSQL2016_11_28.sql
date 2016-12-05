-- 프로시저
-- 인풋,아웃풋
-- 로우셋
--GREATEST LEAST  -- MAX MIN

CREATE OR REPLACE FUNCTION infoTotal() 
RETURNS integer AS $total$

DECLARE total integer;
	
BEGIN
	SELECT count(*)into total FROM test_table;
	RETURN total; 
END;
$total$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION Func_lowSet()
RETURN void AS $$
BEGIN
	SELECT code FROM test_table;
END



CREATE TABLE test_table(
	code char(5),
	total integer
);

INSERT INTO test_table VALUES('abcdd',10);

SELECT infoTotal();