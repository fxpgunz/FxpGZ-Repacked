USE [GunzDB15]
set ANSI_NULLS ON
set QUOTED_IDENTIFIER ON
GO
--ALTER PROC [dbo].[spChangeGambleItemToRewardNormalItem]  
CREATE PROC dbo.spChangeGambleItemToRewardNormalItem  
	  @CID				INT
	, @CIID				INT
	, @GIID				INT
	, @GRIID			INT
	, @RewardItemID		INT
	, @RewardRentPeriod	INT


AS BEGIN

	SET NOCOUNT ON;

	DECLARE @RentHourPeriod		INT;
	DECLARE @GambleItemCount	INT;
	
	-----------------------------------------------------------------------------
			
	SELECT	@RentHourPeriod = ISNULL(RentHourPeriod, 0)
	FROM	GambleRewardItem gri(NOLOCK)  
	WHERE	GRIID = @GRIID
	AND		GIID = @GIID 
	AND		(ItemIDMale = @RewardItemID OR ItemIDFemale = @RewardItemID)
	
	IF (0 = @@ROWCOUNT) BEGIN
		SELECT -1 AS 'Ret'  
		RETURN
	END
			
	SELECT	@GambleItemCount = ISNULL(Cnt, 1)
	FROM	CharacterItem(NOLOCK)
	WHERE	CIID = @CIID;
		
	-----------------------------------------------------------------------------
	
	DECLARE @NowDate	DATETIME;
	SET @NowDate = GETDATE();	
	
	-----------------------------------------------------------------------------
			
	BEGIN TRAN -----------  
	
		----------------------------------------------------------------------------------------
		
		IF( @GambleItemCount > 1 ) BEGIN
		
			UPDATE	CharacterItem 
			SET		Cnt = Cnt - 1
			WHERE	CID = @CID 
			AND		CIID = @CIID 
			
			IF (0 <> @@ERROR) OR (0 = @@ROWCOUNT) BEGIN  
				ROLLBACK TRAN  
				SELECT -2 AS 'Ret'  
				RETURN
			END
			
		END
		ELSE BEGIN
		
			UPDATE	CharacterItem 
			SET		CID = NULL
			WHERE	CID = @CID 
			AND		CIID = @CIID
			
			IF (0 <> @@ERROR) OR (0 = @@ROWCOUNT) BEGIN  
				ROLLBACK TRAN  
				SELECT -3 AS 'Ret'  
				RETURN
			END
			
		END
		
		---------------------------------------------------------------------------------------	
		
		INSERT CharacterItem(CID, ItemID, RegDate, RentDate, RentHourPeriod, Cnt)
		VALUES (@CID, @RewardItemID, @NowDate, @NowDate, @RentHourPeriod, 1)
		
		DECLARE @OrderCIID INT;			
		SET @OrderCIID = @@IDENTITY;
			
		IF (0 <> @@ERROR) BEGIN  
			ROLLBACK TRAN  
			SELECT -4 AS 'Ret'  
			RETURN
		END
			
			
		INSERT dbo.ItemChangeLog_CharacterItem(ChangeType, ChangeDate, CID, CIID, ItemID, [Count])
		VALUES (202, @NowDate, @CID, @CIID, @GIID, 1);	
		
		IF (0 <> @@ERROR) BEGIN  
			ROLLBACK TRAN  
			SELECT -5 AS 'Ret'  
			RETURN
		END

		INSERT dbo.ItemChangeLog_CharacterItem(ChangeType, ChangeDate, CID, CIID, ItemID, [Count])
		VALUES (102, @NowDate, @CID, @OrderCIID, @RewardItemID, 1);
		
		IF (0 <> @@ERROR) BEGIN  
			ROLLBACK TRAN  
			SELECT -6 AS 'Ret'  
			RETURN
		END

		INSERT INTO LogDB..GambleLog(CID, GIID, RewardItemID, RegDate)  
		VALUES (@CID, @GIID, @RewardItemID, @NowDate);
		
		IF (0 <> @@ERROR) BEGIN  
			ROLLBACK TRAN  
			SELECT -7 AS 'Ret'  
			RETURN
		END
		
		----------------------------------------------------------------------------------------

	COMMIT TRAN  -----------
		
	SELECT 0 AS 'Ret', @OrderCIID AS 'ORDERCIID'
END