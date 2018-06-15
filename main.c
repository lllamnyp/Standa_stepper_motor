int main (int argc, char* argv[]) {
	device_t device;
	status_t status;
	move_settings_t move_settings;
	edges_settings_t edges_settings;
	power_settings_t power_settings;
	int target;
	int names_count;
	char device_name[256];
	const int probe_flags = ENUMERATE_PROBE;
	device_enumeration_t devenum;

//	Test for 1 or no arguments, argument must represent integer
	if (argc > 2) {
		printf("Unexpected extra arguments\nExample usage: %s [%%azimuth]\n%%azimuth is an integer specifying target position in degrees\n", argv[0]);
		return 1;
	}

	if (argc == 2 && validateArg(argv[1]) == 0) {
		printf("Invalid argument, must be integer specifying polarizer azimuth");
		return 1;
	}
	else if (argc == 2) {
		sscanf(argv[1], "%d", &target);
		target = mod(target, 360);
		target *= STEPS_PER_DEG;
	}

//	Device enumeration function. Returns an opaque pointer to device enumeration data.
//	Then find number of devices and terminate if none are connected.
//	Always release the memory allocated to devenum before ending the program
	devenum = enumerate_devices(probe_flags, "");
	names_count = get_device_count( devenum );

	if (names_count <= 0) {
		printf( "No devices found\n" );
		free_enumerate_devices( devenum );
		return 1;
	}

//	Get name of device 0 and open device by name (like a file pointer)
	strcpy( device_name, get_device_name( devenum, AXIS_TO_USE ) );
	free_enumerate_devices( devenum );
	device = open_device( device_name );

//  Get the power settings and enable automatic power off (flag 0x02)
//	after 5 seconds of idle since we're probably going to just leave
//	all the hardware plugged in and switched on
	get_power_settings(device, &power_settings);
	printf("power set %d \n", power_settings.PowerFlags);
	power_settings.PowerFlags |= 0x02;
	power_settings.PowerOffDelay = 5;
	set_power_settings(device, &power_settings);

//  Read in move settings and reduce the speed for better precision
//	No hard data on whether this helps, 10 degrees per second seems ok
	get_move_settings(device, &move_settings);
	move_settings.Speed = 1500; // steps/s
	move_settings.Accel = 500;  // steps/s^2
	move_settings.Decel = 500;	// steps/s^2
	set_move_settings(device, &move_settings);

//	Home device if no arguments passed
//	This is the analyzer. It most likely needs homing after completing a measurement.
//	At ANKA it stops at +18 degrees, so no motion "past limit switch" usually needed.
	if (argc == 1) {
		printf("Homing polarizer\n");
		command_homezero(device);
		command_wait_for_stop(device, 1000);
		return 0;
	}

//	TODO: Implement rehoming if a limit switch is hit
	get_edges_settings(device, &edges_settings);
	edges_settings.BorderFlags = 0x00;
	set_edges_settings(device, &edges_settings);

//	Get and print relevant status info
//	TODO: Implement time-stamping and logging to a file?
	get_status( device, &status );
	printf("position %d, encoder %lld, speed %d\n\n", status.CurPosition, status.EncPosition, status.CurSpeed);
	printf("Limit switch (3 == on, 0 == off) %d; Power state (4 == reduced, 1 == off) %d\n", status.GPIOFlags & 0x000003, status.PWRSts);

//  Move to target
	goto_custom(device, status.CurPosition, target);

//	Status, see above
	get_status(device, &status);
	printf("position %d, encoder %lld, speed %d\n\n", status.CurPosition, status.EncPosition, status.CurSpeed);
	printf("Limit switch (3 == on, 0 == off) %d; Power state (4 == reduced, 1 == off) %d\n", status.GPIOFlags & 0x000003, status.PWRSts);

	close_device( &device );

	return 0;
}
