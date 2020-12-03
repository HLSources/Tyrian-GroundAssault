#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "bot.h"

#ifdef _WIN32
extern HINSTANCE h_Library;
#else
extern void *h_Library;
#endif

#define LINK_ENTITY_TO_FUNC(mapClassName)\
extern "C" EXPORT void mapClassName(entvars_t *pev) {\
	static LINK_ENTITY_FUNC otherClassName = NULL;\
	static int skip_this = 0;\
	if (skip_this) return;\
	if (otherClassName == NULL)\
		otherClassName = (LINK_ENTITY_FUNC)GetProcAddress(h_Library, #mapClassName);\
	if (otherClassName == NULL) {\
		printf("XBM: cannot init '%s'\n", #mapClassName);\
		skip_this = 1; return;\
	}\
	(*otherClassName)(pev); }

LINK_ENTITY_TO_FUNC(LightningBlast);
LINK_ENTITY_TO_FUNC(LightningBall);
LINK_ENTITY_TO_FUNC(AcidBlob);
LINK_ENTITY_TO_FUNC(StarBurst);
LINK_ENTITY_TO_FUNC(LightningField);
LINK_ENTITY_TO_FUNC(AtomBomb);
LINK_ENTITY_TO_FUNC(RingTeleport);
LINK_ENTITY_TO_FUNC(FrozenCube);
LINK_ENTITY_TO_FUNC(DisruptorBall);
LINK_ENTITY_TO_FUNC(Ripper);
LINK_ENTITY_TO_FUNC(strtarget);
LINK_ENTITY_TO_FUNC(teleporttarget);
LINK_ENTITY_TO_FUNC(Toilet);
LINK_ENTITY_TO_FUNC(GuidedBomb);
LINK_ENTITY_TO_FUNC(laser_spot);
LINK_ENTITY_TO_FUNC(HVRMissile);
LINK_ENTITY_TO_FUNC(GhostMissile);
LINK_ENTITY_TO_FUNC(DemolitionMissile);
LINK_ENTITY_TO_FUNC(ChargeCannon);
LINK_ENTITY_TO_FUNC(SonicWave);
LINK_ENTITY_TO_FUNC(MicroMissile);
LINK_ENTITY_TO_FUNC(FrostBall);
LINK_ENTITY_TO_FUNC(SunOfGod);
LINK_ENTITY_TO_FUNC(ShockWave);
LINK_ENTITY_TO_FUNC(Teleporter);
LINK_ENTITY_TO_FUNC(AntimatherialMissile);
LINK_ENTITY_TO_FUNC(NuclearMissile);
LINK_ENTITY_TO_FUNC(ProtonMissile);
LINK_ENTITY_TO_FUNC(PlasmaShieldCharge);

LINK_ENTITY_TO_FUNC(BioMissile);
LINK_ENTITY_TO_FUNC(MiniMissile);
LINK_ENTITY_TO_FUNC(HellHounder);
LINK_ENTITY_TO_FUNC(DelayedUse);
LINK_ENTITY_TO_FUNC(m203grenade);
LINK_ENTITY_TO_FUNC(GluonBall);
LINK_ENTITY_TO_FUNC(Trident);
LINK_ENTITY_TO_FUNC(Scorcher);
LINK_ENTITY_TO_FUNC(PlasmaBall);
LINK_ENTITY_TO_FUNC(PulseCannon);
LINK_ENTITY_TO_FUNC(MultiCannon);
LINK_ENTITY_TO_FUNC(PlasmaStorm);
LINK_ENTITY_TO_FUNC(Bomb);
LINK_ENTITY_TO_FUNC(HellFire);
LINK_ENTITY_TO_FUNC(FlashBang);
LINK_ENTITY_TO_FUNC(aiscripted_sequence);
LINK_ENTITY_TO_FUNC(ambient_generic);
LINK_ENTITY_TO_FUNC(beam);
LINK_ENTITY_TO_FUNC(bmortar);
LINK_ENTITY_TO_FUNC(bodyque);
LINK_ENTITY_TO_FUNC(button_target);
LINK_ENTITY_TO_FUNC(cycler);
LINK_ENTITY_TO_FUNC(cycler_prdroid);
LINK_ENTITY_TO_FUNC(cycler_sprite);
LINK_ENTITY_TO_FUNC(cycler_weapon);
LINK_ENTITY_TO_FUNC(cycler_wreckage);
LINK_ENTITY_TO_FUNC(entity_chasecam);
LINK_ENTITY_TO_FUNC(env_ammodispenser);
LINK_ENTITY_TO_FUNC(env_beam);
LINK_ENTITY_TO_FUNC(env_beamtrail);
LINK_ENTITY_TO_FUNC(env_beverage);
LINK_ENTITY_TO_FUNC(env_blood);
LINK_ENTITY_TO_FUNC(env_bubbles);
LINK_ENTITY_TO_FUNC(env_cache);
LINK_ENTITY_TO_FUNC(env_debris);
LINK_ENTITY_TO_FUNC(env_decal);
LINK_ENTITY_TO_FUNC(env_dlight);
LINK_ENTITY_TO_FUNC(env_elight);
LINK_ENTITY_TO_FUNC(env_explosion);
LINK_ENTITY_TO_FUNC(env_fade);
LINK_ENTITY_TO_FUNC(env_flamespawner);
LINK_ENTITY_TO_FUNC(env_fog);
LINK_ENTITY_TO_FUNC(env_fogzone);
LINK_ENTITY_TO_FUNC(env_fountain);
LINK_ENTITY_TO_FUNC(env_funnel);
LINK_ENTITY_TO_FUNC(env_global);
LINK_ENTITY_TO_FUNC(env_glow);
LINK_ENTITY_TO_FUNC(env_gravitypoint);
LINK_ENTITY_TO_FUNC(env_laser);
LINK_ENTITY_TO_FUNC(env_lightatt);
LINK_ENTITY_TO_FUNC(env_lightning);
LINK_ENTITY_TO_FUNC(env_message);
LINK_ENTITY_TO_FUNC(env_model);
LINK_ENTITY_TO_FUNC(env_projectile);
LINK_ENTITY_TO_FUNC(env_rain);
LINK_ENTITY_TO_FUNC(env_render);
LINK_ENTITY_TO_FUNC(env_rotparticles);
LINK_ENTITY_TO_FUNC(env_shake);
LINK_ENTITY_TO_FUNC(env_shockwave);
LINK_ENTITY_TO_FUNC(env_shooter);
LINK_ENTITY_TO_FUNC(env_proj_shooter);
LINK_ENTITY_TO_FUNC(env_sky);
LINK_ENTITY_TO_FUNC(env_smoke);
LINK_ENTITY_TO_FUNC(env_smoker);
LINK_ENTITY_TO_FUNC(env_snow);
LINK_ENTITY_TO_FUNC(env_sound);
LINK_ENTITY_TO_FUNC(env_spark);
LINK_ENTITY_TO_FUNC(env_sprite);
LINK_ENTITY_TO_FUNC(env_state);
LINK_ENTITY_TO_FUNC(env_static);
LINK_ENTITY_TO_FUNC(env_sun);
LINK_ENTITY_TO_FUNC(env_teleporter);
LINK_ENTITY_TO_FUNC(env_warpball);
LINK_ENTITY_TO_FUNC(fireanddie);
LINK_ENTITY_TO_FUNC(func_breakable);
LINK_ENTITY_TO_FUNC(func_breakable_model);
LINK_ENTITY_TO_FUNC(func_button);
LINK_ENTITY_TO_FUNC(func_conveyor);
LINK_ENTITY_TO_FUNC(func_door);
LINK_ENTITY_TO_FUNC(func_door_rotating);
LINK_ENTITY_TO_FUNC(func_friction);
LINK_ENTITY_TO_FUNC(func_guntarget);
LINK_ENTITY_TO_FUNC(func_healthcharger);
LINK_ENTITY_TO_FUNC(func_illusionary);
LINK_ENTITY_TO_FUNC(func_ladder);
LINK_ENTITY_TO_FUNC(func_lava);
LINK_ENTITY_TO_FUNC(func_monsterclip);
LINK_ENTITY_TO_FUNC(func_mortar_field);
LINK_ENTITY_TO_FUNC(func_pendulum);
LINK_ENTITY_TO_FUNC(func_plat);
LINK_ENTITY_TO_FUNC(func_platrot);
LINK_ENTITY_TO_FUNC(func_pushable);
LINK_ENTITY_TO_FUNC(func_recharge);
LINK_ENTITY_TO_FUNC(func_rot_button);
LINK_ENTITY_TO_FUNC(func_rotating);
LINK_ENTITY_TO_FUNC(func_tank);
LINK_ENTITY_TO_FUNC(func_tankcontrols);
LINK_ENTITY_TO_FUNC(func_tanklaser);
LINK_ENTITY_TO_FUNC(func_tankmortar);
LINK_ENTITY_TO_FUNC(func_tankrocket);
LINK_ENTITY_TO_FUNC(func_trackautochange);
LINK_ENTITY_TO_FUNC(func_trackchange);
LINK_ENTITY_TO_FUNC(func_tracktrain);
LINK_ENTITY_TO_FUNC(func_train);
LINK_ENTITY_TO_FUNC(func_wall);
LINK_ENTITY_TO_FUNC(func_wall_computer);
LINK_ENTITY_TO_FUNC(func_wall_toggle);
LINK_ENTITY_TO_FUNC(func_water);
LINK_ENTITY_TO_FUNC(game_counter);
LINK_ENTITY_TO_FUNC(game_counter_set);
LINK_ENTITY_TO_FUNC(game_end);
LINK_ENTITY_TO_FUNC(game_player_equip);
LINK_ENTITY_TO_FUNC(game_player_hurt);
LINK_ENTITY_TO_FUNC(game_player_team);
LINK_ENTITY_TO_FUNC(game_score);
LINK_ENTITY_TO_FUNC(game_team_master);
LINK_ENTITY_TO_FUNC(game_team_set);
LINK_ENTITY_TO_FUNC(game_text);
LINK_ENTITY_TO_FUNC(game_zone_player);
LINK_ENTITY_TO_FUNC(gib);
LINK_ENTITY_TO_FUNC(gibshooter);
LINK_ENTITY_TO_FUNC(grenade);
LINK_ENTITY_TO_FUNC(hvr_rocket);
LINK_ENTITY_TO_FUNC(info_bigmomma);
LINK_ENTITY_TO_FUNC(info_capture_obj);
LINK_ENTITY_TO_FUNC(info_ctfdetect);// Opposing Force CTF compatibility
LINK_ENTITY_TO_FUNC(info_ctfspawn);// Opposing Force CTF compatibility
LINK_ENTITY_TO_FUNC(info_dom_target);
LINK_ENTITY_TO_FUNC(info_intermission);
LINK_ENTITY_TO_FUNC(info_landmark);
LINK_ENTITY_TO_FUNC(info_node);
LINK_ENTITY_TO_FUNC(info_node_air);
LINK_ENTITY_TO_FUNC(info_player_deathmatch);
LINK_ENTITY_TO_FUNC(info_player_start);
LINK_ENTITY_TO_FUNC(info_player_team1);// AGCTF compatibility
LINK_ENTITY_TO_FUNC(info_player_team2);// AGCTF compatibility
LINK_ENTITY_TO_FUNC(info_target);
LINK_ENTITY_TO_FUNC(info_teleport_destination);
LINK_ENTITY_TO_FUNC(infodecal);
LINK_ENTITY_TO_FUNC(item_flare);
LINK_ENTITY_TO_FUNC(item_healthkit);
LINK_ENTITY_TO_FUNC(item_airtank);
LINK_ENTITY_TO_FUNC(item_antidote);
LINK_ENTITY_TO_FUNC(item_fire_supressor);
LINK_ENTITY_TO_FUNC(item_battery);
LINK_ENTITY_TO_FUNC(item_ctfflag);

LINK_ENTITY_TO_FUNC(item_airstrike);
LINK_ENTITY_TO_FUNC(item_haste);
LINK_ENTITY_TO_FUNC(item_rapidfire);
LINK_ENTITY_TO_FUNC(item_quaddamage);
LINK_ENTITY_TO_FUNC(item_invisibility);
LINK_ENTITY_TO_FUNC(item_invulnerability);
LINK_ENTITY_TO_FUNC(item_shield_aug);
LINK_ENTITY_TO_FUNC(item_health_aug);
LINK_ENTITY_TO_FUNC(item_shield_regeneration);
LINK_ENTITY_TO_FUNC(item_accuracy);
LINK_ENTITY_TO_FUNC(item_generator_aug);
LINK_ENTITY_TO_FUNC(item_superweapon);
LINK_ENTITY_TO_FUNC(item_banana);
LINK_ENTITY_TO_FUNC(item_tripmine);
LINK_ENTITY_TO_FUNC(item_spidermine);
LINK_ENTITY_TO_FUNC(item_shield_strength_aug);
LINK_ENTITY_TO_FUNC(item_energy_cube);
LINK_ENTITY_TO_FUNC(item_lightning_field);
LINK_ENTITY_TO_FUNC(item_satellite_strike);
LINK_ENTITY_TO_FUNC(item_flashbang);
LINK_ENTITY_TO_FUNC(item_radshield);
LINK_ENTITY_TO_FUNC(item_plasma_shield);
LINK_ENTITY_TO_FUNC(item_weapon_power_aug);

LINK_ENTITY_TO_FUNC(item_flag_team1);// AGCTF compatibility
LINK_ENTITY_TO_FUNC(item_flag_team2);// AGCTF compatibility
LINK_ENTITY_TO_FUNC(item_lambda);
LINK_ENTITY_TO_FUNC(item_longjump);
LINK_ENTITY_TO_FUNC(item_security);
LINK_ENTITY_TO_FUNC(item_sodacan);
LINK_ENTITY_TO_FUNC(item_suit);
LINK_ENTITY_TO_FUNC(Grenade30mm);
LINK_ENTITY_TO_FUNC(lava_ball);
LINK_ENTITY_TO_FUNC(meteor);
LINK_ENTITY_TO_FUNC(light);
LINK_ENTITY_TO_FUNC(light_environment);
LINK_ENTITY_TO_FUNC(light_spot);
LINK_ENTITY_TO_FUNC(NeedleLaser);
LINK_ENTITY_TO_FUNC(ShockLaser);
LINK_ENTITY_TO_FUNC(Banana);
LINK_ENTITY_TO_FUNC(momentary_door);
LINK_ENTITY_TO_FUNC(momentary_rot_button);
LINK_ENTITY_TO_FUNC(monster_furniture);
LINK_ENTITY_TO_FUNC(monster_generic);
LINK_ENTITY_TO_FUNC(monster_miniturret);
LINK_ENTITY_TO_FUNC(monster_tripmine);
LINK_ENTITY_TO_FUNC(monster_spider_mine);
LINK_ENTITY_TO_FUNC(monster_mortar);
LINK_ENTITY_TO_FUNC(monster_player);
LINK_ENTITY_TO_FUNC(monster_sentry);
LINK_ENTITY_TO_FUNC(monster_turret);
LINK_ENTITY_TO_FUNC(monstermaker);
LINK_ENTITY_TO_FUNC(multi_manager);
LINK_ENTITY_TO_FUNC(multi_switcher);// SHL compatibility
LINK_ENTITY_TO_FUNC(multi_watcher);// SHL compatibility
LINK_ENTITY_TO_FUNC(multisource);
LINK_ENTITY_TO_FUNC(node_viewer);
LINK_ENTITY_TO_FUNC(node_viewer_fly);
LINK_ENTITY_TO_FUNC(node_viewer_human);
LINK_ENTITY_TO_FUNC(node_viewer_large);
LINK_ENTITY_TO_FUNC(path_corner);
LINK_ENTITY_TO_FUNC(path_track);
LINK_ENTITY_TO_FUNC(player);
LINK_ENTITY_TO_FUNC(player_loadsaved);
LINK_ENTITY_TO_FUNC(player_weaponstrip);
LINK_ENTITY_TO_FUNC(scripted_sentence);
LINK_ENTITY_TO_FUNC(scripted_sequence);
LINK_ENTITY_TO_FUNC(soundent);
LINK_ENTITY_TO_FUNC(speaker);
LINK_ENTITY_TO_FUNC(streak_spiral);
LINK_ENTITY_TO_FUNC(target_cdaudio);
LINK_ENTITY_TO_FUNC(test_effect);
LINK_ENTITY_TO_FUNC(testhull);
LINK_ENTITY_TO_FUNC(trigger);
LINK_ENTITY_TO_FUNC(trigger_auto);
LINK_ENTITY_TO_FUNC(trigger_autosave);
LINK_ENTITY_TO_FUNC(trigger_bounce);
LINK_ENTITY_TO_FUNC(trigger_butthurt);
LINK_ENTITY_TO_FUNC(trigger_camera);
LINK_ENTITY_TO_FUNC(trigger_cap_point);
LINK_ENTITY_TO_FUNC(trigger_cdaudio);
LINK_ENTITY_TO_FUNC(trigger_changelevel);
LINK_ENTITY_TO_FUNC(trigger_changetarget);
LINK_ENTITY_TO_FUNC(trigger_counter);
LINK_ENTITY_TO_FUNC(trigger_dom_point);
LINK_ENTITY_TO_FUNC(trigger_endsection);
LINK_ENTITY_TO_FUNC(trigger_gravity);
LINK_ENTITY_TO_FUNC(trigger_hurt);
LINK_ENTITY_TO_FUNC(trigger_monsterjump);
LINK_ENTITY_TO_FUNC(trigger_multiple);
LINK_ENTITY_TO_FUNC(trigger_once);
LINK_ENTITY_TO_FUNC(trigger_onsight);
LINK_ENTITY_TO_FUNC(trigger_playerfreeze);// XDM3035c
LINK_ENTITY_TO_FUNC(trigger_push);
LINK_ENTITY_TO_FUNC(trigger_relay);
LINK_ENTITY_TO_FUNC(trigger_sound);
LINK_ENTITY_TO_FUNC(trigger_startpatrol);
LINK_ENTITY_TO_FUNC(trigger_teleport);
LINK_ENTITY_TO_FUNC(trigger_transition);
LINK_ENTITY_TO_FUNC(weapon_tyriangun);
LINK_ENTITY_TO_FUNC(worldspawn);
LINK_ENTITY_TO_FUNC(xen_hair);
LINK_ENTITY_TO_FUNC(xen_hull);
LINK_ENTITY_TO_FUNC(xen_plantlight);
LINK_ENTITY_TO_FUNC(xen_spore_large);
LINK_ENTITY_TO_FUNC(xen_spore_medium);
LINK_ENTITY_TO_FUNC(xen_spore_small);
LINK_ENTITY_TO_FUNC(xen_tree);
LINK_ENTITY_TO_FUNC(xen_ttrigger);
LINK_ENTITY_TO_FUNC(calc_position);
LINK_ENTITY_TO_FUNC(calc_ratio);
LINK_ENTITY_TO_FUNC(calc_subvelocity);
LINK_ENTITY_TO_FUNC(calc_velocity_path);
LINK_ENTITY_TO_FUNC(calc_velocity_polar);
LINK_ENTITY_TO_FUNC(env_particle);
LINK_ENTITY_TO_FUNC(locus_alias);
LINK_ENTITY_TO_FUNC(locus_beam);
LINK_ENTITY_TO_FUNC(locus_variable);
LINK_ENTITY_TO_FUNC(multi_alias);
LINK_ENTITY_TO_FUNC(info_alias);
LINK_ENTITY_TO_FUNC(info_group);
LINK_ENTITY_TO_FUNC(trigger_changealias);
