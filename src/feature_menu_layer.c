#include "pebble.h"

#define NUM_MENU_SECTIONS 2
#define NUM_MENU_ICONS 3
#define NUM_FIRST_MENU_ITEMS 2
#define NUM_SECOND_MENU_ITEMS 68

static Window *window;

// This is a menu layer
// You have more control than with a simple menu layer
static MenuLayer *menu_layer;

// Menu items can optionally have an icon drawn with them
static GBitmap *menu_icons[NUM_MENU_ICONS];

static int current_icon = 0;

static MenuLayer *info;

// You can draw arbitrary things in a menu item such as a background
static GBitmap *menu_background;

// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    case 1:
      return NUM_SECOND_MENU_ITEMS;
    default:
      return 0;
  }
}

static const char* artists[] = {
"3LAU",
"Above & Beyond",
"Alesia",
"Alesso",
"Aly & Fila",
"Amine Edge & DANCE",
"Andrew Rayel",
"Antiserum",
"Antiserum vs. Mayhem",
"Arctic Moon",
"Arty",
"Baumer",
"Brillz",
"Cedric Gervais",
"Claude VonStroke",
"Cosmic Gate",
"Dash Berlin",
"Datsik",
"David Steven",
"Dillon Francis",
"Dirtyphonics",
"Emma Hewitt",
"Fishfood",
"Flinch",
"Flosstradamus",
"Fury",
"Green Velvet",
"Harvard Bass",
"Headhunterz",
"Henry Fong",
"Hunter/Game",
"Illustrated",
"Indecent Noise",
"Isaac",
"Kennedy Jones",
"Knife Party",
"MC Dino",
"MOTi",
"Manufactured Superstars",
"Max Vangeli",
"Mayhem",
"Must Die!",
"Myon & Shane 54",
"Noisia",
"Oliver Heldens",
"Ookay",
"Ookay B2B Kennedy Jones",
"Orjan Nilsen",
"Paper Diamond",
"Planet of the Drums",
"Protohype",
"RAM",
"Rebecca & Fiona",
"Richy",
"Route 94",
"SBCR DJ Set (The Bloody Beetroots)",
"Sander van Doorn",
"Showtek",
"Steve Angello",
"Steve Aoki",
"Sunnery James & Ryan Marciano",
"Thomas Gold",
"Tritonal",
"Umek",
"Ummet Ozcan",
"Vice",
"W&W",
"Yellow Claw"
};

// A callback is used to specify the height of the section header
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Days");
      break;

    case 1:
      menu_cell_basic_header_draw(ctx, cell_layer, "Artists");
      break;
  }
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case 0:
      switch(cell_index -> row) {
        case 0:
          menu_cell_basic_draw(ctx, cell_layer, "Friday", NULL, NULL);
        break;
        
        case 1:
          menu_cell_basic_draw(ctx, cell_layer, "Saturday", NULL, NULL);
        break;
      }
    break;
  
    case 1:
      // Use the row to specify which item we'll draw
      menu_cell_basic_draw(ctx, cell_layer, artists[cell_index->row], "9:00 Trap Stage", NULL);
      break;
  }
}
MenuLayer *menuLayer;

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  switch (cell_index->row) {
    // This is the menu item with the cycling icon
    
    case 0:
      //data = "test";
      Layer *window_layer = window_get_root_layer(menu_layer);
      GRect bounds = layer_get_frame(window_layer);
      menuLayer = menu_layer_create(bounds);
      window_stack_push(menuLayer, true);
      //layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;
      
    case 1:
      // Cycle the icon
      current_icon = (current_icon + 1) % NUM_MENU_ICONS;
      // After changing the icon, mark the layer to have it updated
      layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;
  }

}

// This initializes the menu upon window load
void window_load(Window *window) {
  // Here we load the bitmap assets
  // resource_init_current_app must be called before all asset loading
  int num_menu_icons = 0;
  menu_icons[num_menu_icons++] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_BIG_WATCH);
  menu_icons[num_menu_icons++] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_SECTOR_WATCH);
  menu_icons[num_menu_icons++] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_BINARY_WATCH);

  // And also load the background
  menu_background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_BRAINS);

  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

void window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);

  // Cleanup the menu icons
  for (int i = 0; i < NUM_MENU_ICONS; i++) {
    gbitmap_destroy(menu_icons[i]);
  }

  // And cleanup the background
  gbitmap_destroy(menu_background);
}

int main(void) {
  window = window_create();

  // Setup the window handlers
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(window, true /* Animated */);

  app_event_loop();

  window_destroy(window);
}
