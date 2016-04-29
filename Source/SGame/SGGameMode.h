// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"
#include "Messaging.h"

#include "SGTileBase.h"
#include "SGameMessages.h"
#include "SGLinkLine.h"
#include "SGGrid.h"
#include "SGTileManager.h"

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
	void SetNextStatus(ESGGameStatus::Type inNewStatus);

	/** Initialize the tiles on the grid*/
	UFUNCTION(BlueprintCallable, Category = Game)
	ESGGameStatus::Type GetCurrentGameStatus();

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
	void OnPlayerInput();

	/** Return the tile can link to the linkline */
	UFUNCTION(BlueprintCallable, Category = LinkLine)
	bool CanLinkToLastTile(const ASGTileBase* inCurrentTile);

	/** Override the parent tick to do some customized tick operations*/
	virtual void Tick(float DeltaSeconds) override;

	ASGGrid* GetCurrentGrid() const { return CurrentGrid; }
	void SetCurrentGrid(ASGGrid* val) { checkSlow(val != nullptr);  CurrentGrid = val; }

	void AddTiles(ASGTileBase* TileToAdded)
	{
		AllTiles.AddUnique(TileToAdded);
	}

protected:
	/** Contains all the tiles in the game, including the disappering tiles */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ASGTileBase*> AllTiles;

private:
	/** Handles Game start messages. */
	void HandleGameStart(const FMessage_Gameplay_GameStart& Message, const IMessageContextRef& Context);

	/** Handles the game status update messages. */
	void HandleGameStatusUpdate(const FMessage_Gameplay_GameStatusUpdate& Message, const IMessageContextRef& Context);

	/** Handles the player picked new tile*/
	void HandleNewTileIsPicked(const FMessage_Gameplay_NewTilePicked& Message, const IMessageContextRef& Context);

	/** Handle collect the link line*/
	void HandleCollectLinkLine(const FMessage_Gameplay_CollectLinkLine& Message, const IMessageContextRef& Context);

	ESGGameStatus::Type CurrentGameGameStatus;
	ESGGameStatus::Type NextGameStatus;

	// Holds the messaging endpoint.
	FMessageEndpointPtr MessageEndpoint;

	/** Current round number*/
	uint32				CurrentRound;

	/** Current link line */
	ASGLinkLine*		CurrentLinkLine;

	/** Current grid */
	ASGGrid*			CurrentGrid;

	void UpdateTileSelectState();
	void ResetTileSelectInfo();

	void UpdateTileLinkState();
	void ResetTileLinkInfo();
};
