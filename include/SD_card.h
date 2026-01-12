#include <FS.h>
#include <SPI.h>
#include <SD.h>

#define SD_MOSI 11
#define SD_MISO 13
#define SD_SCK 12
#define SD_CS 10

SPIClass SD_SPI;

void write_to_file(const char* filename, const char* data);
void delete_file(const char* filename);
String read_file(const char* filename);

void sd_card_init()
{
    SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, SD_SPI, 80000000)) {
        // Card mount succeeded
        return;
    }
    else
    {
        // Card mount failed
    }
}

void write_to_file(const char* filename, const char* data)
{
    File file = SD.open(filename, "w", true);
    file.print(data);
    file.close();
}

void delete_file(const char* filename)
{
    SD.remove(filename);
}

String read_file(const char* filename)
{
    File file = SD.open(filename);
    String file_contents = file.readStringUntil('\r');
    file.close();
    return file_contents;
}