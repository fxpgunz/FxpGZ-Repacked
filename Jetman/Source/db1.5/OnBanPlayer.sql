USE [GunzDBTest]
GO

/****** Object:  StoredProcedure [dbo].[spOnBanPlayer]    Script Date: 07/29/2018 21:36:48 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO


CREATE  PROC [dbo].[spOnBanPlayer]
 @AID int,
 @HackingType  int,      
 @BannedReason varchar(20)
AS    
BEGIN  
 SET NOCOUNT ON      

 UPDATE Account SET HackingType = @HackingType, BannedReason = @BannedReason , IsBanned = 1 WHERE AID = @AID  
END
GO


