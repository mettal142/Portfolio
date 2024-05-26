#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TPSGameMode.generated.h"

namespace MatchState
{
	extern TPS_API const FName Cooldown;
}

/**
 * 
 */
UCLASS()
class TPS_API ATPSGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ATPSGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class ATPSCharacter* ElimmedCharacter, class ATPSPlayerController* VictimController, ATPSPlayerController* AttackerController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController);
	
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 10.f;
public:
	FORCEINLINE float getCountdownTime() const { return CountdownTime; }

};
