
/****** Object:  StoredProcedure [dbo].[spOnBanPlayer]    Script Date: 07/29/2018 21:36:48 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[spGetAchievementsByCID]
@CID int
AS
	BEGIN
	
		SET NOCOUNT ON
		
		SELECT * FROM [dbo].[CharacterAchievements] WHERE CID = @CID
END


GO