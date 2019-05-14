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
