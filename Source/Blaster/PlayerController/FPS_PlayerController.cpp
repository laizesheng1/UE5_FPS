// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_PlayerController.h"
#include "Blaster/HUD/FPS_HUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void AFPS_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	FPS_HUD = Cast<AFPS_HUD>(GetHUD());

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