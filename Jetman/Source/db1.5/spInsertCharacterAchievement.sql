USE [GunzDB15]

GO

CREATE PROCEDURE [dbo].[spInsertAchievement]
@CID int,
@AchievementID int,
@AchievementType tinyint

AS
	BEGIN
	
		SET NOCOUNT ON
		
		INSERT INTO dbo.CharacterAchievements(CID,AchievementID,AchievementType) 
		VALUES(@CID,@AchievementID,@AchievementType) 
	END
GO