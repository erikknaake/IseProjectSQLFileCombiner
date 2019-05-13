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
