#include "decrypt.hpp"
#include "hook.hpp"
#include "sdk.hpp"


#include <xorstr.hpp>
#pragma warning (disable : 4996)

uint64_t entities[100];
uint32_t entity_count = 0;

uint64_t camera_manager = 0, uworld = 0, localplayer_pawn = 0, actors_tarray = 0;

bool linetracesingle(const fvector& start, const fvector& end, uint64_t target)
{
	using call_type = bool(__fastcall*)(
		void*, // UWorld,
		const fvector& Start,
		const fvector& End,
		tenumasbyte<etracetypequery>,
		bool bTraceComplex,
		const tarray<uint64_t>&,
		tenumasbyte<edrawdebugtrace>,
		fhitresult* OutHit,
		bool bIgnoreSelf,
		const flinearcolor&,
		const flinearcolor&,
		float DrawTime
		);

	fhitresult hit;

	tarray<uint64_t> actorstoignore(&target, 1);

	if (uworld && actors_tarray)
		return reinterpret_cast<call_type>(base + offset::linetrace)((void*)uworld, start, end, etracetypequery::tracetypequery1, true, actorstoignore, edrawdebugtrace::edrawdebugtrace__none, &hit, true, flinearcolor(), flinearcolor(), 0.f);

	return false;
}

uint64_t gnames = 0;
std::string get_gname_by_id(int32_t id)
{
	int64_t fNamePtr = read<int64_t>(gnames + int(id / offset::chunksize) * 0x8);
	if (fNamePtr)
	{
		int64_t fName = read<uint64_t>(fNamePtr + int(id % offset::chunksize) * 0x8);
		if (valid_ptr(fName))
		{
			char buffer[64] = { NULL };
			memcpy(buffer, (PVOID)(fName + 0x10), sizeof(buffer));
			return buffer;
		}
	}

	return "";
}

camera get_camera()
{
	return camera(
		read<fvector>(camera_manager + offset::camera_loc),
		read<fvector>(camera_manager + offset::camera_rot),
		read<float>(camera_manager + offset::camera_fov));
}

void* o_post_render = nullptr;
void post_render(AHUD* myhud)
{
	uint64_t canvas = read<uint64_t>((uint64_t)myhud + offset::canvas);
	camera cam = get_camera();

	width = read<int>(canvas + 0x40);
	height = read<int>(canvas + 0x44);

	for (uint32_t i = 0; i < entity_count; i++)
	{
		uint64_t actor = entities[i];

		uint64_t mesh = read<uint64_t>(actor + offset::mesh);
		if (!mesh) continue;

		uint64_t root_component = decrypt.xenuine(read<uint64_t>(actor + offset::root_component));
		if (!root_component) continue;

		fvector2d out;
		fvector root_pos = read<fvector>(root_component + offset::absolute_location);
		if (!cam.w2s(root_pos, &out)) continue;

		flinearcolor color = linetracesingle(cam.loc, root_pos, actor) ? flinearcolor(1, 0, 0) : flinearcolor(0, 1, 0);

		myhud->drawline(out.x, out.y, width / 2.f, height - 10, color, 1.f);
	}

	return reinterpret_cast<void(*)(AHUD*)>(o_post_render)(myhud);
}

void update_entity()
{
	while (true)
	{
		gnames = decrypt.xenuine(read<uint64_t>(decrypt.xenuine(read<uint64_t>(base + offset::gnames))));
		uworld = decrypt.xenuine(read<uint64_t>(base + offset::uworld));
		uint32_t entity_count = 0;

		if (uworld)
		{
			uint64_t ulevel = decrypt.xenuine(read<uint64_t>(uworld + offset::persistent_level));
			uint64_t localplayer = read<uint64_t>(base + offset::local_player);

			if (ulevel && localplayer)
			{
				uint64_t controller = decrypt.xenuine(read<uint64_t>(localplayer + offset::controller));
				localplayer_pawn = decrypt.xenuine(read<uint64_t>(controller + offset::pawn));
				uint64_t* myhud = read<uint64_t*>(controller + offset::myhud);

				if (!o_post_render && myhud)
				{
					hook::vmt(myhud, post_render, 0xE0, &o_post_render);
				}

				if (controller && localplayer_pawn)
				{
					camera_manager = read<uint64_t>(controller + offset::camera_manager);

					actors_tarray = decrypt.xenuine(read<uint64_t>(ulevel + offset::actors));
					uint64_t actors = read<uint64_t>(actors_tarray);
					uint32_t actors_count = read<uint32_t>(actors_tarray + 0x8);

					for (uint32_t i = 0; i < actors_count; i++)
					{
						uint64_t actor = read<uint64_t>(actors + i * 8ui64);
						if (!actor || actor == localplayer_pawn) continue;

						uint32_t actor_id = decrypt.index(read<uint32_t>(actor + offset::object_id));
						if (!actor_id) continue;

						std::string gname = get_gname_by_id(actor_id);
						if (gname.length() < 3) continue;

						if (gname == xorstr_("PlayerFemale_A_C") || gname == xorstr_("PlayerMale_A_C") || gname == xorstr_("RegistedPlayer") ||
							gname == xorstr_("AIPawn_Base_C") || gname == xorstr_("AIPawn_Base_Female_C") || gname == xorstr_("AIPawn_Base_Female_C"))
						{
							entities[entity_count] = actor;
							entity_count++;
						}
					}
				}
			}
		}

		::entity_count = entity_count;

		Sleep(500);
	}
}

void main_thread() 
{
	/*if (AllocConsole())
	{
		freopen(xorstr_("CONIN$"), "r", stdin);
		freopen(xorstr_("CONOUT$"), "w", stdout);
		freopen(xorstr_("CONOUT$"), "w", stderr);
	}*/

	if (!decrypt.init(base))
	{
		return;
	}

	create_thread(update_entity);
}

BOOL APIENTRY DllMain(HINSTANCE h_module, DWORD dw_reason, LPVOID lpv_reserved) 
{
	if (dw_reason == DLL_PROCESS_ATTACH)
	{
		main_thread();
	}

	return TRUE;
}