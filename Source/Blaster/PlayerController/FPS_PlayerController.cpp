// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_PlayerController.h"
#include "Blaster/HUD/FPS_HUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

void AFPS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	FPS_HUD = Cast<AFPS_HUD>(GetHUD());
	ServerCheckMatchState();
}

void AFPS_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPS_PlayerController, MatchState);

}

void AFPS_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();			//set HUD before characteroverlay was initialized
}



void AFPS_PlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void AFPS_PlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		CooldownTime = GameMode->CooldownTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, LevelStartingTime, CooldownTime);

		//when test is a bug, in listen server is a server and client
		//if (FPS_HUD && MatchState == MatchState::WaitingToStart)
		//{
		//	FPS_HUD->AddAnnoucement();
		//}
	}
}

void AFPS_PlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	CooldownTime = Cooldown;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
	if (FPS_HUD && MatchState == MatchState::WaitingToStart)
	{
		FPS_HUD->AddAnnoucement();
	}
}

void AFPS_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
}

void AFPS_PlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	FPS_HUD = FPS_HUD == nullptr ? Cast<AFPS_HUD>(GetHUD()) : FPS_HUD;

	if (FPS_HUD && FPS_HUD->CharacterOverlay && FPS_HUD->CharacterOverlay->HealthBar &&
		FPS_HUD->CharacterOverlay->HealthText)
	{
		const float HealthPrecent = Health / MaxHealth;
		FPS_HUD->CharacterOverlay->HealthBar->SetPercent(HealthPrecent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		FPS_HUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void AFPS_PlayerController::SetHUDScore(float Score)
{
	FPS_HUD = FPS_HUD == nullptr ? Cast<AFPS_HUD>(GetHUD()) : FPS_HUD;

	if (FPS_HUD && FPS_HUD->CharacterOverlay && FPS_HUD->CharacterOverlay->ScoreAmount)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		FPS_HUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

void AFPS_PlayerController::SetHUDDefeats(int32 Defeats)
{
	FPS_HUD = FPS_HUD == nullptr ? Cast<AFPS_HUD>(GetHUD()) : FPS_HUD;
	if (FPS_HUD && FPS_HUD->CharacterOverlay && FPS_HUD->CharacterOverlay->DefeatsAmount)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		FPS_HUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}

void AFPS_PlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	FPS_HUD = FPS_HUD == nullptr ? Cast<AFPS_HUD>(GetHUD()) : FPS_HUD;
	if (FPS_HUD && FPS_HUD->CharacterOverlay && FPS_HUD->CharacterOverlay->WeaponAmmoAmount)
	{
		FString WeaponAmmoText = FString::Printf(TEXT("%d"), Ammo);
		FPS_HUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(WeaponAmmoText));
	}
}

void AFPS_PlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	FPS_HUD = FPS_HUD == nullptr ? Cast<AFPS_HUD>(GetHUD()) : FPS_HUD;

	if (FPS_HUD && FPS_HUD->CharacterOverlay && FPS_HUD->CharacterOverlay->CarriedAmmoAmount)
	{
		FString CarriedAmmoText = FString::Printf(TEXT("%d"), Ammo);
		FPS_HUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));
	}
}

void AFPS_PlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	FPS_HUD = FPS_HUD == nullptr ? Cast<AFPS_HUD>(GetHUD()) : FPS_HUD;

	if (FPS_HUD && FPS_HUD->CharacterOverlay && FPS_HUD->CharacterOverlay->MatchCountdownText)
	{
		if (CountdownTime < 0.f)
		{
			FPS_HUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60.f;
		FString MatchCountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		FPS_HUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(MatchCountdownText));
	}
}

void AFPS_PlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	FPS_HUD = FPS_HUD == nullptr ? Cast<AFPS_HUD>(GetHUD()) : FPS_HUD;

	if (FPS_HUD && FPS_HUD->Annoucement && FPS_HUD->Annoucement->WarmupTime)
	{
		if (CountdownTime < 0.f)
		{
			FPS_HUD->Annoucement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60.f;
		FString WarmupTimeText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		FPS_HUD->Annoucement->WarmupTime->SetText(FText::FromString(WarmupTimeText));
	}
}

void AFPS_PlayerController::SetHUDGrenades(int32 Grenades)
{
	FPS_HUD = FPS_HUD == nullptr ? Cast<AFPS_HUD>(GetHUD()) : FPS_HUD;
	if (FPS_HUD && FPS_HUD->CharacterOverlay && FPS_HUD->CharacterOverlay->GrenadeText)
	{
		FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
		FPS_HUD->CharacterOverlay->GrenadeText->SetText(FText::FromString(GrenadesText));
	}
	else
	{
		HUDGrenades = Grenades;
	}
}

void AFPS_PlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart)
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress)
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown)
		TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	/*if (HasAuthority())			//left a bug :server time can't down, remove is good
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCooldownTime() + LevelStartingTime);
		}
	}*/

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondsLeft;			//one second interal
}

void AFPS_PlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientRequestServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AFPS_PlayerController::ClientRequestServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (RoundTripTime * 0.5f);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AFPS_PlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AFPS_PlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

//on server
void AFPS_PlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStart();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AFPS_PlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStart();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AFPS_PlayerController::HandleMatchHasStart()
{
	FPS_HUD = FPS_HUD == nullptr ? Cast<AFPS_HUD>(GetHUD()) : FPS_HUD;
	if (FPS_HUD)
	{
		if (FPS_HUD->CharacterOverlay == nullptr) FPS_HUD->AddCharacterOverlay();
		if (FPS_HUD->Annoucement)
		{
			FPS_HUD->Annoucement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AFPS_PlayerController::HandleCooldown()
{
	FPS_HUD = FPS_HUD == nullptr ? Cast<AFPS_HUD>(GetHUD()) : FPS_HUD;
	if (FPS_HUD)
	{
		FPS_HUD->CharacterOverlay->RemoveFromParent();
		if (FPS_HUD->Annoucement && FPS_HUD->Annoucement->AnnouncementText
			&& FPS_HUD->Annoucement->InfoText)
		{
			FPS_HUD->Annoucement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Start In:");
			FPS_HUD->Annoucement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayState = GetPlayerState<ABlasterPlayerState>();
			if (BlasterGameState && BlasterPlayState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner.");
				}
				else if (TopPlayers.Num() == 1&& TopPlayers[0]== BlasterPlayState)
				{
					InfoTextString = FString("You are the winner!");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win: \n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
				FPS_HUD->Annoucement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BlasterCharacter && BlasterCharacter->GetCombat())
	{
		BlasterCharacter->bDisableGameplay = true;
		BlasterCharacter->GetCombat()->FireButtonPressed(false);
	}
}

void AFPS_PlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (FPS_HUD && FPS_HUD->CharacterOverlay)
		{
			CharacterOverlay = FPS_HUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);

				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
				if (BlasterCharacter && BlasterCharacter->GetCombat())
				{
					SetHUDGrenades(BlasterCharacter->GetCombat()->GetGrenades());
				}
			}
		}
	}
}
