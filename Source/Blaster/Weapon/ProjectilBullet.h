// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectilBullet.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectilBullet : public AProjectile
{
	GENERATED_BODY()
public:

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

};
