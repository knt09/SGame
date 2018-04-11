// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"
#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "SGTileBase.h"
#include "SGameMessages.h"
#include "SGLinkLine.h"
#include "SGGrid.h"
#include "SGSpritePawn.h"
#include "SGPlayerSkillManager.h"

#include "SGGameMode.generated.h"

/**
 * The Gameplay mode
 */
UCLASS()
class SGAME_API ASGGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASGGameMode(const FObjectInitializer& ObjectInitializer);

	/** Called when the game starts. */
	virtual void BeginPlay() override;

	/** Initialize the tiles on the grid*/
	UFUNCTION(BlueprintCallable, Category = Game)
	ESGGameStatus GetCurrentGameStatus();

	/** Begin the new round */
	UFUNCTION(BlueprintCallable, Category = Game)
	void OnBeginRound();

	/** Player turn begin */
	UFUNCTION(BlueprintCallable, Category = Game)
	void OnPlayerTurnBegin();

	/** Player regenerate */
	UFUNCTION(BlueprintCallable, Category = Game)
	void OnPlayerRegenerate();

	/** Player skil CD */
	UFUNCTION(BlueprintCallable, Category = Game)
	void OnPlayerSkillCD();

	/** Player input */
	UFUNCTION(BlueprintCallable, Category = Game)
	void OnPlayerBeginInputStage();

	/** Player end build path */
	UFUNCTION(BlueprintCallable, Category = Game)
	void OnPlayerEndBuildPathStage();
	FTimerHandle PlayerEndInputTimer;
	void TimerPlayerEndInput();

	/** Player end input */
	UFUNCTION(BlueprintCallable, Category = Game)
	void OnPlayerEndInputStage();

	/** Enemy attack stage*/
	void OnEnemyAttackStage();

	/** Called when this round end*/
	void OnRoundEndStage();

	/** Called when game over */
	UFUNCTION(BlueprintCallable, Category = Game)
	void OnGameOver();

	/** Create player skill */
	UFUNCTION(BlueprintCallable, Category = Game)
	ASGSkillBase* CreatePlayerSkilkByName(FString inSkillName);

	/** Check if game over */
	bool CheckGameOver();

	/** Override the parent tick to do some customized tick operations*/
	virtual void Tick(float DeltaSeconds) override;

	ASGGrid* GetCurrentGrid() const { return CurrentGrid; }
	void SetCurrentGrid(ASGGrid* val) { checkSlow(val != nullptr);  CurrentGrid = val; }

	int32 GetCurrentRound() const { return CurrentRound; }

	UFUNCTION(BlueprintCallable, Category = Game)
	bool IsLinkLineValid();

	bool ShouldReplayLinkAnimation() const { return bShouldReplayLinkAnimation; }

	/** Tell wheter can link to test tile */
	UFUNCTION(BlueprintCallable, Category = Tile)
	bool CanLinkToLastTile(const ASGTileBase* inTestTile);

	/** Collect a array of tiles*/
	UFUNCTION(BlueprintCallable, Category = Tile)
	bool CollectTileArray(TArray<ASGTileBase*> inTileArrayToCollect);

	/**
	* Calculate the enemy damage
	*
	* @param outDamageCanBeShield damage can be shielded
	* @param outDamageDirectToHP damage direct to the hp
	*
	* @return true means there is enemy who will attack the player
	*/
	UFUNCTION(BlueprintCallable, Category = Attack)
	bool CalculateEnemyDamageToPlayer(float& outDamageCanBeShield, float& outDamageDirectToHP);

	/**
	* Calculate the linkline resources and the damage
	*/
	UFUNCTION(BlueprintCallable, Category = LinkLine)
	void CalculateLinkLine();

protected:

	/** The minum lenth require for on valid link line*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Game)
	int32 MinimunLengthLinkLineRequired;

	/** Whether to replay the link animation*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	bool bShouldReplayLinkAnimation;

	/** Whether to replay the link animation*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	USGPlayerSkillManager* PlayerSkillManager;

	/** Current grid */
	UPROPERTY(BlueprintReadOnly, Category = Game)
	ASGGrid*			CurrentGrid;

	/**
	* Calculate the linkline damage
	*/
	TArray<FTileDamageInfo> CaculateLinkLineDamage(TArray<ASGTileBase*>& CauseDamageTiles);
private:
	/** Handles Game start messages. */
	void HandleGameStart(const FMessage_Gameplay_GameStart& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context);

	/** Handles the game status update messages. */
	void HandleGameStatusUpdate(const FMessage_Gameplay_GameStatusUpdate& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context);

	/** Handle all tile has finish moving message, push the game procesdure to next stage */
	void HandleAllTileFinishMoving(const FMessage_Gameplay_AllTileFinishMove& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context);

	/** Handle begin attack event*/
	void HandleBeginAttack(const FMessage_Gameplay_EnemyBeginAttack& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context);

	/** Handle collect the link line*/
	void HandleCollectLinkLine(const FMessage_Gameplay_CollectLinkLine& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context);

	/** Handles the player picked new tile*/
	void HandleNewTileIsPicked(const FMessage_Gameplay_NewTilePicked& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context);

	/** Current game status for this mode*/
	ESGGameStatus CurrentGameGameStatus;

	// Holds the messaging endpoint.
	TSharedPtr<FMessageEndpoint, ESPMode::ThreadSafe> MessageEndpoint;

	/** Current round number*/
	int32				CurrentRound;

	/** Current link line */
	ASGLinkLine*		CurrentLinkLine;

	/** Current player pawn (master) */
	ASGSpritePawn*		CurrentPlayerPawn;
};
