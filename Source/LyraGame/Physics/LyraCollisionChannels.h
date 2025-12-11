// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


/**
 * when you modify this, please note that this information can be saved with instances
 * also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list
 **/

#define RS_ObjectChannel_Weapon         ECC_GameTraceChannel6
#define RS_ObjectChannel_Projectile		ECC_GameTraceChannel7
#define RS_TraceChannel_AimAssist       ECC_GameTraceChannel1
#define RS_TraceChannel_Interaction		ECC_GameTraceChannel8

#define RS_PhysicalMaterial_Default		SurfaceType_Default
#define RS_PhysicalMaterial_Character	SurfaceType1
#define RS_PhysicalMaterial_Rock		SurfaceType2
#define RS_PhysicalMaterial_Wood		SurfaceType3
