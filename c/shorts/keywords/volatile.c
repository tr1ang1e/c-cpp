/* 'volatile' keyword is used for
 * prevent compiler from optimizing 
 * marked out variable
 * 
 * The thing we want to say to compiler is:
 * "this variable might be changed from the place which is unknown to you"
 * 
 * Common cases to use 'volatile'
 *   > interact with hardware which changes memory content by itself
 *   > have another thread also uses the same memory
 *   > there is a signal handler might chacnge memory content
 * 
 * */
 
/* Example case: writing for hardware,
 * some hardware memory is mapped to RAM. So
 * this memory might be changed not only by
 * our program but by hardware itself
 * 
 * Let us say we have structure to interact with
 * our hardware: ...
 * 
 * */

struct MyHardwareGadget  // struct to interact with hardware device
{
	int command;		 // command idntificator
	int	isBusy;			 // flag to know if device is able to handle command
};

/* ... and the following function to
 * send command to the hardware device.
 * 
 * */

void send_command(MyHardwareGadget* gadget, int command)
{
	while (gadget->isBusy)
	{
		// wait device for being free
	}

	gadget->command = command;		// writing command starts the action
}

/* The compiler might optimize previous function code in such a way:
 * ... while(gadget->isBusy) ...  >>  ... while(1) ...
 * 
 * The reason is that compiler
 *   - doesn't know anyting about device 
 *   - can't find place in function code which affects this variable
 * and that's why replaces variable with constant in purpose of optimization
 * 
 * The right way to declare variable is:
 * (replace or comment previous realization)
 * 
 * */

void send_command(volatile MyHardwareGadget* gadget, int command)
{
	// ...
}