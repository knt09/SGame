// Fill out your copyright notice in the Description page of Project Settings.

#include "SGame.h"
#include "SGSpritePawn.h"
#include "PaperSprite.h"

// Sets default values
ASGSpritePawn::ASGSpritePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RenderComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PlayerPawnSprite"));
	RenderComponent->Mobility = EComponentMobility::Movable;
	RenderComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	HPMax = 100;
	ArmorMax = 0;
}

// Called when the game starts or when spawned
void ASGSpritePawn::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = HPMax;
	CurrentArmor = ArmorMax;
	
	MessageEndpoint = FMessageEndpoint::Builder("Gameplay_PlayerPawn")
		.Handling<FMessage_Gameplay_PlayerTakeDamage>(this, &ASGSpritePawn::HandlePlayerTakeDamage)
		.Handling<FMessage_Gameplay_ResourceCollect>(this, &ASGSpritePawn::HandleCollectResouce);
	if (MessageEndpoint.IsValid() == true)
	{
		// Subscribe the grid needed messages
		MessageEndpoint->Subscribe<FMessage_Gameplay_PlayerTakeDamage>();
		MessageEndpoint->Subscribe<FMessage_Gameplay_ResourceCollect>();
	}
}

// Called every frame
void ASGSpritePawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ASGSpritePawn::HandlePlayerTakeDamage(const FMessage_Gameplay_PlayerTakeDamage& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	// todo: Add armor damage calculation
	CurrentHP = CurrentHP - Message.DirectDamage;
	SetCurrentHealth(CurrentHP);

	OnPlayHitAniamtion();
}

void ASGSpritePawn::HandleCollectResouce(const FMessage_Gameplay_ResourceCollect& Message, const TSharedRef<IMessageContext, ESPMode::ThreadSafe>& Context)
{
	CurrentHP += Message.SummupResouces[static_cast<int32>(ESGResourceType::ETR_HP)];
	FMath::Clamp(CurrentHP, 0, HPMax);
	SetCurrentHealth(CurrentHP);

	CurrentArmor += Message.SummupResouces[static_cast<int32>(ESGResourceType::ETT_Armor)];
	FMath::Clamp(CurrentArmor, 0, ArmorMax);
}

#if WITH_EDITOR

bool ASGSpritePawn::GetReferencedContentObjects(TArray<UObject*>& Objects) const
{
	Super::GetReferencedContentObjects(Objects);

	if (UPaperSprite* SourceSprite = RenderComponent->GetSprite())
	{
		Objects.Add(SourceSprite);
	}
	return true;
}

#endif

