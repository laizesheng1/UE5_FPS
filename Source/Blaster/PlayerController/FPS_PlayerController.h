// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPS_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AFPS_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield (float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetServerTime();		//synced with server world clock
	virtual void ReceivedPlayer() override;			//Sync with server clock as soon as possible
	void OnMatchStateSet(FName State);
	void HandleMatchHasStart();
	void HandleCooldown();

	float SingleTripTime = 0.f;

protected:
	virtual void BeginPlay() override;
	void SetHUDTime();
	void PollInit();

	/**
	 * Sync time between client and server 
	 */
	//RPC to send from the client to server to request time
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	//Reports the current server time to the client in response
	UFUNCTION(Client, Reliable)
	void ClientRequestServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f;
	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server,Reliable)
	void ServerCheckMatchState();
	UFUNCTION(Client,Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);

private:
	UPROPERTY()
	class AFPS_HUD* FPS_HUD;
	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;

	float LevelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	float HUDHealth;
	float HUDMaxHealth;
	bool bInitializeHealth = false;
	float HUDScore;
	bool bInitializeScore = false;
	int32 HUDDefeats;
	bool bInitializeDefeats = false;
	int32 HUDGrenades;
	bool bInitializeGrenade = false;
	float HUDShield;
	float HUDMaxShield;
	bool bInitializeShield = false;
	float HUDCarriedAmmo;
	bool bInitializeCarriedAmmo = false;
	float HUDWeaponAmmo;
	bool bInitializeWeaponAmmo = false;

	float HighPingRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;

	float PingAnimationRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.f;
	float HighPingThreshold = 50.f;
};
