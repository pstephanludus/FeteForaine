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

#include "VRPawn.h"
#include "IHeadMountedDisplay.h"
#include "MotionControllerComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AVRPawn::AVRPawn(const class FObjectInitializer& PCIP) : Super(PCIP)
{

	
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	enableGravity = false;

	oculusLocationOffset = FVector(0.f, 0.f, 150.f);

	// Set Base Eye Height
	this->BaseEyeHeight = 0.f;

	// Set root scene component - use static mesh to ensure collisions
	RootComponent = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("SceneRoot"));

	// Add Scene component (for headset positioning), set to -110 to ensure headset starts at floor
	scene = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("VRBaseScene"));
	scene->SetRelativeLocation(FVector(0.f, 0.f, -110.f));
	scene->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Add camera
	camera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	camera->AttachToComponent(scene, FAttachmentTransformRules::KeepRelativeTransform);
	camera->SetFieldOfView(110.f);

	// Add Capsule Collission, set default VR half height and radius values
	capsuleCollision = PCIP.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CapsuleCollision"));
	capsuleCollision->SetCapsuleHalfHeight(96.f);
	capsuleCollision->SetCapsuleRadius(22.f);
	capsuleCollision->SetRelativeLocation(FVector(0.f, 0.f, -110.f));
	capsuleCollision->AttachToComponent(camera, FAttachmentTransformRules::KeepRelativeTransform);

	// Add Motion Controllers
	motionControllerLeft = PCIP.CreateDefaultSubobject<UMotionControllerComponent>(this, TEXT("MotionController_Left"));
	motionControllerLeft->Hand = EControllerHand::Left;
	motionControllerLeft->AttachToComponent(scene, FAttachmentTransformRules::KeepRelativeTransform);
	motionControllerLeft->SetRelativeLocation(FVector(0.f, 0.f, 110.f));

	motionControllerRight = PCIP.CreateDefaultSubobject<UMotionControllerComponent>(this, TEXT("MotionController_Right"));
	motionControllerRight->Hand = EControllerHand::Right;
	motionControllerRight->AttachToComponent(scene, FAttachmentTransformRules::KeepRelativeTransform);
	motionControllerRight->SetRelativeLocation(FVector(0.f, 0.f, 110.f));

	HitResult = false;

}
// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	

	// Adjust pawn spawn target offset based on HMD
	if (GEngine->HMDDevice.IsValid())
	{

		// Override height offset for Oculus Rift
		switch (GEngine->HMDDevice->GetHMDDeviceType())
		{
		case EHMDDeviceType::DT_OculusRift:
			this->SetActorLocation(this->GetActorLocation() + oculusLocationOffset);
			GEngine->HMDDevice->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
			break;
		default:
			break;
		}

		// Set tracking origin (Oculus & Vive)
		GEngine->HMDDevice->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}

}
// Override all default pawn values
void AVRPawn::OverridePawnValues(float PawnBaseEyeHeight, float FOV, float CapsuleHalfHeight, float CapsuleRadius, FVector CapsuleRelativeLocation, FVector SceneLocation, FVector LeftControllerLocation, FVector RightControllerLocation)
{
	// Set Pawn base eye hegiht
	this->BaseEyeHeight = PawnBaseEyeHeight;

	// Set Camera Field of View
	camera->SetFieldOfView(FOV);

	// Set capsule collision settings
	capsuleCollision->SetCapsuleHalfHeight(CapsuleHalfHeight);
	capsuleCollision->SetCapsuleRadius(CapsuleRadius);
	capsuleCollision->SetRelativeLocation(CapsuleRelativeLocation);

	// Set scene location
	scene->SetRelativeLocation(SceneLocation);

	// Set motion controller location
	motionControllerLeft->SetRelativeLocation(LeftControllerLocation);
	motionControllerRight->SetRelativeLocation(RightControllerLocation);
}


// Pawn Rotation - usefull for static mouse rotations during development
void AVRPawn::RotatePawn(float RotationRate, float XAxisInput, float YAxisInput)
{
	if (XAxisInput != 0.f)
	{
		this->AddActorLocalRotation(FRotator(0.f, XAxisInput * RotationRate, 0.f));
	}

	if (YAxisInput != 0.f)
	{
		this->AddActorLocalRotation(FRotator(0.f, YAxisInput * RotationRate, 0.f));
	}

}

// Check if the HMD is worn
bool AVRPawn::IsHMDWorn()
{
	if (GEngine->HMDDevice.IsValid())
	{
		if (GEngine->HMDDevice->GetHMDWornState() == EHMDWornState::Worn)
		{
			return true;
		}
	}

	return false;
}

// Print debug message
void AVRPawn::PrintDebugMessage(FString Message, bool OverwriteExisting, float Duration, FColor Color)
{

	int32 Key;

	if (OverwriteExisting)
	{
		Key = 0;
	}
	else
	{
		Key = -1;
	}

	GEngine->AddOnScreenDebugMessage(Key, Duration, Color, Message);
}



// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// Get the current position of the Camera
	FVector CameraPosition = camera->GetComponentTransform().GetLocation();

	// Apply gravity if enabled and camera is positioned at the head of the player
	if (enableGravity && camera->IsValidLowLevel() && CameraPosition.Z > this->GetActorLocation().Z)
	{
		// Set line trace for gravity variables
		FHitResult RayHit(EForceInit::ForceInit);
		FCollisionQueryParams RayTraceParams(FName(TEXT("GravityRayTrace")), true, this->GetOwner());

		// Initialize Gravity Trace Hit Result var
		RayTraceParams.bTraceComplex = true;
		RayTraceParams.bTraceAsyncScene = true;
		RayTraceParams.bReturnPhysicalMaterial = false;

		// Do a line trace and check for a component that can be stepped on
		HitResult = GetWorld()->LineTraceSingleByChannel(RayHit, CameraPosition, CameraPosition + FVector(0.f, 0.f, FMath::Abs(gravityVariables.floorTraceRange) * -1.f),
			ECollisionChannel::ECC_Visibility, RayTraceParams);
;


		// Check if we need to float the Pawn over uneven terrain
		if (gravityVariables.respondToUnevenTerrain
			&& HitResult && RayHit.GetComponent()->CanCharacterStepUpOn == ECanBeCharacterBase::ECB_Yes
			&& (RayHit.Distance + gravityVariables.floorTraceTolerance) < gravityVariables.floorTraceRange)
		{
			this->TeleportTo(this->GetActorLocation() + FVector(0.f, 0.f, gravityVariables.floorTraceRange - RayHit.Distance), this->GetActorRotation());

		}

		// Apply gravity
		if (!HitResult || RayHit.GetComponent()->CanCharacterStepUpOn != ECanBeCharacterBase::ECB_Yes)
		{
			this->TeleportTo(this->GetActorLocation() + (gravityVariables.gravityDirection * gravityVariables.gravityStrength), this->GetActorRotation());
		}



	}

	//Debug line
	//UE_LOG(LogTemp, Warning, TEXT("Bool Value is %s"), bHit ? TEXT("True") : TEXT("False"));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("val bHit : %s"), bHit ? TEXT("True") : TEXT("False")));

}



