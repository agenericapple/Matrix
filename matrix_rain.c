#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <gui/elements.h>
#include <furi_hal.h>

// Matrix Rain variables and functions
#define MATRIX_RAIN_COLUMNS 32
#define MATRIX_RAIN_ROWS 16
char matrix_rain[MATRIX_RAIN_COLUMNS][MATRIX_RAIN_ROWS];
int matrix_rain_speed = 1;

static void matrix_rain_init() {
    for (int i = 0; i < MATRIX_RAIN_COLUMNS; ++i) {
        for (int j = 0; j < MATRIX_RAIN_ROWS; ++j) {
            matrix_rain[i][j] = ' ';
        }
    }
}

static void matrix_rain_update() {
    for (int i = MATRIX_RAIN_COLUMNS - 1; i > 0; --i) {
        for (int j = 0; j < MATRIX_RAIN_ROWS; ++j) {
            matrix_rain[i][j] = matrix_rain[i - 1][j];
        }
    }

    for (int j = 0; j < MATRIX_RAIN_ROWS; ++j) {
        if (rand() % 100 < matrix_rain_speed) {
            matrix_rain[0][j] = rand() % 94 + 33; // ASCII characters from '!' to '~'
        } else {
            matrix_rain[0][j] = ' ';
        }
    }
}

static void matrix_rain_render(Canvas* const canvas) {
    for (int i = 0; i < MATRIX_RAIN_COLUMNS; ++i) {
        for (int j = 0; j < MATRIX_RAIN_ROWS; ++j) {
            canvas_set_color(canvas, ColorGreen);
            canvas_draw_char(canvas, i * 8, j * 8, matrix_rain[i][j]);
        }
    }
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    if (input_event->key == InputKeyBack && input_event->type == InputTypeShort) {
        bool* processing = event_queue;
        *processing = false;
    }
}

int32_t matrix_rain_app() {
    bool processing = true;

    // Initialize Matrix Rain
    matrix_rain_init();

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, matrix_rain_render, NULL); // No owner info needed
    view_port_input_callback_set(view_port, input_callback, &processing);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while (processing) {
        matrix_rain_update(); // Update the Matrix Rain effect
        view_port_update(view_port);
        furi_delay_ms(100);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);

    return 0;
}

int main() {
    matrix_rain_app();
    return 0;
}
