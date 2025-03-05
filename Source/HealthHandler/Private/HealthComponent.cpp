// VícAgent CopyRights All right Reserved.


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
	UE_LOG(LogTemp, Warning, TEXT("HC_Plug: The Ownwer is Death"));

	OnDeathEvent.Broadcast(LastHitDamage);
}

void UHealthComponent::OnWound(bool bWound, float Damage)
{
	UE_LOG(LogTemp, Warning, TEXT("HC_Plug:The Ownwer is Wound"));
	DamageReceived = Damage;
	OnWoundEvent.Broadcast(bWound, Damage);
}

void UHealthComponent::OnArmor(float newAromourValue)
{
	onArmourChanged.Broadcast(newAromourValue);
}

void UHealthComponent::OnRecovery()
{
	UE_LOG(LogTemp, Warning, TEXT("HealtComp Plugin: The Ownwer is in Recovery"));

	GetWorld()->GetTimerManager().ClearTimer(T_RecoveryFromWound);

	
	
	RegenPerSec();
	OnRecoveryEvent.Broadcast();
	bIsWounded = false;
	

}


void UHealthComponent::RegenPerSec()
{
	UE_LOG(LogTemp, Warning, TEXT("HealtComp Plugin: The Ownwer is in HealthEffect"));

	if ( Health >= MaxHealth ||  Health <= 0.f ) {
		GetWorld()->GetTimerManager().ClearTimer(T_HealthRegen);
		return;
	}
	
	if(!HealthRegen_HasDuration)
	{
		Heal(MaxHealth);
	}

	Heal(AmtHealthEffect * 10);

	
	GetWorld()->GetTimerManager().SetTimer(T_HealthRegen, this, 
		&UHealthComponent::RegenPerSec,
		TimeHealthRegen, 
		true);
	
}

void UHealthComponent::DamagePerSec() {

	AmtHealthEffect *= -1;
	RegenPerSec();

}
void UHealthComponent::HandledTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	if (Damage < 0.f || bIsDead)
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

	UpdateHealth(Health - DMG);

	if (Health <= 0)
	{
		bIsDead = true;
		OnDeath(Damage);
		return;
	}
		
	bIsWounded = true;
	
	
	UE_LOG(LogTemp, Warning, TEXT("The Character is Still Wounded? %s"), (bIsWounded ? TEXT("true") : TEXT("false")));

	OnWound(bIsWounded, Damage);
	OnHealthChanged.Broadcast(this, Health,
		Damage,
		DamageType, InstigatedBy, DamageCauser);// nullptr, nullptr, nullptr);
	

	if (TimeToRecovery <= 0.f)
	{
		OnRecoveryEvent.Broadcast();
		OnRecovery();		
		return;
	}
	else 
	{		
		GetWorld()->GetTimerManager().SetTimer(T_RecoveryFromWound, this,
			&UHealthComponent::OnRecovery, TimeToRecovery, false);
	}
	
	

}


void UHealthComponent::Heal(float DeltaHeal)
{
	if (DeltaHeal < 0.f || bIsDead)
	{
		return;
	}
	
	UpdateHealth(Health + DeltaHeal);

	OnHealthChanged.Broadcast(this, Health, DeltaHeal, nullptr, nullptr, nullptr);

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
	
	Health = FMath::Clamp(DeltaHealth, 0.f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Health changed: %f"), Health);
}

void UHealthComponent::UpdateArmor(float DeltaArmor)
{
	Armor = FMath::Clamp(DeltaArmor, 0.f, MaxArmor);

	UE_LOG(LogTemp, Warning, TEXT("Armor changed: %f"), Armor);
}

void UHealthComponent::UpdateMaxHealth(float DeltaHealth)
{
	
	MaxHealth = FMath::Clamp(DeltaHealth, 0.f, DefaultMaxValueStat);

	UE_LOG(LogTemp, Warning, TEXT("MaxHealth changed: %f"), MaxHealth);
}

void UHealthComponent::UpdateMaxArmot(float DeltaArmor)
{
	MaxArmor = FMath::Clamp(DeltaArmor, 0.f, DefaultMaxValueStat);

	UE_LOG(LogTemp, Warning, TEXT("MaxArmor changed: %f"), MaxArmor);
}

float UHealthComponent::GetHealthBarPercent() const
{
	return (Health / MaxHealth);
}

float UHealthComponent::GetArmorBarPercent() const
{
	return (Armor/MaxArmor);
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
	DOREPLIFETIME_CONDITION(UHealthComponent, DamageReceived, COND_OwnerOnly);
}



