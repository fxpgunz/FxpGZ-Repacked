USE [GunzDB15]
GO

CREATE TABLE [dbo].[SkillMapTimes]
(	
	[MapID][tinyint] NOT NULL,
	[CID][int] NOT NULL,
	[BestTime][INT] NOT NULL
) ON [PRIMARY]
GO

CREATE PROCEDURE [dbo].[spInsertSkillMapTime]
@MapID tinyint,
@CID int,
@BestTime INT  /*todo*/
AS BEGIN
	SET NOCOUNT ON;

	UPDATE dbo.SkillMapTimes SET BestTime = @BestTime WHERE MapID = @MAPID AND CID = @CID
	IF @@ROWCOUNT = 0
	BEGIN
		INSERT INTO dbo.SkillMapTimes(MapID,CID,BestTime) VALUES(@MapID,@CID,@BestTime)
	END
END
	
GO

CREATE PROCEDURE [dbo].[spGetSkillMapBestTime]
@MapID tinyint,
@CID int

	AS BEGIN
		SET NOCOUNT ON;
		
		SELECT BestTime FROM dbo.SkillMapTimes WHERE CID = @CID AND MapID = @MapID
	END
GO