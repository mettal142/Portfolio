#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class ABlasterCharacter;
public:	
	UBuffComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Heal(float HealAmount, float HealingTime);
	void HealRampUp(float DeltaTime);


protected:
	virtual void BeginPlay() override;

	bool bHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;


private:
	UPROPERTY()
	class ABlasterCharacter* Character;
		
};
