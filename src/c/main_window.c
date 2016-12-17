#include <pebble.h>
#include "main.h"
#include "main_window.h"


Window *main_window;
MenuLayer *main_menu_layer;


uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return app_settings.no_of_rutines;
}

int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, "Routines");
}

void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  char snum[5] = "00000";
  char name[30];
  int id = app_settings.set_routines[cell_index->row];
  snprintf(snum, sizeof(snum), "%d", menu_data.routine_length[id]);
  snprintf(name, sizeof(name), "%s", menu_data.routine_names[id]);
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%d row: %s: %s.", cell_index->row, name, snum);
  menu_cell_basic_draw(ctx, cell_layer, name, snum, NULL);
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  int key = app_settings.set_routines[cell_index->row];
  load_routine(key);
  app_settings.current_routine = key;
  open_starting_window();

}

void setup_menu_layer(Window *window) {
	Layer *window_layer = window_get_root_layer(window);

    main_menu_layer = menu_layer_create(GRect(0, 0, 144, 168));
    menu_layer_set_callbacks(main_menu_layer, NULL, (MenuLayerCallbacks){
        .get_num_sections = menu_get_num_sections_callback,
        .get_num_rows = menu_get_num_rows_callback,
        .get_header_height = menu_get_header_height_callback,
        .draw_header = menu_draw_header_callback,
        .draw_row = menu_draw_row_callback,
        .select_click = menu_select_callback,
    });

    menu_layer_set_click_config_onto_window(main_menu_layer, window);

    layer_add_child(window_layer, menu_layer_get_layer(main_menu_layer));
}

void main_window_show(){
  main_window_create();
  window_stack_push(main_window_get_window(), true);
}


void main_window_load(Window *window) {
  setup_menu_layer(window);
}


void main_window_unload(Window *window) {
  menu_layer_destroy(main_menu_layer);
}


void main_window_create() {
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });
}


void main_window_destroy() {
  window_destroy(main_window);
}


Window *main_window_get_window() {
  return main_window;
}
