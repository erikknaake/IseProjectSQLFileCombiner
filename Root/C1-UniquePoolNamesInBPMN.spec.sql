USE IseProject
GO


EXEC tSQLt.NewTestClass 'C1_UniquePoolNamesInBPMN'

GO
CREATE OR ALTER PROC C1_UniquePoolNamesInBPMN.SetUp
AS
BEGIN
	EXEC tSQLt.FakeTable 'dbo.BPMN'
	EXEC tSQLt.FakeTable 'dbo.Pool'

	EXEC tSQLt.ApplyTrigger 'dbo.Pool', 'utr_poolUniquePoolNameInBPMN'

	INSERT INTO BPMN VALUES (1, 'A test BPMN')
	SELECT *
		INTO expected
		FROM Pool
END
GO

GO
CREATE OR ALTER PROC C1_UniquePoolNamesInBPMN.testTwoDifferentNamesInSameBPMN
AS
BEGIN
	INSERT INTO expected VALUES (1, 'Sales', 1), (2, 'Finance', 1)
	EXEC tSQLt.ExpectNoException

	INSERT INTO Pool VALUES (1, 'Sales', 1), (2, 'Finance', 1)

	EXEC tSQLt.AssertEqualsTable  'expected', 'Pool'
END
GO

GO
CREATE OR ALTER PROC C1_UniquePoolNamesInBPMN.testTwoEqualNamesInDifferentBPMN
AS
BEGIN
	INSERT INTO expected VALUES (1, 'Sales', 1), (2, 'Sales', 2)
	EXEC tSQLt.ExpectNoException

	INSERT INTO Pool VALUES (1, 'Sales', 1), (2, 'Sales', 2)

	EXEC tSQLt.AssertEqualsTable  'expected', 'Pool'
END
GO

GO
CREATE OR ALTER PROC C1_UniquePoolNamesInBPMN.testTwoEqualNamesInSameBPMN
AS
BEGIN
	INSERT INTO expected VALUES (1, 'Sales', 1)
	EXEC tSQLt.ExpectException @ExpectedErrorNumber = 50101

	INSERT INTO Pool VALUES (1, 'Sales', 1), (2, 'Sales', 1)

	EXEC tSQLt.AssertEqualsTable  'expected', 'Pool'
END
GO

EXEC tSQLt.Run 'C1_UniquePoolNamesInBPMN'
