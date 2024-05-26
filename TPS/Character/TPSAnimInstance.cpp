// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSAnimInstance.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TPS/Weapon/Weapon.h"
#include "TPS/TPSTypes/CombatState.h"

void UTPSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	TPSCharacter = Cast<ATPSCharacter>(TryGetPawnOwner());
}

void UTPSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (TPSCharacter == nullptr)
	{
		TPSCharacter = Cast<ATPSCharacter>(TryGetPawnOwner());
	}

	if (TPSCharacter == nullptr) return;

	FVector Velocity = TPSCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = TPSCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = TPSCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size()>0.f ? true :  false;
	bWeaponEquipped = TPSCharacter->IsWeaponEquipped();
	EquippedWeapon = TPSCharacter->GetEquippedWeapon();
	bIsCrouched = TPSCharacter->bIsCrouched;
	bIsAiming = TPSCharacter->IsAiming();
	TurningInPlace = TPSCharacter->getTurningInPlace();
	bRotateRootBone = TPSCharacter->ShoildRotateRootBone();
	bElimmed = TPSCharacter->IsElimmed();

	//Offset Yaw for Strafing
	FRotator AimRotation = TPSCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(TPSCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = TPSCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = TPSCharacter->GetAO_Yaw();
	AO_Pitch = TPSCharacter->GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && TPSCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		TPSCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (TPSCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - TPSCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
		}
	}

	bUseFABRIK = TPSCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAimOffsets = TPSCharacter->GetCombatState() != ECombatState::ECS_Reloading && !TPSCharacter->GetDisableGameplay();
	bTransformRightHand = TPSCharacter->GetCombatState() != ECombatState::ECS_Reloading && !TPSCharacter->GetDisableGameplay();;
	
}
