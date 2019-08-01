/*
 * Control GPIO pins on the fly
 *
 * Copyright (c) 2008-2011 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <common.h>
#include <command.h>
#include <errno.h>
#include <dm.h>
#include <asm/gpio.h>
#include <../include/pwm.h>
#include <backlight.h>

DECLARE_GLOBAL_DATA_PTR;

static int do_beep(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int channel,period,duty_ns;
	struct udevice *dev;
	int ret;
	bool enable=false;
	if(argc<4)
			return CMD_RET_USAGE;
	channel = simple_strtoul(argv[1],NULL,10);
	period = simple_strtoul(argv[2],NULL,10);
	duty_ns = simple_strtoul(argv[3],NULL,10);
	
	gpio_set_pull(EXYNOS4X12_GPIO_D00,S5P_GPIO_PULL_NONE);
	gpio_cfg_pin(EXYNOS4X12_GPIO_D00,S5P_GPIO_FUNC(0x2));

	ret = uclass_get_device(UCLASS_PWM,0,&dev);
	if(ret)	return cmd_process_error(cmdtp,ret);
	ret = pwm_set_config(dev,channel,period,duty_ns);
	if(ret)	return cmd_process_error(cmdtp,ret);
	
	ret = pwm_set_enable(dev,channel,enable);
	if(ret)	return cmd_process_error(cmdtp,ret);
#if   0
	/*
	 * TODO(sjg@chromium.org): For now we must fit into the existing GPIO
	 * framework, so we look up the name here and convert it to a GPIO number.
	 * Once all GPIO drivers are converted to driver model, we can change the
	 * code here to use the GPIO uclass interface instead of the numbered
	 * GPIO compatibility layer.
	 */
	ret = gpio_lookup_name(str_gpio, NULL, NULL, &gpio);
	if (ret) {
		printf("GPIO: '%s' not found\n", str_gpio);
		return cmd_process_error(cmdtp, ret);
	}

	/* turn the gpio name into a gpio number */
	gpio = name_to_gpio(str_gpio);
	if (gpio < 0)
		goto show_usage;

	/* grab the pin before we tweak it */
	ret = gpio_request(gpio, "cmd_gpio");
	if (ret && ret != -EBUSY) {
		printf("gpio: requesting pin %u failed\n", gpio);
		return -1;
	}

	/* finally, let's do it: set direction and exec command */
	if (sub_cmd == GPIO_INPUT) {
		gpio_direction_input(gpio);
		value = gpio_get_value(gpio);
	} else {
		switch (sub_cmd) {
		case GPIO_SET:
			value = 1;
			break;
		case GPIO_CLEAR:
			value = 0;
			break;
		case GPIO_TOGGLE:
			value = gpio_get_value(gpio);
			if (!IS_ERR_VALUE(value))
				value = !value;
			break;
		default:
			goto show_usage;
		}
		gpio_direction_output(gpio, value);
	}
#endif
	return ret;
}

U_BOOT_CMD(beep, 4, 1, do_beep,
		"beep",
	   " beep  [channel] [period_ns] [duty_ns]");
