Unreal Portfolio
---
+ 엔진 버전 : Unreal Engine 5.1
+ 에디터 : Visual Studio 2022
+ 개발 규모 : 1인 개발

데모 동영상
---
>YouTube

[![이미지 텍스트](https://github.com/mettal142/Portfolio/blob/main/Blaster/Images/HUD.png)](https://youtu.be/nOkH8RUjV6Y)

---
Character
---
+ Character Structure
![스크린샷 2024-03-25 233612](https://github.com/mettal142/Portfolio/blob/main/Blaster/Images/Character%20Structure.png)

---
Animation
---

+ Character Mesh의 손 위치를 총기의 알맞은 소켓에 두기 위해 FABRIK(Forward And Backward Reaching Inverse Kinematics) 사용
![FABRIK](https://github.com/mettal142/Portfolio/blob/main/Blaster/Images/%EC%8A%A4%ED%81%AC%EB%A6%B0%EC%83%B7%202024-05-19%20215345.png)
> FABRIK 적용 코드 - Animation Instance.cpp 

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (BlasterCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
		}
	}

	bUseFABRIK = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAimOffsets = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading && !BlasterCharacter->GetDisableGameplay();
	bTransformRightHand = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading && !BlasterCharacter->GetDisableGameplay();

  
---
CombatComponent
---
무기와의 상호작용 및 교전에 관련된 기능을 구현한 컴포넌트
+ Equip/Unequip : 떨어진진 무기를 착용 및 해제 - E Key
+ Fire : 장착된 무기를 격발 - Mouse Left Button
+ Zoom : FOV를 조정하여 정밀사격 준비 - Mouse Right Button
+ Reload : 무기를 재장전 - R Key

---
Weapon
---
+ Projectile Weapon
  + Rocket Launcher
    
    + Rocket Projectile
      
  + Grenade Launcher
    
    + Grenade Projectile

+ HitScan Weapon
  + Assult Rifle
    
  + SubMachineGun
 
  + Sniper Rifle
 
  + Pistol
 
  + Shotgun

+ Scatter

---
Pickup Item
---
별도의 상호작용 입력 없이 Collision으로 작용하는 아이템
+ Ammo

+ Healing Pack
