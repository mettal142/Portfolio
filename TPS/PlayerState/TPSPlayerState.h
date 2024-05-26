// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API ATPSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public: 
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	
	/// <summary>
	/// Replication Notifies
	/// </summary>
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();
	
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

private:
	UPROPERTY()
	class ATPSCharacter* Character;
	UPROPERTY()
	class ATPSPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	
};
