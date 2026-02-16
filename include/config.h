#include "SD_card.h"

#define CONFIG_BUFFER_SIZE  1024
#define LINE_READ_BUFFER    128

#define CONFIG_FILE         "/.cfg"

typedef struct {
    int lighting_hue;
    int saturation;
    int brightness;
    int is_lighting_on;
    int animation_index;
} Config;
static Config GLOBAL_CONFIG;

static char* CONFIG_BUFFER = (char*) malloc(CONFIG_BUFFER_SIZE);

typedef struct {
    char name[64];
    char value[64];
} Config_Var;

void write_config_to_file();
void update_config_from_file();
void update_config_from_str(String);
void update_config_from_buffer();

/**
 * Returns 1 if the persistent config could not be connected
 */
int config_init()
{
    return sd_card_init();
}

void export_global_config_to_string(char** result)
{
    snprintf(*result, CONFIG_BUFFER_SIZE, 
        "%d %d %d %d %d\r", 
        GLOBAL_CONFIG.lighting_hue,
        GLOBAL_CONFIG.saturation,
        GLOBAL_CONFIG.brightness,
        GLOBAL_CONFIG.is_lighting_on,
        GLOBAL_CONFIG.animation_index
    );
}

void write_config_to_file()
{
    export_global_config_to_string(&CONFIG_BUFFER);

    delete_file(CONFIG_FILE);
    write_to_file(CONFIG_FILE, CONFIG_BUFFER);
}

void update_config_from_file()
{
    String file_contents = read_file(CONFIG_FILE);
    file_contents.toCharArray(CONFIG_BUFFER, CONFIG_BUFFER_SIZE);
    
    update_config_from_buffer();
}

void update_config_from_str(String text)
{
    text.toCharArray(CONFIG_BUFFER, CONFIG_BUFFER_SIZE);

    update_config_from_buffer();
}

void update_config_from_buffer()
{
    sscanf(CONFIG_BUFFER, 
        "%d %d %d %d %d\r", 
        &GLOBAL_CONFIG.lighting_hue,
        &GLOBAL_CONFIG.saturation,
        &GLOBAL_CONFIG.brightness,
        &GLOBAL_CONFIG.is_lighting_on,
        &GLOBAL_CONFIG.animation_index
    );
}