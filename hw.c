#include "hw.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "utils.h"
#include "log.h"
#include "gui.h"


void init_hw_clock()
{
    int fd;
    char buf[32];
    int ret;
    antfx_conf_t* config = antfx_get_config();
    fd = open(config->i2c_dev_del, O_WRONLY);
    if (fd != -1)
    {
        snprintf(buf, 32, "0x%02X", config->i2c_hw_clock_addr);
        ret = write(fd, buf, strlen(buf));
        close(fd);
    }
    fd = open(config->i2c_dev_new, O_WRONLY);
    if (fd == -1)
    {
        ERROR("Unable to open sys file for registering HW clock: %s", strerror(errno));
        return;
    }
    snprintf(buf, 32, "%s 0x%02X", "ds1307", config->i2c_hw_clock_addr);
    ret = write(fd, buf, strlen(buf));
    close(fd);
    if (ret != (int)strlen(buf))
    {
        ERROR("Unable to write command to init HW clock");
        return;
    }
    usleep(1000000);
    ret = system("hwclock -s");
    if (ret == -1)
    {
        ERROR("Unable to execute commad for applying datetime from HW clock: %s", strerror(errno));
        return;
    }
}
void fm_set_freq(double f)
{
    uint8_t radio[5] = {0};
    uint8_t freq_h = 0;
    uint8_t freq_l = 0;
    int fd;
    ssize_t ret;
    unsigned int freq_b;
    antfx_conf_t* config = antfx_get_config();
    freq_b = 4 * (f * 1000000 + 225000) / 32768; //calculating PLL word
    freq_h = freq_b >> 8;
    freq_l = freq_b & 0XFF;

    //printf ("Frequency = "); printf("%f",frequency);
    //printf("\n"); // data to be sent

    radio[0] = freq_h; //FREQUENCY H
    radio[1] = freq_l; //FREQUENCY L
    radio[2] = 0xB0;   //3 byte (0xB0): high side LO injection is on,.
    radio[3] = 0x10;   //4 byte (0x10) : Xtal is 32.768 kHz
    radio[4] = 0x00;   //5 byte0x00)

    if ((fd = wiringPiI2CSetup(config->i2c_hw_radio_addr)) < 0)
    {
        ERROR("error opening i2c channel");
    }
    ret = write(fd, radio, 5);
    if (ret == -1 || ret != 5)
    {
        ERROR("Unable to write i2c set freq command to radio control: %s", strerror(errno));
    }
    close(fd);
    LOG("FM RADIO on at frequency: %f", f);
}
void fm_mute()
{
    uint8_t radio[5] = {0};
    uint8_t freq_h = 0;
    uint8_t freq_l = 0;
    int fd;
    ssize_t ret;
    unsigned int freq_b;
    antfx_conf_t* config = antfx_get_config();
    double frequency = fm_get_freq();
    if (frequency == -1)
    {
        return;
    }
    freq_b = 4 * (frequency * 1000000 + 225000) / 32768; //calculating PLL word
    freq_h = freq_b >> 8;
    freq_h = freq_h | 0x80; // mutes the radio
    freq_l = freq_b & 0XFF;

    //load the above into the array
    radio[0] = freq_h; //FREQUENCY H
    radio[1] = freq_l; //FREQUENCY L
    radio[2] = 0xB0;   //3 byte (0xB0): high side LO injection is on,.
    radio[3] = 0x10;   //4 byte (0x10) : Xtal is 32.768 kHz
    radio[4] = 0x00;   //5 byte0x00)

    if ((fd = wiringPiI2CSetup(config->i2c_hw_radio_addr)) < 0)
    {
        ERROR("error opening i2c channel");
    }
    ret = write(fd, radio, 5);
    if (ret == -1 || ret != 5)
    {
        ERROR("Unable to write i2c mute command to radio control: %s", strerror(errno));
    }
    close(fd);
    LOG("FM RADIO off at frequency: %f", frequency);
}
double fm_get_freq()
{
    uint8_t radio[5] = {0};
    ssize_t ret;
    int fd;
    double frequency;
    antfx_conf_t* config = antfx_get_config();
    if ((fd = wiringPiI2CSetup(config->i2c_hw_radio_addr)) < 0)
    {
        ERROR("error opening i2c channel");
    }
    ret = read(fd, radio, 5);
    close(fd);
    if (ret == -1 || ret != 5)
    {
        ERROR("Unable to read i2c command from radio control: %s", strerror(errno));
        return -1;
    }

    frequency = ((((radio[0] & 0x3F) << 8) + radio[1]) * 32768 / 4 - 225000) / 10000;
    frequency = round(frequency * 10.0) / 1000.0;
    frequency = round(frequency * 10.0) / 10.0;

    return frequency;
}