// Fill out your copyright notice in the Description page of Project Settings.

#include "SGame.h"
#include "SGCheatManager.h"
#include "SGTileBase.h"
#include "SGEnemyTileBase.h"
#include "SGPlayerController.h"
#include "SGGrid.h"
#include "SGSpritePawn.h"

USGCheatManager::USGCheatManager()
{
	MessageEndpoint = FMessageEndpoint::Builder("CheatManagerMessageEP");
}

void USGCheatManager::BeginAttack()
{
	// Find the link line actor in the world
	for (TActorIterator<ASGEnemyTileBase> It(GetWorld()); It; ++It)
	{
		ASGEnemyTileBase* Tile = *It;
		Tile->EnemyAttack();
	}
}

void USGCheatManager::UseSkill(int32 inSkillIndex)
{
	ASGPlayerController* MyPC = GetOuterASGPlayerController();

	checkSlow(MyPC->SkillsArray.IsValidIndex(inSkillIndex) && MyPC->SkillsArray[inSkillIndex]);
	MyPC->SkillsArray[inSkillIndex]->PlayerUseSkill();
}


void USGCheatManager::StartGame()
{
	if (MessageEndpoint.IsValid() == true)
	{
		// Test: Send game start message 		
		MessageEndpoint->Publish(new FMessage_Gameplay_GameStart(), EMessageScope::Process);
	}

	// Start the new round
	NewRound();
}

void USGCheatManager::NewRound()
{
	if (MessageEndpoint.IsValid() == true)
	{
		// Test: Send game start message
		FMessage_Gameplay_GameStatusUpdate* GameStatusUpdateMesssage = new FMessage_Gameplay_GameStatusUpdate();
		GameStatusUpdateMesssage->NewGameStatus = ESGGameStatus::EGS_RondBegin;
		MessageEndpoint->Publish(GameStatusUpdateMesssage, EMessageScope::Process);
	}
}

void USGCheatManager::ForceCollect()
{
	if (MessageEndpoint.IsValid() == true)
	{
		// Test: Send game start message
		FMessage_Gameplay_CollectLinkLine* CollectLinkLineMessage = new FMessage_Gameplay_CollectLinkLine();
		MessageEndpoint->Publish(CollectLinkLineMessage, EMessageScope::Process);
	}
}

void USGCheatManager::PlayerEndBuildPath()
{
	if (MessageEndpoint.IsValid() == true)
	{
		// Test: Send game start message
		FMessage_Gameplay_GameStatusUpdate* GameStatusUpdateMesssage = new FMessage_Gameplay_GameStatusUpdate();
		GameStatusUpdateMesssage->NewGameStatus = ESGGameStatus::EGS_PlayerEndBuildPath;
		MessageEndpoint->Publish(GameStatusUpdateMesssage, EMessageScope::Process);
	}
}

void USGCheatManager::SetHealth(int newHealth)
{
	for (TActorIterator<ASGSpritePawn> It(GetWorld()); It; ++It)
	{
		((ASGSpritePawn*)(*It))->SetCurrentHealth(newHealth);
	}
}

void USGCheatManager::ResetGrid()
{
	for (TActorIterator<ASGGrid> It(GetWorld()); It; ++It)
	{
		((ASGGrid*)(*It))->ResetGrid();
	}
}