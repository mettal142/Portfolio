#include "TPSPlayerController.h"
#include "TPS/HUD/TPSHUD.h"
#include "TPS/HUD/CharacterOverlay.h"
#include "TPS/HUD/Announcement.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TPS/Character/TPSCharacter.h"
#include "Net/UnrealNetwork.h"
#include "TPS/GameMode/TPSGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "TPS/TPSComponents/CombatComponent.h"
#include "TPS/GameState/TPSGameState.h"
#include "TPS/PlayerState/TPSPlayerState.h"

void ATPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TPSHUD = Cast<ATPSHUD>(GetHUD());
	ServerCheckMatchState();

}

void ATPSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSPlayerController, MatchState);
}

void ATPSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();

}

void ATPSPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncFrequency = 0.f;
	}
}


void ATPSPlayerController::ServerCheckMatchState_Implementation()
{
	ATPSGameMode* GameMode = Cast<ATPSGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime,MatchTime, CooldownTime, LevelStartingTime);
	}
}

void ATPSPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup,float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
	if (TPSHUD && MatchState == MatchState::WaitingToStart)
	{
		TPSHUD->AddAnnouncement();
	}
}
void ATPSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ATPSCharacter* TPSCharacter = Cast<ATPSCharacter>(InPawn);
	if (TPSCharacter)
	{
		SetHUDHealth(TPSCharacter->GetHealth(), TPSCharacter->GetMaxHealth());
	}
}

void ATPSPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	TPSHUD = TPSHUD == nullptr ? TPSHUD = Cast<ATPSHUD>(GetHUD()) : TPSHUD; 

	bool bHUDValid = TPSHUD &&
		TPSHUD->CharacterOverlay &&
		TPSHUD->CharacterOverlay->HealthBar &&
		TPSHUD->CharacterOverlay->HealthText;

	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		TPSHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"),FMath::CeilToInt(Health),FMath::CeilToInt(MaxHealth));
		TPSHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ATPSPlayerController::SetHUDScore(float Score)
{
	TPSHUD = TPSHUD == nullptr ? TPSHUD = Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bHUDValid = TPSHUD &&
		TPSHUD->CharacterOverlay &&
		TPSHUD->CharacterOverlay->ScoreAmount;
	
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		TPSHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}

}

void ATPSPlayerController::SetHUDDefeats(int32 Defeats)
{
	TPSHUD = TPSHUD == nullptr ? TPSHUD = Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bHUDValid = TPSHUD &&
		TPSHUD->CharacterOverlay &&
		TPSHUD->CharacterOverlay->DefeatsAmount;

	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		TPSHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}

void ATPSPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	TPSHUD = TPSHUD == nullptr ? TPSHUD = Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bHUDValid = TPSHUD &&
		TPSHUD->CharacterOverlay &&
		TPSHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		TPSHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}

}

void ATPSPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	TPSHUD = TPSHUD == nullptr ? TPSHUD = Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bHUDValid = TPSHUD &&
		TPSHUD->CharacterOverlay &&
		TPSHUD->CharacterOverlay->CarriedAmmoAmount;

	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		TPSHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ATPSPlayerController::SetHUDMatCountdown(float CountdownTime)
{
	TPSHUD = TPSHUD == nullptr ? TPSHUD = Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bHUDValid = TPSHUD &&
		TPSHUD->CharacterOverlay &&
		TPSHUD->CharacterOverlay->MatchCountdownText;

	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			TPSHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Secounds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Secounds);
		TPSHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}

}

void ATPSPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	TPSHUD = TPSHUD == nullptr ? TPSHUD = Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	bool bHUDValid = TPSHUD &&
		TPSHUD->Announcement &&
		TPSHUD->Announcement->WarmupTime;

	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			TPSHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Secounds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Secounds);
		TPSHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}

}

void ATPSPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + MatchTime - GetServerTime() + LevelStartingTime;

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority())
	{
		TPSGameMode = TPSGameMode == nullptr ? Cast<ATPSGameMode>(UGameplayStatics::GetGameMode(this)) : TPSGameMode;
		if (TPSGameMode)
		{
			SecondsLeft = FMath::CeilToInt(TPSGameMode->getCountdownTime() + LevelStartingTime);
		}
	}

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatCountdown(TimeLeft);
		}
	}

	CountdownInt = SecondsLeft;

}

void ATPSPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (TPSHUD && TPSHUD->CharacterOverlay)
		{
			CharacterOverlay = TPSHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth,HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}


void ATPSPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ATPSPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RountTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f*RountTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();

}

float ATPSPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds()+ClientServerDelta;
}

void ATPSPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}

}

void ATPSPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}

}

void ATPSPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();

	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ATPSPlayerController::HandleMatchHasStarted()
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	if (TPSHUD)
	{
		if (TPSHUD->CharacterOverlay == nullptr) TPSHUD->AddCharacterOverlay();
		if (TPSHUD->Announcement)
		{
			TPSHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	
}

void ATPSPlayerController::HandleCooldown()
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSHUD>(GetHUD()) : TPSHUD;
	if (TPSHUD)
	{
		TPSHUD->CharacterOverlay->RemoveFromParent();
		bool bHUDValid = TPSHUD->Announcement && 
			TPSHUD->Announcement->AnnouncementText && 
			TPSHUD->Announcement->InfoText;

		if (bHUDValid)
		{
			TPSHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In: ");
			TPSHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
	
			ATPSGameState* TPSGameState = Cast<ATPSGameState>(UGameplayStatics::GetGameState(this));
			ATPSPlayerState* TPSPlayerState = GetPlayerState<ATPSPlayerState>();

			if (TPSGameState)
			{
				TArray<ATPSPlayerState*> TopPlayer = TPSGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayer.Num() == 0)
				{
					InfoTextString = FString("There is no Winner.");
				}
				else if (TopPlayer.Num() == 1 && TopPlayer[0] == TPSPlayerState)
				{
					InfoTextString = FString("You are the Winner!");
				}
				else if (TopPlayer.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner : \n%s"),*TopPlayer[0]->GetPlayerName());
				}
				else if (TopPlayer.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win : \n");
					for (auto TiedPlayer : TopPlayer)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
					
				}

				TPSHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}


		}
	}
	ATPSCharacter* TPSCharacter = Cast<ATPSCharacter>(GetPawn());
	if (TPSCharacter&& TPSCharacter->GetCombat())
	{
		TPSCharacter->bDisableGameplay = true;
		TPSCharacter->GetCombat()->FireButtonPressed(false);
	}
}
