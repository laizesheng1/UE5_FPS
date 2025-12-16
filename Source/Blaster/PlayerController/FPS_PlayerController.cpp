// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_PlayerController.h"
#include "Blaster/HUD/FPS_HUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blaster/Character/BlasterCharacter.h"

void AFPS_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	FPS_HUD = Cast<AFPS_HUD>(GetHUD());

}

void AFPS_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
	CheckTimeSync(DeltaTime);
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
}

void AFPS_PlayerController::SetHUDScore(float Score)
{
	FPS_HUD = FPS_HUD == nullptr ? Cast<AFPS_HUD>(GetHUD()) : FPS_HUD;

	if (FPS_HUD && FPS_HUD->CharacterOverlay && FPS_HUD->CharacterOverlay->ScoreAmount)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		FPS_HUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
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
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60.f;
		FString MatchCountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		FPS_HUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(MatchCountdownText));
	}
}

void AFPS_PlayerController::SetHUDTime()
{
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());
	if (CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountdown(MatchTime - GetServerTime());
	}
	CountdownInt = SecondsLeft;
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
