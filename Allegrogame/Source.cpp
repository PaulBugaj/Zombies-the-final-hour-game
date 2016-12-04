#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\allegro_image.h>
#include<allegro5\allegro_native_dialog.h>

int main(void)
{
	al_init();
	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	ALLEGRO_DISPLAY *display;
	bool keys[] = { false, false, false, false };
	enum KEYS { DOWN, LEFT, RIGHT, UP };
	const float FPS = 60.0;
	const int WIDTH = 1066;
	const int HEIGHT = 600;
	bool render = false;

	int xcam = 0;
	int ycam = 0;

	int mapColumns = 20;
	int mapSize = 400;
	int tileSize = 100;

	int map[] = { 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5,
				  1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
				  2, 2, 1, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5,
				  2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
				  2, 2, 1, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5,
				  2, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2,
				  2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2};

	if (!al_init())
		al_show_native_message_box(NULL, "ERROR", NULL, "Could not initialize Allegro", NULL, NULL);

	display = al_create_display(WIDTH, HEIGHT);

	if (!display)
		al_show_native_message_box(NULL, "ERROR", NULL, "Could not create display", NULL, NULL);

	al_set_window_position(display, 200, 200);

	bool done = false, draw = true, active = false;
	float x = 533, y = 300, moveSpeed = 1;
	int dir = DOWN, sourceX = 250, sourceY = 0;

	ALLEGRO_BITMAP *player = al_load_bitmap("postac.png");
	ALLEGRO_BITMAP *title = al_load_bitmap("zombies.png");
	ALLEGRO_BITMAP *map1 = NULL;
	map1 = al_load_bitmap("zombiemap.png");
	al_convert_mask_to_alpha(player, al_map_rgb(241, 228, 77));
	ALLEGRO_KEYBOARD_STATE keystate;

	ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();

	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	ALLEGRO_FONT *font18 = al_load_font("arial.ttf", 18, 0);

	al_start_timer(timer);

	al_draw_bitmap(title, 0, 0, 0);
	al_flip_display();
	al_rest(2.0);

	while (!done)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		al_get_keyboard_state(&keystate);
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = true;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = true;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = true;
				break;
			}
		}

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}

		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			case ALLEGRO_KEY_LEFT:
				keys[LEFT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				keys[RIGHT] = false;
				break;
			case ALLEGRO_KEY_UP:
				keys[UP] = false;
				break;
			case ALLEGRO_KEY_DOWN:
				keys[DOWN] = false;
				break;
			}
		}

		else if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			xcam -= keys[RIGHT] * 5;
			xcam += keys[LEFT] * 5;
			ycam -= keys[DOWN] * 5;
			ycam += keys[UP] * 5;

			render = true;
			active = true;
			if (al_key_down(&keystate, ALLEGRO_KEY_DOWN))
			{
				y += moveSpeed;
				dir = DOWN;
			}
			else if (al_key_down(&keystate, ALLEGRO_KEY_UP))
			{
				y -= moveSpeed;
				dir = UP;
			}
			else if (al_key_down(&keystate, ALLEGRO_KEY_RIGHT))
			{
				x += moveSpeed;
				dir = RIGHT;
			}
			else if (al_key_down(&keystate, ALLEGRO_KEY_LEFT))
			{
				x -= moveSpeed;
				dir = LEFT;
			}
			else
				active = false;
			if (active)
				sourceX += al_get_bitmap_width(player) / 15;
			else
				sourceX = 249;

			if (sourceX >= al_get_bitmap_width(player))
				sourceX = 0;
			sourceY = dir;
			draw = true;
		}

		if (render && al_is_event_queue_empty(event_queue))
		{
			render = false;
			for (int i = 0; i < mapSize; i++)
			{
				al_draw_bitmap_region(map1, tileSize * map[i], 0, tileSize, tileSize,
					xcam + tileSize * (i % mapColumns), ycam + tileSize * (i / mapColumns), 0);
			}

		}

		if (draw)
		{
			al_draw_bitmap_region(player, sourceX, sourceY * al_get_bitmap_height(player) / 4, 50, 50, x, y, NULL);
			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}
	}

	al_show_native_message_box(display, "Zombies: The Final Hour", "Do you really want to exit?", NULL, NULL, ALLEGRO_MESSAGEBOX_YES_NO);
	al_destroy_bitmap(title);
	al_destroy_bitmap(player);
	al_destroy_bitmap(map1);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_display(display);
	return 0;
}




