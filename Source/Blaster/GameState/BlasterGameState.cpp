// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameState.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/PlayerController/FPS_PlayerController.h"

void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlasterGameState, TopScoringPlayers);
	DOREPLIFETIME(ABlasterGameState, RedTeamScore);
	DOREPLIFETIME(ABlasterGameState, BlueTeamScore);
}

void ABlasterGameState::UpdateTopScore(ABlasterPlayerState* ScoreingPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoreingPlayer);
		TopScore = ScoreingPlayer->GetScore();
	}
	else if (ScoreingPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoreingPlayer);
	}
	else if (ScoreingPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.Add(ScoreingPlayer);
		TopScore = ScoreingPlayer->GetScore();
	}
}

void ABlasterGameState::RedTeamScores()
{
	++RedTeamScore;

	AFPS_PlayerController* FPlayer = Cast<AFPS_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (FPlayer)
	{
		FPlayer->SetHUDRedTeamScore(RedTeamScore);
	}
}

void ABlasterGameState::BlueTeamScores()
{
	++BlueTeamScore;
	AFPS_PlayerController* FPlayer = Cast<AFPS_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (FPlayer)
	{
		FPlayer->SetHUDBlueTeamScore(BlueTeamScore);
	}
}

void ABlasterGameState::OnRep_RedTeamScore()
{
	AFPS_PlayerController* FPlayer = Cast<AFPS_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (FPlayer)
	{
		FPlayer->SetHUDRedTeamScore(RedTeamScore);
	}
}

void ABlasterGameState::OnRep_BlueTeamScore()
{
	AFPS_PlayerController* FPlayer = Cast<AFPS_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (FPlayer)
	{
		FPlayer->SetHUDBlueTeamScore(BlueTeamScore);
	}
}
