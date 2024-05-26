#include "TPSGameMode.h"
#include "TPS/Character/TPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TPS/PlayerController/TPSPlayerController.h"
#include "Gameframework/PlayerStart.h"
#include "TPS/PlayerState/TPSPlayerState.h"
#include "TPS/GameState/TPSGameState.h"
namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ATPSGameMode::ATPSGameMode()
{
	bDelayedStart = true;

}

void ATPSGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ATPSGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime=WarmupTime+MatchTime- GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ATPSGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATPSPlayerController* TPSPlayer = Cast<ATPSPlayerController>(*It);
		if(TPSPlayer)
		{
			TPSPlayer->OnMatchStateSet(MatchState);
		}
	}
}

void ATPSGameMode::PlayerEliminated(ATPSCharacter* ElimmedCharacter, ATPSPlayerController* VictimController, ATPSPlayerController* AttackerController)
{
	ATPSPlayerState* AttackerPlayerState = AttackerController ? Cast<ATPSPlayerState>(AttackerController->PlayerState) : nullptr;
	ATPSPlayerState* VictimPlayerState = VictimController ? Cast<ATPSPlayerState>(VictimController->PlayerState) : nullptr;

	ATPSGameState* TPSGameState = GetGameState<ATPSGameState>();

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && TPSGameState)
	{
		AttackerPlayerState->AddToScore(1.f);
		TPSGameState->UpdateTopScore(AttackerPlayerState);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ATPSGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController,PlayerStarts[Selection]);
	}
}

