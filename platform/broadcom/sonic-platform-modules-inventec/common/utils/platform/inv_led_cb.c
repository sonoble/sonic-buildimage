void inv_ledproc_linkscan_cb(int unit, soc_port_t port, bcm_port_info_t *info)
{
	//int unit = 0;
	int speed, up;
	bcm_pbmp_t pbmp;
	bcm_port_config_t cfg;
	bcm_port_config_t_init(&cfg);
	bcm_port_config_get(unit, &cfg);
	BCM_PBMP_ASSIGN(pbmp, cfg.port);

	BCM_PBMP_ITER(pbmp, port)
	{
		uint8 led_control_data = 0;
		if (port == 66)
			continue;
		bcm_port_link_status_get(unit, port, &up);
		//printf("port = %d, up = %d\n", port, up);
		if (up == 1){
			led_control_data |= 0x1;
			bcm_port_speed_get(unit, port, &speed);
			//printf("port = %d, speed = %d\n", port, speed);
			switch (speed)
			{
				case 10000:
					led_control_data |= (1 << 1);
					break;
				case 25000:
					led_control_data |= (2 << 1);
					break;
				case 40000:
					led_control_data |= (3 << 1);
					break;
				case 100000:
					led_control_data |= (4 << 1);
					break;
			}
		}
		else{
			led_control_data |= 0;
		}
		//led_control_data |= 0x10;
		//printf("data = %d\n", led_control_data);
		if (port >= 67){
			bcm_switch_led_control_data_write(0, 0, (port-3) * sizeof(led_control_data), &led_control_data, sizeof(led_control_data));
			bcm_switch_led_control_data_read(0, 0, (port-3) * sizeof(led_control_data), led_control_data, sizeof(led_control_data));
		}
		else{
		bcm_switch_led_control_data_write(0, 0, (port-1) * sizeof(led_control_data), &led_control_data, sizeof(led_control_data));
		bcm_switch_led_control_data_read(0, 0, (port-1) * sizeof(led_control_data), led_control_data, sizeof(led_control_data));
		}
		//printf("ctrl_data = %d\n", led_control_data);
	}
}

bcm_linkscan_register(0, inv_ledproc_linkscan_cb);
