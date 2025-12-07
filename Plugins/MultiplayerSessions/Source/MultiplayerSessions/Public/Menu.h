// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberPublicConnections = 4, FString TypeofMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));
protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;
	//自定义委托的回调函数
	UFUNCTION()
	void OnCreateSession(bool bIsSuccessful);
	void OnFindSessions(bool bIsSuccessful, const TArray<FOnlineSessionSearchResult>& SessionsResults);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bIsSuccessful);
	UFUNCTION()
	void OnStartSession(bool bIsSuccessful);
private:
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

	class UMultiplayerSessionSubsystem* MultiplayerSessionSubsystem;
	int32 NumPublicConnections{ 4 };
	FString MatchType = TEXT("FreeForAll");
	FString PathToLobby = TEXT("");
};
