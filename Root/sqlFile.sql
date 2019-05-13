IF NOT EXISTS (SELECT 1 FROM sys.databases WHERE NAME = 'IseProject')
	CREATE DATABASE IseProject
GO

USE IseProject


/*******************************************************************************************
	Domains
*******************************************************************************************/
IF TYPE_ID(N'NAME') IS NULL
	CREATE TYPE NAME FROM VARCHAR(12) NOT NULL
GO



/*******************************************************************************************
	Clean up tables
*******************************************************************************************/

DROP TABLE IF EXISTS Pool
GO

DROP TABLE IF EXISTS BPMN
GO

/*******************************************************************************************
	Tables
*******************************************************************************************/

CREATE TABLE BPMN
(
	BPMNId INT NOT NULL,
	name NAME NOT NULL,

	CONSTRAINT pk_BPMN PRIMARY KEY (BPMNId)
)
GO

CREATE TABLE Pool
(
	poolId INT NOT NULL,
	name NAME NOT NULL,
	BPMNId INT NOT NULL,

	CONSTRAINT fk_Pool_BPMN_BPMNID FOREIGN KEY (BPMNId) references BPMN(BPMNId),

	CONSTRAINT pk_Pool PRIMARY KEY (PoolId),
)
GO
USE IseProject
GO


/*******************************************************************************************
	C-1:
	Binnen een model moeten pools een unieke naam hebben.

	Can go wrong when:
	A Pool is moved to a different BPMN where a pool with the same name already exists
	A Pool is inserted when there is a pool with the same name
	A Pool is updated where its name is changed to a name that already exists
*******************************************************************************************/
CREATE OR ALTER TRIGGER utr_poolUniquePoolNameInBPMN
	ON Pool
	AFTER UPDATE, INSERT
AS
BEGIN
	BEGIN TRY
		IF EXISTS (
			SELECT 1
				FROM Pool 
				GROUP BY BPMNId, name
				HAVING COUNT(*) > 1
			)
			THROW 50101, 'A pool name must be unique within a BPMN', 1
	END TRY
	BEGIN CATCH
		THROW
	END CATCH
END
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

