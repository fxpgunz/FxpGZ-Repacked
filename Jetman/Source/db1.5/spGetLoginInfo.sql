USE [GunzDBTest]
GO

/****** Object:  StoredProcedure [dbo].[spGetLoginInfo]    Script Date: 08/01/2018 21:33:40 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

/* LoginInfo ¾ò¾î¿È */
CREATE PROC [dbo].[spGetLoginInfo]
	@UserID		varchar(32)
AS
SET NOCOUNT ON
SELECT AID, UserID, [Password],[Salt] FROM [Login](nolock) WHERE UserID = @UserID

GO


