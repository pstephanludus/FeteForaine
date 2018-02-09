// Copyright (C) 2017 Nicolas Lehmann NglStudio

/*Copyright (C) 2017 Nicolas Lehmann NglStudio

Gnu general public license version 3

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once


#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine.h"//add engine.h for FPredictProjectilePathParams Kismet
#include "VRTeleporter.generated.h"

UENUM(BlueprintType)
enum class EMoveDirectionEnum : uint8
{
	MOVE_FORWARD 	UMETA(DisplayName = "Towards Player"),
	MOVE_BACKWARD 	UMETA(DisplayName = "Away from Player"),
	MOVE_LEFT		UMETA(DisplayName = "Left of Player"),
	MOVE_RIGHT		UMETA(DisplayName = "Right of Player"),
	MOVE_CUSTOM		UMETA(DisplayName = "Use a Custom Rotation for Direction")
};

//Forward declaration

class UStaticMesh;
class UParticleSystem;



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEMPLATE_VRPLUGNL_V2_API UVRTeleporter : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVRTeleporter();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// The teleport beam's mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
		UStaticMesh* teleportBeamMesh;

	/** The teleport beam's Launch Velocity Magnitude - higher number increases range of teleport */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
		float beamMagnitude;

	/** A location offset from the parent mesh origin where the teleport beam will start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
		FVector beamLocationOffset;

	/** For ray type beam, ensure the lenth of the beam reaches target location instantenously. Uses RayScaleRate as base length unit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
		bool rayInstantScale;

	/** How much the ray will scale up until it reaches target location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
		float rayScaleRate;

	/** The teleport beam's navigation mesh tolerance - fine tune to fit your nav mesh bounds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
		FVector beamHitNavMeshTolerance;

	/** The teleport beam's custom gravity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Beam Parameters")
		float arcOverrideGravity;

	// The teleport target stuff

	/** Additional offset of pawn (internal offsets are Steam: 112, Rift: 250) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
		FVector teleportTargetPawnSpawnOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
		float floorIsAtZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
		UStaticMesh* teleportTargetMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
		FVector teleportTargetMeshScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
		FVector teleportTargetMeshSpawnOffset;

	/** Custom marker rotation (applied per frame) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
		FRotator customMarkerRotation;

	/** If player should face marker rotation (use with Custom Marker Rotation) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
		bool faceMarkerRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
		UParticleSystem* teleportTargetParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
		FVector teleportTargetParticleScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR - Teleport Target Parameters")
		FVector teleportTargetParticleSpawnOffset;

	/** Check to see if an active teleport mode is turned on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VR - Read Only")
		bool isTeleporting;


	// Show the teleportation arc trace
	UFUNCTION(BlueprintCallable, Category = "VR")
		bool showTeleportArc();

	// Show the teleportation ray trace
	UFUNCTION(BlueprintCallable, Category = "VR")
		bool showTeleportRay();

	// Remove the teleportation arc trace
	UFUNCTION(BlueprintCallable, Category = "VR")
		bool hideTeleportArc();

	// Remove the teleportation ray trace
	UFUNCTION(BlueprintCallable, Category = "VR")
		bool hideTeleportRay();

	// Show marker in the world
	UFUNCTION(BlueprintCallable, Category = "VR")
		bool showMarker();

	// Move Marker
	UFUNCTION(BlueprintCallable, Category = "VR")
		bool moveMarker(EMoveDirectionEnum MarkerDirection = EMoveDirectionEnum::MOVE_FORWARD, int Rate = 25, FRotator CustomDirection = FRotator::ZeroRotator);

	// Remove marker
	UFUNCTION(BlueprintCallable, Category = "VR")
		bool hideMarker();

	// Teleport
	UFUNCTION(BlueprintCallable, Category = "VR")
		bool teleportNow();

	private:
		// Teleport target height offset - defaults to SteamVR
		FVector pawnHeightOffset;

		// Teleport targetting mode
		int teleportMode;

		// Teleport Arc constants
		const float arcRadius = 0.f;
		const float maxSimTime=2.f;
		const float simFrequency=30.f;


		// Teleport Arc spline parameters
		USplineComponent *arcSpline;
		TArray<FVector> arcPoints;
		TArray<USplineMeshComponent*> arcSplineMeshes;
		TArray<TEnumAsByte<EObjectTypeQuery> > arcObjectTypesToIgnore;
		FVector rayMeshScale;
		FVector rayMeshScale_Max;
		bool isBeamTypeTeleport;
		float rayNumOfTimesToScale;
		float rayNumOfTimesToScale_Actual;
		float rayDistanceToTarget;


		// TeleportRay mesh
		UStaticMeshComponent* rayMesh;

		// Teleport target location
		FVector targetLocation;
		FRotator targetRotation;
		bool isTargetLocationValid;

		// Spawned visible components for targetting marker
		UParticleSystemComponent* targetParticleSystemComponent;
		UStaticMeshComponent* targetStaticMeshComponent;

		// Draw teleport arc
		void drawTeleportArc();

		// Clear teleport arc spline
		void clearTeleportArc();

		// Draw teleport ray
		void drawTeleportRay();

		// Clear teleport arc spline
		void clearTeleportRay();

		// Spawn target location marker
		void spawnTargetMarker(FVector MarkerLocation = FVector::ZeroVector, FRotator MarkerRotation = FRotator::ZeroRotator);

		// Show target location marker
		void setTargetMarkerVisibility(bool MakeVisible = false);

		// Show target location marker
		void setTargetMarkerLocationAndRotation(FVector MarkerLocation = FVector::ZeroVector, FRotator MarkerRotation = FRotator::ZeroRotator);

		// Remove target location marker
		void removeTargetMarker();

};
