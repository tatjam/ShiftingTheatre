#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include <vector>




#include "Game/Tilemap.h"
#include "Game/Player.h"
#include "Game/EnemyManager.h"

#include "Math/Geometry.h"

#define FPS 60


#define SCALE 4.0f

void do_end();

ALLEGRO_BITMAP* load_bitmap(string path)
{
	ALLEGRO_BITMAP* n = al_load_bitmap(path.c_str());

	if (n == NULL)
	{
		printf("ERROR LOADING ASSET: %s", path.c_str());
		abort();
	}

	return n;
}

void load_resources(std::map<string, ALLEGRO_BITMAP*> resources)
{
	printf("Loading, working directory: %s", al_get_current_directory());

	resources = std::map<string, ALLEGRO_BITMAP*>();


	resources.insert(std::make_pair("test_tile", load_bitmap("Resource/test_tile.png")));
}

//Places the player at the spawn pixel (100, 255, 255)
void place_player(Player* player, ALLEGRO_BITMAP* map)
{
	for (int x = 0; x < al_get_bitmap_width(map); x++)
	{
		for (int y = 0; y < al_get_bitmap_height(map); y++)
		{
			ALLEGRO_COLOR c = al_get_pixel(map, x, y);
			unsigned char r, g, b;
			al_unmap_rgb(c, &r, &g, &b);

			if (r == 100 && g == 255 && b == 255)
			{
				player->x = x * 8;
				player->y = y * 8;
				break;
			}
		}
	}
}

int main()
{

	printf("CLOSING THIS CONSOLE WILL END THE GAME\n");
	printf("======================================\n");
	printf("Shifting Theatre by Tajamsoft for LDJAM\n");
	printf("Version 0.1.1 (Small bugfix):\n");
	printf(" - Fixed ending not showing  \n\n");
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_DISPLAY *display = NULL;


	if (!al_init() || !al_init_primitives_addon() || !al_init_font_addon() || !al_init_image_addon()
		|| !al_install_audio() || !al_init_acodec_addon()) {
		fprintf(stderr, "Failed to initialize allegro!\n");
		return -1;
	}

	if (!al_reserve_samples(8))
	{
		fprintf(stderr, "Failde to reserve audio samples!\n");
		return -1;
	}

	if (!al_install_mouse())
	{
		fprintf(stderr, "Failed to initialize mouse!\n");
		return -1;
	}


	Vector2 screen_size = Vector2(170 * SCALE, 170 * SCALE);

	display = al_create_display(screen_size.x, screen_size.y);

	if (!display) {
		fprintf(stderr, "Failed to create display!\n");
		return -1;
	}


	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}

	al_install_keyboard();

	al_register_event_source(event_queue, al_get_display_event_source(display));



	ALLEGRO_FONT* font = al_create_builtin_font();

	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
	al_start_timer(timer);

	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_set_target_bitmap(al_get_backbuffer(display));


	bool need_redraw = false;



	//Resource loading
	//-----------------------------------------------------------------------------------------------
	ALLEGRO_BITMAP* test_tile_bm = al_load_bitmap("./Resource/test_tile.png");
	ALLEGRO_BITMAP* test_tile_2_bm = al_load_bitmap("./Resource/test_tile_2.png");
	ALLEGRO_BITMAP* player_idle = al_load_bitmap("./Resource/player_test.png");
	ALLEGRO_BITMAP* test_tile_bg_bm = al_load_bitmap("./Resource/test_tile_bg.png");
	ALLEGRO_BITMAP* platform_bm = al_load_bitmap("./Resource/platform.png");
	ALLEGRO_BITMAP* lava_bm = al_load_bitmap("./Resource/lava.png");
	ALLEGRO_BITMAP* lava_u_bm = al_load_bitmap("./Resource/lava_u.png");


	ALLEGRO_BITMAP* walljump_bm_f1 = al_load_bitmap("./Resource/wallj/f1.png");
	ALLEGRO_BITMAP* walljump_bm_f2 = al_load_bitmap("./Resource/wallj/f2.png");
	ALLEGRO_BITMAP* walljump_bm_f3 = al_load_bitmap("./Resource/wallj/f3.png");
	ALLEGRO_BITMAP* walljump_bm_f4 = al_load_bitmap("./Resource/wallj/f4.png");
	ALLEGRO_BITMAP* walljump_bm_f5 = al_load_bitmap("./Resource/wallj/f5.png");

	ALLEGRO_BITMAP* player_run_bm_f1 = al_load_bitmap("./Resource/player_test_run1.png");
	ALLEGRO_BITMAP* player_run_bm_f2 = al_load_bitmap("./Resource/player_test_run2.png");
	ALLEGRO_BITMAP* player_run_bm_f3 = al_load_bitmap("./Resource/player_test_run3.png");
	ALLEGRO_BITMAP* player_run_bm_f4 = al_load_bitmap("./Resource/player_test_run4.png");
	ALLEGRO_BITMAP* player_run_bm_f5 = al_load_bitmap("./Resource/player_test_run5.png");

	ALLEGRO_BITMAP* player_hang = al_load_bitmap("./Resource/player_hang_left.png");

	ALLEGRO_BITMAP* enemy_test_bm_f1 = al_load_bitmap("./Resource/enemy_test.png");
	ALLEGRO_BITMAP* enemy_test_bm_f2 = al_load_bitmap("./Resource/enemy_test_f2.png");
	ALLEGRO_BITMAP* enemy_test_bm_f3 = al_load_bitmap("./Resource/enemy_test_f3.png");

	ALLEGRO_BITMAP* enemy_bomb_bm_f1 = al_load_bitmap("./Resource/enemy_bomb.png");
	ALLEGRO_BITMAP* enemy_bomb_bm_f2 = al_load_bitmap("./Resource/enemy_bomb_f2.png");
	ALLEGRO_BITMAP* enemy_bomb_bm_f3 = al_load_bitmap("./Resource/enemy_bomb_f3.png");
	ALLEGRO_BITMAP* enemy_bomb_bm_ex = al_load_bitmap("./Resource/enemy_bomb_explode.png");
	ALLEGRO_BITMAP* enemy_bomb_bm_ball = al_load_bitmap("./Resource/boom.png");

	ALLEGRO_BITMAP* enemy_thrower_bm_f1 = al_load_bitmap("./Resource/enemy_thrower.png");
	ALLEGRO_BITMAP* enemy_thrower_bm_f2 = al_load_bitmap("./Resource/enemy_thrower_f2.png");
	ALLEGRO_BITMAP* enemy_thrower_bm_f3 = al_load_bitmap("./Resource/enemy_thrower_f3.png");
	ALLEGRO_BITMAP* enemy_thrower_bm_kn = al_load_bitmap("./Resource/flying_blade.png");
	ALLEGRO_BITMAP* enemy_thrower_bm_kn_f1 = al_load_bitmap("./Resource/flying_blade_f1.png");
	ALLEGRO_BITMAP* enemy_thrower_bm_kn_f2 = al_load_bitmap("./Resource/flying_blade_f2.png");
	ALLEGRO_BITMAP* enemy_thrower_bm_kn_f3 = al_load_bitmap("./Resource/flying_blade_f3.png");

	ALLEGRO_BITMAP* tile_door = al_load_bitmap("./Resource/door.png");

	ALLEGRO_BITMAP* fireball_bm = al_load_bitmap("./Resource/fireball.png");
	ALLEGRO_BITMAP* glow_bm = al_load_bitmap("./Resource/glow.png");

	ALLEGRO_BITMAP* sheet = al_load_bitmap("./Resource/sheet.png");

	ALLEGRO_BITMAP* underground_drop = al_load_bitmap("./Resource/backdrop.png");

	ALLEGRO_BITMAP* mini_ball_bm = al_load_bitmap("./Resource/mini_fireball.png");
	ALLEGRO_BITMAP* spinner = al_load_bitmap("./Resource/spinner.png");
	
	ALLEGRO_BITMAP* cannon = al_load_bitmap("./Resource/cannon.png");
	ALLEGRO_BITMAP* cannon_ball = al_load_bitmap("./Resource/cannon_ball.png");

	ALLEGRO_SAMPLE* player_jump = al_load_sample("./Resource/Jump.wav");
	ALLEGRO_SAMPLE* player_wjump = al_load_sample("./Resource/WJump.wav");

	ALLEGRO_SAMPLE* fireball = al_load_sample("./Resource/Fireball.wav");

	ALLEGRO_SAMPLE* curtain_open = al_load_sample("./Resource/CurtainOpen.wav");
	ALLEGRO_SAMPLE* curtain_close = al_load_sample("./Resource/CurtainClose.wav");

	ALLEGRO_SAMPLE* working = al_load_sample("./Resource/Working.wav");

	ALLEGRO_SAMPLE* fuse = al_load_sample("./Resource/Fuse.wav");
	ALLEGRO_SAMPLE* explosion = al_load_sample("./Resource/Explosion.wav");

	ALLEGRO_SAMPLE* kill = al_load_sample("./Resource/Kill.wav");
	ALLEGRO_SAMPLE* hurt = al_load_sample("./Resource/Hurt.wav");


	ALLEGRO_SAMPLE* music_a = al_load_sample("./Resource/nes_underground.wav");
	ALLEGRO_SAMPLE* music_b = al_load_sample("./Resource/nes_underground_tension.wav");
	ALLEGRO_SAMPLE* music_menu = al_load_sample("./Resource/epicprog_ss.wav");

	ALLEGRO_SAMPLE* cannon_shoot = al_load_sample("./Resource/CannonShoot.wav");


	int level_pointer = 0;
	int max_level = 5;

	int deaths = 0;

	std::vector<string> levels = std::vector<string>();
	
	//Load our levels
	levels.push_back("./Resource/level/0.png");
	levels.push_back("./Resource/level/1.png");
	levels.push_back("./Resource/level/2.png");
	levels.push_back("./Resource/level/3.png");
	levels.push_back("./Resource/level/4.png");
	levels.push_back("./Resource/level/5.png");

	//Tilemap
	//-----------------------------------------------------------------------------------------------
	Tilemap test_tilemap = Tilemap(display, 8);

	test_tilemap.load_file(levels[level_pointer]);
	
	//Replaces black
	Tile ground = Tile(test_tile_bm, true, al_map_rgb(0, 0, 0));
	//Replaces gray
	Tile ground_2 = Tile(test_tile_2_bm, true, al_map_rgb(128, 128, 128));
	//Replaces white
	Tile air = Tile(NULL, false, al_map_rgb(255, 255, 255));
	//Replaces ~pink
	Tile back = Tile(test_tile_bg_bm, false, al_map_rgb(182, 170, 188));

	Tile door = Tile(tile_door, false, al_map_rgb(255, 255, 0));

	Tile lava = Tile(lava_bm, false, al_map_rgb(255, 0, 0));
	Tile lava_u = Tile(lava_u_bm, false, al_map_rgb(200, 0, 0));

	test_tilemap.add_tile(ground); test_tilemap.add_tile(ground_2); test_tilemap.add_tile(air);
	test_tilemap.add_tile(back); test_tilemap.add_tile(lava); test_tilemap.add_tile(lava_u);
	test_tilemap.add_tile(door);

	test_tilemap.platform_manager.platform_bm = platform_bm;

	//Player
	//-----------------------------------------------------------------------------------------------
	
	Player player = Player();
	player.player_bit = player_idle;
	player.walljump_bm_f1 = walljump_bm_f1;
	player.walljump_bm_f2 = walljump_bm_f2;
	player.walljump_bm_f3 = walljump_bm_f3;
	player.walljump_bm_f4 = walljump_bm_f4;
	player.walljump_bm_f5 = walljump_bm_f5;

	player.player_run_bm_f1 = player_run_bm_f1;
	player.player_run_bm_f2 = player_run_bm_f2;
	player.player_run_bm_f3 = player_run_bm_f3;
	player.player_run_bm_f4 = player_run_bm_f4;
	player.player_run_bm_f5 = player_run_bm_f5;
	
	player.player_hang = player_hang;

	player.jump = player_jump;
	player.wjump = player_wjump;

	player.hurt = hurt;


	//Loop
	//-----------------------------------------------------------------------------------------------
	unsigned int ticks = 0;

	float diffX = 0.0f;
	float diffY = 0.0f;

	//al_convert_memory_bitmaps();

	test_tilemap.render(SCALE, display, platform_bm);

	place_player(&player, test_tilemap.map);


	BasicEnemy enemy = BasicEnemy();

	JumpingLava j_lava = JumpingLava();

	std::vector<ALLEGRO_BITMAP*> enemy_test_v = std::vector<ALLEGRO_BITMAP*>();
	enemy_test_v.push_back(enemy_test_bm_f1);
	enemy_test_v.push_back(enemy_test_bm_f2);
	enemy_test_v.push_back(enemy_test_bm_f3);

	std::vector<ALLEGRO_BITMAP*> jumping_lava_v = std::vector<ALLEGRO_BITMAP*>();
	jumping_lava_v.push_back(fireball_bm);
	jumping_lava_v.push_back(glow_bm);

	std::vector<ALLEGRO_BITMAP*> enemy_bomb_v = std::vector<ALLEGRO_BITMAP*>();
	enemy_bomb_v.push_back(enemy_bomb_bm_f1);
	enemy_bomb_v.push_back(enemy_bomb_bm_f2);
	enemy_bomb_v.push_back(enemy_bomb_bm_f3);
	enemy_bomb_v.push_back(enemy_bomb_bm_ex);
	enemy_bomb_v.push_back(enemy_bomb_bm_ball);

	std::vector<ALLEGRO_BITMAP*> enemy_thrower_v = std::vector<ALLEGRO_BITMAP*>();
	enemy_thrower_v.push_back(enemy_thrower_bm_f1);
	enemy_thrower_v.push_back(enemy_thrower_bm_f2);
	enemy_thrower_v.push_back(enemy_thrower_bm_kn);
	enemy_thrower_v.push_back(enemy_thrower_bm_kn_f1);
	enemy_thrower_v.push_back(enemy_thrower_bm_kn_f2);
	enemy_thrower_v.push_back(enemy_thrower_bm_kn_f3);


	std::vector<ALLEGRO_SAMPLE*> enemy_test_sv = std::vector<ALLEGRO_SAMPLE*>();
	enemy_test_sv.push_back(kill);

	std::vector<ALLEGRO_SAMPLE*> jumping_lava_sv = std::vector<ALLEGRO_SAMPLE*>();
	jumping_lava_sv.push_back(fireball);

	std::vector<ALLEGRO_SAMPLE*> enemy_bomb_sv = std::vector<ALLEGRO_SAMPLE*>();
	enemy_bomb_sv.push_back(fuse);
	enemy_bomb_sv.push_back(explosion);
	enemy_bomb_sv.push_back(kill);

	std::vector<ALLEGRO_SAMPLE*> enemy_thrower_sv = std::vector<ALLEGRO_SAMPLE*>();
	enemy_thrower_sv.push_back(kill);
	
	std::vector<ALLEGRO_BITMAP*> enemy_spinner_v = std::vector<ALLEGRO_BITMAP*>();
	enemy_spinner_v.push_back(spinner);
	enemy_spinner_v.push_back(mini_ball_bm);

	std::vector<ALLEGRO_BITMAP*> enemy_cannon_v = std::vector<ALLEGRO_BITMAP*>();
	enemy_cannon_v.push_back(cannon);
	enemy_cannon_v.push_back(cannon_ball);
	enemy_cannon_v.push_back(enemy_bomb_bm_ball);

	std::vector<ALLEGRO_SAMPLE*> enemy_cannon_sv = std::vector<ALLEGRO_SAMPLE*>();
	enemy_cannon_sv.push_back(cannon_shoot);
	enemy_cannon_sv.push_back(explosion);

	EnemyManager enemy_manager = EnemyManager();
	enemy_manager.give_sprites(&enemy_test_v, &jumping_lava_v, &enemy_bomb_v, &enemy_thrower_v, &enemy_spinner_v, &enemy_cannon_v);
	enemy_manager.give_samples(&enemy_test_sv, &jumping_lava_sv, &enemy_bomb_sv, &enemy_thrower_sv, &enemy_cannon_sv);
	enemy_manager.setup(test_tilemap.map, 8);

	place_player(&player, test_tilemap.map);


	bool in_anim = false;
	bool in_out_anim = false;

	int anim_frames = 0;

	float sheet1_x = -512.0f;
	float sheet2_x = 512.0f;

	bool sound_played = false;
	printf("Starting game loop \n");

	int frame = 0;

	ALLEGRO_SAMPLE_ID m_id;

	al_play_sample(music_menu, 0.7f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, &m_id);

	ALLEGRO_BITMAP* menu = al_load_bitmap("./Resource/menu.png");

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_scaled_bitmap(menu, 0, 0, 170, 170, 0, 0, 170 * SCALE, 170 * SCALE, 0);
	al_flip_display();

	al_rest(12.5f);

	al_stop_sample(&m_id);

	al_play_sample(music_a, 0.7f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_LOOP, &m_id);

	bool tension_playing = false;

	while (1)
	{

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		/* handle input events */

		if (event.type == ALLEGRO_EVENT_TIMER) 
		{
			player.update(test_tilemap);

			test_tilemap.update();



			if (player.finished)
			{
				//Load new level:
				if (player.finished_f)
				{
					if (player.died == false)
					{
						level_pointer++;
						if (level_pointer > max_level / 1.5)
						{
							//Tension music
							if (!tension_playing)
							{
								al_stop_sample(&m_id);
								tension_playing = true;
								al_play_sample(music_b, 0.7f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_LOOP, &m_id);
							}
						}
					}
					else
					{
						deaths++;
					}
					player.finished_f = false;
				}
				if (level_pointer > max_level)
				{
					//TODO FINISH GAME
					do_end();
					return 0;

					//Finished :)
				}
				else
				{
					in_anim = true;
					anim_frames++;
					if (anim_frames >= 40)
					{
						in_anim = false;
						ALLEGRO_SAMPLE_ID id;
						if (!player.died)
						{
							
							al_play_sample(working, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_LOOP, &id);


							test_tilemap.load_file(levels[level_pointer]);
							test_tilemap.render(SCALE, display, NULL);
						}
						enemy_manager.clear();
						enemy_manager.setup(test_tilemap.map, 8);
						place_player(&player, test_tilemap.map);
						player.finished = false;


						in_out_anim = true;

						diffY = 0;

						anim_frames = 0;

						sound_played = false;

						if (!player.died)
						{
							al_stop_sample(&id);
						}

						player.died = false;
					}
					else
					{
						if (!sound_played)
						{
							al_play_sample(curtain_close, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
							sound_played = true;
						}
						//Lower anim
						sheet1_x += 6.6f;
						sheet2_x -= 6.6f;
					}
					
				}
			}

			if (in_out_anim)
			{
				anim_frames++;
				if (anim_frames >= 40)
				{
					in_out_anim = false;
					anim_frames = 0;
					sheet1_x = -512.0f;
					sheet2_x = 512.0f;

					sound_played = false;
				}
				else
				{
					if (!sound_played)
					{
						al_play_sample(curtain_open, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
						sound_played = true;
					}
					//Upper anim
					sheet1_x -= 6.6f;
					sheet2_x += 6.6f;
				}
			}

			enemy_manager.update(&player, &test_tilemap, ticks);

			diffX = player.x - 64;

			//Will only update if player is out of the screen
			if (player.y > (screen_size.y / SCALE - 30) + diffY)
			{
				diffY = player.y - 84;
			}

			if (player.y < 0 + diffY + 10)
			{
				diffY = player.y - 148;
			}

			need_redraw = true;
			ticks++;
		}

		
		if (need_redraw && al_event_queue_is_empty(event_queue) || in_out_anim) 
		{

			al_clear_to_color(al_map_rgb(0, 0, 0));

			al_draw_scaled_bitmap(underground_drop, 0.0f, 0.0f, 180, 180, 0, 0, 180 * SCALE, 180 * SCALE, 0);

			test_tilemap.draw(diffX, diffY, SCALE, screen_size.x, screen_size.y);

			player.render(SCALE, font, diffX, diffY);

			enemy_manager.draw(diffX, diffY, SCALE);

			if (in_anim == true || in_out_anim)
			{
				al_draw_scaled_bitmap(sheet, 0, 0, 200, 200, sheet1_x, 0.0f, 200 * SCALE, 200 * SCALE, 0);
				al_draw_scaled_bitmap(sheet, 0, 0, 200, 200, sheet2_x, 0.0f, 200 * SCALE, 200 * SCALE, ALLEGRO_FLIP_HORIZONTAL);
			}

			//DRAW HUD

			al_draw_filled_rectangle(0, (screen_size.y - 10 * SCALE), screen_size.x, screen_size.y, al_map_rgb(0, 0, 0));

			al_draw_textf(font, al_map_rgb(255, 255, 255), 5, (screen_size.y - 8 * SCALE), 0, "HEALTH: %i", player.health);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 5, (screen_size.y - 4 * SCALE), 0, "LEVEL: %i", level_pointer + 1);
			al_draw_textf(font, al_map_rgb(255, 255, 255), 40 * SCALE, (screen_size.y - 6 * SCALE), 0, "DEATHS: %i", deaths);

			al_flip_display();

			need_redraw = false;
		}

	}

	al_destroy_display(display);

	return 0;

}

void do_end()
{
	ALLEGRO_BITMAP* ending = al_load_bitmap("./Resource/ending.png");
	al_draw_scaled_bitmap(ending, 0, 0, 340, 340, 0, 0, 340 * (SCALE / 2), 340 * (SCALE / 2), 0);
	al_flip_display();
	al_rest(20);
}