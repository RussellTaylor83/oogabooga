typedef enum EntityArchetype
{
	arch_nil = 0,
	arch_tank = 1,
	arch_edith = 2,
	arch_blossom = 3,
	arch_veg = 4,
	arch_building = 5,
} EntityArchetype;

typedef struct Entity
{
	bool is_valid;
	EntityArchetype arch;
	Vector2 pos;
} Entity;
#define MAX_ENTITY_COUNT 1024

typedef struct World
{
	Entity entities[MAX_ENTITY_COUNT];
} World;
World *world = 0;

Entity *entity_create()
{
	Entity *entity_found = 0;
	for (int i = 0; i < MAX_ENTITY_COUNT; i++)
	{
		Entity *existing_entity = &world->entities[i];
		if (!existing_entity->is_valid)
		{
			entity_found = existing_entity;
			break;
		}
	}
	assert(entity_found, "No more free entities!");
	entity_found->is_valid = true;
	return entity_found;
}

void entity_destroy(Entity *entity)
{
	memset(entity, 0, sizeof(Entity));
}

void setup_veg(Entity *en)
{
	en->arch = arch_veg;
}

int entry(int argc, char **argv)
{
	window.title = STR("Edith & Tank");
	window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 720;
	window.x = 200;
	window.y = 90;
	window.clear_color = hex_to_rgba(0xffffffff);

	world = alloc(get_heap_allocator(), sizeof(World));

	float64 last_time = os_get_current_time_in_seconds();

	Gfx_Image *player = load_image_from_disk(fixed_string("assets/tank.png"), get_heap_allocator());
	assert(player, "Couldn't load player image");

	Gfx_Image *edith = load_image_from_disk(fixed_string("assets/edith_front.png"), get_heap_allocator());
	Gfx_Image *blossom = load_image_from_disk(fixed_string("assets/blossom.png"), get_heap_allocator());
	Gfx_Image *building = load_image_from_disk(fixed_string("assets/building.png"), get_heap_allocator());
	Gfx_Image *veg = load_image_from_disk(fixed_string("assets/veg.png"), get_heap_allocator());

	Entity *player_en = entity_create();

	for (int i = 0; i < 10; i++)
	{
		Entity *en = entity_create();
		setup_veg(en);
		en->pos = v2(get_random_float32_in_range(-(window.pixel_width), window.pixel_width),
					 get_random_float32_in_range(-(window.pixel_height), window.pixel_height));
		// log("Veg at %.2f, %.2f", en->pos.x, en->pos.y);
	}

	while (!window.should_close)
	{
		if (is_key_just_pressed(KEY_ESCAPE))
		{
			window.should_close = true;
		}

		float64 now = os_get_current_time_in_seconds();
		if ((int)now != (int)last_time)
			log("%.2f FPS\n%.2fms", 1.0 / (now - last_time), (now - last_time) * 1000);
		float64 delta_t = now - last_time;
		last_time = now;

		reset_temporary_storage();

		draw_frame.projection = m4_make_orthographic_projection(window.width * -0.5, window.width * 0.5, window.height * -0.5, window.height * 0.5, -1, 10);

		draw_frame.view = m4_make_scale(v3(1.0, 1.0, 1.0));

		for (int i = 0; i < MAX_ENTITY_COUNT; i++)
		{
			Entity *en = &world->entities[i];
			if (en->is_valid)
			{
				switch (en->arch)
				{
				case arch_veg:
				{
					Vector2 size = v2(63, 56);
					Matrix4 xform = m4_scalar(1.0);
					xform = m4_translate(xform, v3(en->pos.x, en->pos.y, 0));
					// xform = m4_translate(xform, v3(size.x * -0.5, 0, 0));
					draw_image_xform(veg, xform, size, COLOR_WHITE);
				}
				default:
				{
				}
				}
			}
		}

		Vector2 input_axis = v2(0, 0);
		if (is_key_down('A'))
		{
			input_axis.x -= 1.0;
		}
		if (is_key_down('D'))
		{
			input_axis.x += 1.0;
		}
		if (is_key_down('S'))
		{
			input_axis.y -= 1.0;
		}
		if (is_key_down('W'))
		{
			input_axis.y += 1.0;
		}
		input_axis = v2_normalize(input_axis);

		player_en->pos = v2_add(player_en->pos, v2_mulf(input_axis, 1000.0 * delta_t));

		{
			Vector2 size = v2(409, 281);
			Matrix4 xform = m4_scalar(1.0);
			xform = m4_translate(xform, v3(player_en->pos.x, player_en->pos.y, 0));
			xform = m4_translate(xform, v3(size.x * -0.5, 0, 0));
			draw_image_xform(player, xform, size, COLOR_WHITE);
		}

		// float aspect = (f32)window.width / (f32)window.height;
		// float mx = (input_frame.mouse_x / (f32)window.width * 2.0 - 1.0) * aspect;
		// float my = input_frame.mouse_y / (f32)window.height * 2.0 - 1.0;

		os_update();
		gfx_update();
	}

	return 0;
}