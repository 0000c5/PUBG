#pragma once

namespace offset 
{
	// AHUD
	constexpr auto drawline = 0x6A84F98;
	constexpr auto drawtext = 0x6A854B0;
	constexpr auto drawrect = 0x6A853EC;
	constexpr auto linetrace = 0x6af7504;

	constexpr auto decrypt = 0x8c55e28;
	constexpr auto uworld = 0xa5a1b08;
	constexpr auto local_player = 0xA6E6FC0;
	constexpr auto gnames = 0xa7ad770;

	constexpr auto chunksize = 0x4070;

	constexpr auto persistent_level = 0x150;
	constexpr auto controller = 0x30;
	constexpr auto actors = 0xd0;

	constexpr auto object_id = 0x20;
	constexpr auto myhud = 0x4B8;

	// AHUD
	constexpr auto canvas = 0x430;

	// AController
	constexpr auto camera_manager = 0x4c0;
	constexpr auto pawn = 0x498;

	// APlayerCameraManager
	constexpr auto camera_fov = 0x1c88;
	constexpr auto camera_rot = 0x1c8c;
	constexpr auto camera_loc = 0x1ca0;

	// AActor
	constexpr auto mesh = 0x548;
	constexpr auto root_component = 0x1c0;

	// RootComponent
	constexpr auto component_to_world = 0x270;
	constexpr auto absolute_location = component_to_world + 0x10;

	// AntiCheatSyncManager
	constexpr auto anticheatcharactersyncmanager = 0xC50;
	constexpr auto dormantcharacterclientlist = 0x50;
}