#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assualt Rifle"),
	EWT_RocketLaucher UMETA(DisplayName = "Rocket Laucher"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};