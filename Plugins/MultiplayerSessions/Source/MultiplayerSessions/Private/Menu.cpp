// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 NumberPublicConnections, FString TypeofMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberPublicConnections;
	MatchType = TypeofMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;		//聚焦于这个部件

	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* PlayerController = world->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputeModeData;
			InputeModeData.SetWidgetToFocus(TakeWidget())
				.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputeModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
	UGameInstance* GameInstance = GetGameInstance();
	if(GameInstance)
	{
		MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionSubsystem>();
	}
	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionSubsystem->MultiplayerOnJoinSessionsComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bIsSuccessful)
{
	if (bIsSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15.0f,
				FColor::Yellow,
				FString(TEXT("Session created successfully!")));
		}
		UWorld* world = GetWorld();
		if (world)
		{
			world->ServerTravel(PathToLobby);
		}
	}
	else {
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15.0f,
				FColor::Red,
				FString(TEXT("Failed to create session!")));
		}
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(bool bIsSuccessful, const TArray<FOnlineSessionSearchResult>& SessionsResults)
{
	if (MultiplayerSessionSubsystem == nullptr)
		return;
	for (auto Result : SessionsResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15.0f,
				FColor::Cyan,
				FString::Printf(TEXT("Id: %s, User: %s"), *Id, *User));
		}
		if (SettingsValue == MatchType)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1,
					15.0f,
					FColor::Cyan,
					FString::Printf(TEXT("Join Match Type: %s"), *MatchType));
			}
			MultiplayerSessionSubsystem->JoinSession(Result);
			return;
		}
	}
	if (!bIsSuccessful || SessionsResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			if (!SessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(
						-1,
						15.f,
						FColor::Red,
						FString::Printf(TEXT("Get Connect Error"))
					);
				}
				return;
			}

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Yellow,
					FString::Printf(TEXT("Connect string: %s"), *Address)
				);
			}

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bIsSuccessful)
{
}

void UMenu::OnStartSession(bool bIsSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->FindSession(10000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* PlayerController = world->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputeModeData;
			PlayerController->SetInputMode(InputeModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
