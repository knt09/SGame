// Fill out your copyright notice in the Description page of Project Settings.

#include "SGame.h"
#include "SGTileBase.h"
#include "SGGrid.h"
#include "SGGameMode.h"

// Sets default values
ASGTileBase::ASGTileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// We want the tile can be moved (falling), so we need a root component
	SetRootComponent(GetRenderComponent());
}

// Called when the game starts or when spawned
void ASGTileBase::BeginPlay()
{
	Super::BeginPlay();

	// Set our class up to handle clicks and touches.
	OnClicked.AddUniqueDynamic(this, &ASGTileBase::TilePress_Mouse);
	OnBeginCursorOver.AddUniqueDynamic(this, &ASGTileBase::TileEnter_Mouse);
	OnReleased.AddUniqueDynamic(this, &ASGTileBase::TileRelease_Mouse);

	OnInputTouchBegin.AddUniqueDynamic(this, &ASGTileBase::TilePress);
	OnInputTouchEnter.AddUniqueDynamic(this, &ASGTileBase::TileEnter);
	OnInputTouchEnd.AddUniqueDynamic(this, &ASGTileBase::TileRelease);

	FString EndPointName = FString::Printf(TEXT("Gameplay_Tile_%d"), GridAddress);
	MessageEndpoint = FMessageEndpoint::Builder(*EndPointName)
		.Handling<FMessage_Gameplay_TileSelectableStatusChange>(this, &ASGTileBase::HandleSelectableStatusChange)
		.Handling<FMessage_Gameplay_TileLinkedStatusChange>(this, &ASGTileBase::HandleLinkStatusChange)
		.Handling<FMessage_Gameplay_TileBeginMove>(this, &ASGTileBase::HandleTileMove)
		.Handling<FMessage_Gameplay_TileCollect>(this, &ASGTileBase::HandleTileCollected)
		.Handling<FMessage_Gameplay_DamageToTile>(this, &ASGTileBase::HandleTakeDamage);

	if (MessageEndpoint.IsValid() == true)
	{
		// Subscribe the tile need events
		MessageEndpoint->Subscribe<FMessage_Gameplay_TileSelectableStatusChange>();
		MessageEndpoint->Subscribe<FMessage_Gameplay_TileLinkedStatusChange>();
		MessageEndpoint->Subscribe<FMessage_Gameplay_TileBeginMove>();
		MessageEndpoint->Subscribe<FMessage_Gameplay_TileCollect>();
		MessageEndpoint->Subscribe<FMessage_Gameplay_DamageToTile>();
	}

	Grid = Cast<ASGGrid>(GetOwner());
}

// Called every frame
void ASGTileBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// TickFalling(DeltaTime);
}

void ASGTileBase::TilePress(ETouchIndex::Type FingerIndex, AActor* TouchedActor)
{
	UE_LOG(LogSGameTile, Log, TEXT("Tile %s was pressed, address (%d,%d)"), *GetName(), GridAddress % 6, GridAddress / 6);

	// Tell the game logic, the new tile is picked
	FMessage_Gameplay_NewTilePicked* TilePickedMessage = new FMessage_Gameplay_NewTilePicked();
	TilePickedMessage->TileID = TileID;
	if (MessageEndpoint.IsValid() == true)
	{
		MessageEndpoint->Publish(TilePickedMessage, EMessageScope::Process);
	}
}

void ASGTileBase::TileEnter(ETouchIndex::Type FingerIndex, AActor* TouchedActor)
{
	UE_LOG(LogSGameTile, Log, TEXT("Tile %s was entered, address (%d,%d)"), *GetName(), GridAddress % 6, GridAddress / 6);
	FMessage_Gameplay_NewTilePicked* TilePickedMessage = new FMessage_Gameplay_NewTilePicked();
	TilePickedMessage->TileID = TileID;
	if (MessageEndpoint.IsValid() == true)
	{
		MessageEndpoint->Publish(TilePickedMessage, EMessageScope::Process);
	}
}

void ASGTileBase::TileRelease(ETouchIndex::Type FingerIndex, AActor* TouchedActor)
{
	FMessage_Gameplay_GameStatusUpdate* GameStatusUpdateMessage = new FMessage_Gameplay_GameStatusUpdate();
	GameStatusUpdateMessage->NewGameStatus = ESGGameStatus::EGS_PlayerEndBuildPath;

	if (MessageEndpoint.IsValid() == true)
	{
		MessageEndpoint->Publish(GameStatusUpdateMessage, EMessageScope::Process);
	}
}

void ASGTileBase::TilePress_Mouse(AActor* TouchedActor, FKey ButtonPressed)
{
	TilePress(ETouchIndex::Touch1, TouchedActor);
}

void ASGTileBase::TileEnter_Mouse(AActor* TouchedActor)
{
	// This is meant to simulate finger-swiping, so ignore if the mouse isn't clicked.
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (PC->IsInputKeyDown(EKeys::LeftMouseButton))
		{
			TileEnter(ETouchIndex::Touch1, TouchedActor);
		}
	}
}

void ASGTileBase::TileRelease_Mouse(AActor* TouchedActor, FKey ButtonPressed)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		TileRelease(ETouchIndex::Touch1, TouchedActor);
	}
}

bool ASGTileBase::IsSelectable() const
{
	return Data.TileStatusArray.Contains(ESGTileStatusFlag::ESF_SELECTABLE);
}

void ASGTileBase::SetGridAddress(int32 NewLocation)
{
	GridAddress = NewLocation;
}

int32 ASGTileBase::GetGridAddress() const
{
	return GridAddress;
}

void ASGTileBase::OnTileCollected()
{

}

void ASGTileBase::OnTileTakeDamage()
{

}

TArray<FTileResourceUnit> ASGTileBase::GetTileResource() const
{
	return Data.TileResourceArray;
}

void ASGTileBase::HandleTileCollected(const FMessage_Gameplay_TileCollect& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	FILTER_MESSAGE;

	// Do some collect animation

	// After all, tell the game mode to remove it
	check(Grid && Grid->GetTileManager());
	if (Grid->GetTileManager()->DestroyTileWithID(TileID) == false)
	{
		UE_LOG(LogSGameTile, Warning, TEXT("Tile delete failed!"));
	}
}

bool ASGTileBase::OnTakeTileDamage(const TArray<FTileDamageInfo>& DamageInfos, FTileLifeArmorInfo& LifeArmorInfo) const
{
	for (int i = 0; i < DamageInfos.Num(); i++)
	{
		// Calculate the piercing damage first
		LifeArmorInfo.CurrentLife -= DamageInfos[i].InitialDamage * DamageInfos[i].PiercingArmorRatio;
		if (LifeArmorInfo.CurrentLife < 0)
		{
			return true;
		}

		// Reduce the tile armor value
		float ResultDamage = DamageInfos[i].InitialDamage * (1 - DamageInfos[i].PiercingArmorRatio);

		// Currently the tile armor duracity is fix to 1 (1 armor absorb = 1 damage)
		if (LifeArmorInfo.CurrentArmor > 0)
		{
			float ArmorBefore = LifeArmorInfo.CurrentArmor;
			float ArmorAfter = ArmorBefore - ResultDamage;
			LifeArmorInfo.CurrentArmor = FMath::Clamp(ArmorAfter, 0.0f, LifeArmorInfo.ArmorMax);
			
			ResultDamage -= ArmorBefore;
			if (ResultDamage < 0)
			{
				ResultDamage = 0;
			}
		}

		// The damage don't absorb completely 
		if (ResultDamage > 0)
		{
			LifeArmorInfo.CurrentLife -= ResultDamage;
			if (LifeArmorInfo.CurrentLife < 0)
			{
				return true;
			}
		}
	}

	return false;
}

bool ASGTileBase::EvaluateDamageToTile(const TArray<FTileDamageInfo>& DamageInfos) const
{
	FTileLifeArmorInfo FakeInfo = Data.LifeArmorInfo;
	return OnTakeTileDamage(DamageInfos, FakeInfo);
}

void ASGTileBase::OnTweenCompleteNative(AiTweenEvent* eventOperator, AActor* actorTweening, USceneComponent* componentTweening, UWidget* widgetTweening, FName tweenName, FHitResult sweepHitResultForMoveEvents, bool successfulTransform)
{
	if (tweenName == TEXT("Falling"))
	{
		UE_LOG(LogSGame, Log, TEXT("Tile %d finished moved to the new address %d"), TileID, GridAddress);
		FinishFalling();
	}
}

void ASGTileBase::HandleTakeDamage(const FMessage_Gameplay_DamageToTile& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	FILTER_MESSAGE;

	if (Abilities.bCanTakeDamage == false)
	{
		UE_LOG(LogSGameTile, Log, TEXT("Tile cannnot take damage"));
		return;
	}

	ASGGameMode* GameMode = Cast<ASGGameMode>(UGameplayStatics::GetGameMode(this));
	checkSlow(GameMode);

	if (GameMode->ShouldReplayLinkAnimation() == true)
	{
		// Take damage will delay to replay link animation
		CachedDamageMessage = Message;
	}
	else
	{
		// Take damage first
		bool TileDead = OnTakeTileDamage(Message.DamageInfos, Data.LifeArmorInfo);
	}
}

void ASGTileBase::HandleSelectableStatusChange(const FMessage_Gameplay_TileSelectableStatusChange& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	FILTER_MESSAGE;

	UE_LOG(LogSGameTile, Log, TEXT("Tile %d selectable flag changed to %d"), GridAddress, Message.NewSelectableStatus);

	if (Message.NewSelectableStatus == true)
	{
		// Add the selectable flag to the status array
		Data.TileStatusArray.AddUnique(ESGTileStatusFlag::ESF_SELECTABLE);

		// Set the white color 
		GetRenderComponent()->SetSpriteColor(FLinearColor::White);
	}
	else
	{
		// Remove the selectable flag
		Data.TileStatusArray.Remove(ESGTileStatusFlag::ESF_SELECTABLE);

		// Dim the sprite
		GetRenderComponent()->SetSpriteColor(FLinearColor(0.2f, 0.2f, 0.2f));
	}
}

void ASGTileBase::HandleLinkStatusChange(const FMessage_Gameplay_TileLinkedStatusChange& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	FILTER_MESSAGE;

	UE_LOG(LogSGameTile, Log, TEXT("Tile %d link status changed to %d"), GridAddress, Message.NewLinkStatus);

	if (Message.NewLinkStatus == true)
	{
		// Add the selectable flag to the status array
		Data.TileStatusArray.AddUnique(ESGTileStatusFlag::ESF_LINKED);

		// Set the linked sprite
		GetRenderComponent()->SetSprite(Sprite_Selected);
	}
	else
	{
		// Remove the selectable flag
		Data.TileStatusArray.Remove(ESGTileStatusFlag::ESF_LINKED);

		// Set the normal sprite
		GetRenderComponent()->SetSprite(Sprite_Normal);
	}
}

void ASGTileBase::HandleTileMove(const FMessage_Gameplay_TileBeginMove& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	FILTER_MESSAGE;

	UE_LOG(LogSGameTile, Log, TEXT("Tile ID: %d, at old address: %d will move to the new address %d"), Message.TileID, Message.OldTileAddress, Message.NewTileAddress);

	if (Grid == nullptr)
	{
		Grid = Cast<ASGGrid>(GetOwner());
	}
	check(Grid);
	FallingEndLocation = Grid->GetLocationFromGridAddress(Message.NewTileAddress);
	FallingStartLocation = GetActorLocation();

	// Set the new grid address
	SetGridAddress(Message.NewTileAddress);

	// Start the falling
	StartFalling();
}

void ASGTileBase::FinishFalling()
{
	SetActorLocation(FallingEndLocation);
	if (MessageEndpoint.IsValid() == true)
	{
		// Send the finish move message to other module
		FMessage_Gameplay_TileEndMove* Message = new FMessage_Gameplay_TileEndMove();
		Message->TileID = TileID;
		MessageEndpoint->Publish(Message, EMessageScope::Process);
	}
}

