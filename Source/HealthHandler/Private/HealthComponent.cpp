// License
// This Plugin is created @Author: VicAgent Marz 2025
// This project is licensed under the [MIT License] (https://mit-license.org/).


#include "HealthComponent.h"

//Engine
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/DamageType.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

//Utils
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"


UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	// ...


}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	AActor* MyOwner = GetOwner();
	if (MyOwner && MyOwner->HasAuthority())
	{

		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandledTakeAnyDamage);


	}

}

void UHealthComponent::OnDeath(float LastHitDamage)
{
	UE_LOG(LogTemp, Warning, TEXT("HealtComp Plugin: The Ownwer is Death"));
	OnDeathEvent.Broadcast(LastHitDamage);
}

void UHealthComponent::OnWound(bool bWound, float Damage)
{
	UE_LOG(LogTemp, Warning, TEXT("HealtComp Plugin:The Ownwer is Wound"));
	bIsWounded = true;
	OnWoundEvent.Broadcast(bWound, Damage);
}

void UHealthComponent::OnArmor(float newAromourValue)
{
	UE_LOG(LogTemp, Warning, TEXT("HealtComp Plugin: The Ownwer is breaking the armor"));
	onArmorChanged.Broadcast(newAromourValue);
}

void UHealthComponent::OnHealing(float HealAmount)
{	
	UE_LOG(LogTemp, Warning, TEXT("HealtComp Plugin: The Ownwer is healing"));
	bIsWounded = false;
	OnHealingEvent.Broadcast(HealAmount);
}


void UHealthComponent::AutoHealthRegen()
{
	UE_LOG(LogTemp, Warning, TEXT("HealtComp Plugin: The Ownwer is in HealthEffect"));
	
	GetWorld()->GetTimerManager().ClearTimer(T_AutoHeal);

	if (Health >= MaxHealth || CalculeIsDead())
	{
		
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimer(T_AutoHeal, FTimerDelegate::CreateLambda(
		[this] () {
			Heal(HealthRegenAmount);
			if(Health >= MaxHealth || bIsWounded){ 
				GetWorld()->GetTimerManager().ClearTimer(T_AutoHeal); 
			}			
		}
	), HealthRegenPeriod, true);	
	
}

void UHealthComponent::SetupAutoHealthRegen(float RegenPeriod, float HealAmount)
{
	HealthRegenPeriod = RegenPeriod;
	HealthRegenAmount = HealAmount;
}

void UHealthComponent::HandledTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	if (Damage < 0.f || CalculeIsDead())
	{
		return;
	}
	 
	float DMG= Damage;

	if( Armor > 0  ) 
	{
		if (Armor > Damage) { 
			Armor -= DMG;
			OnArmor(Armor);
			return;
		}
		
		DMG -= Armor;
		Armor = 0;
		OnArmor(Armor);
	}

	GetWorld()->GetTimerManager().ClearTimer(T_AutoHeal);
	UpdateHealth(Health - DMG);
	
	UE_LOG(LogTemp, Warning, TEXT("The Character is Still Wounded? %s"), (bIsWounded ? TEXT("true") : TEXT("false")));
	
	
	OnWound(true, Damage);
	OnHealthChanged.Broadcast(this, Health,
		Damage,
		DamageType, InstigatedBy, DamageCauser);// nullptr, nullptr, nullptr);
	
	if (CalculeIsDead())
	{
		OnDeath(Damage);
		return;
	}
	if (!HasAutoHealthRegen) return;


	FTimerHandle T_DelayAutoHeal;
	GetWorld()->GetTimerManager().ClearTimer(T_DelayAutoHeal);
	GetWorld()->GetTimerManager().SetTimer(T_DelayAutoHeal, FTimerDelegate::CreateLambda(
		[this]() {
			AutoHealthRegen();
		}
	),DealyToAutoHealthRegen, false);
	

}


void UHealthComponent::Heal(float DeltaHeal)
{
	if (DeltaHeal < 0.f || CalculeIsDead())
	{
		return;
	}
	
	UpdateHealth(Health + DeltaHeal);

	OnHealthChanged.Broadcast(this, Health, DeltaHeal, nullptr, nullptr, nullptr);
	OnHealing(DeltaHeal);
}



void UHealthComponent::FixArmor(float DeltaArmor)
{
	if ( DeltaArmor < 0.f || bIsDead)//|| Armor <= 0.0f)
	{
		return;
	}

	UpdateArmor(Armor + DeltaArmor);

	OnArmor(Armor);
}


void UHealthComponent::OnRep_Health(float OldHealth)
{
	float DeltaHealth = Health - OldHealth;
	
	OnHealthChanged.Broadcast(this, Health, DeltaHealth, nullptr, nullptr, nullptr);

	if (bIsDead)  OnDeath(DeltaHealth);
	else if (bIsWounded) OnWound(bIsWounded, DeltaHealth);
	
}

void UHealthComponent::OnRep_Armor()
{	
	if(Armor<=0) return;
	OnArmor(Armor);
}



void UHealthComponent::UpdateHealth(float DeltaHealth)
{
	if(GetOwner()->GetLocalRole()==ROLE_Authority)
	{
		Health = FMath::Clamp(DeltaHealth, 0.f, MaxHealth);
		CalculeIsDead();
		UE_LOG(LogTemp, Warning, TEXT("Health changed: %f"), Health);
	}
	
}

void UHealthComponent::UpdateArmor(float DeltaArmor)
{
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		Armor = FMath::Clamp(DeltaArmor, 0.f, MaxArmor);

		UE_LOG(LogTemp, Warning, TEXT("Armor changed: %f"), Armor);
	}
	
}

void UHealthComponent::UpdateMaxHealth(float DeltaHealth)
{
	if (GetOwner()->GetLocalRole() == ROLE_Authority) {
		MaxHealth = FMath::Clamp(DeltaHealth, 0.f, DefaultMaxValueStat);

		UE_LOG(LogTemp, Warning, TEXT("MaxHealth changed: %f"), MaxHealth);
	}

}

void UHealthComponent::UpdateMaxArmor(float DeltaArmor)
{
	if (GetOwner()->GetLocalRole() == ROLE_Authority) {
		MaxArmor = FMath::Clamp(DeltaArmor, 0.f, DefaultMaxValueStat);

		UE_LOG(LogTemp, Warning, TEXT("MaxArmor changed: %f"), MaxArmor);
	}
	
}





/** Setting replicated variables*/
void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, MaxHealth);
	DOREPLIFETIME(UHealthComponent, Armor);
	DOREPLIFETIME(UHealthComponent, MaxArmor);	
	DOREPLIFETIME(UHealthComponent, bIsDead);
	DOREPLIFETIME(UHealthComponent, bIsWounded);
	
	


}

