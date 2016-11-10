#include <iostream>
#include <libusb.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
unsigned char *data;
void print_response(libusb_device_handle *dev_handle, int length)
{
	int actual; //used to find out how many bytes were read
	int r; // for return values
	r = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had jsut 1)
	if(r < 0) {
		cout<<"Cannot Claim Interface"<<endl;
		exit(0);
	}
	cout<<"Claimed Interface"<<endl;
	
	//Reading response
	cout<<"Reading Data..."<<endl;
	r = libusb_bulk_transfer(dev_handle, (129 | LIBUSB_ENDPOINT_IN), data, length, &actual, 0); //my device's in endpoint was 129
	if(r == 0 && actual == length) //we read 1 byte successfully
	{
		cout<<"Reading Successful!"<<endl;
		cout<<"Data->";
		int i;
		for(i = 0; i<length; i++)
			cout<<(int)data[i]<<" "; 
		cout<<endl;
	}
	else
		cout<<"Read Error"<<endl;
	
	
	r = libusb_release_interface(dev_handle, 0); //release the claimed interface
	if(r!=0) {
		cout<<"Cannot Release Interface"<<endl;
		exit(0); 
	}
	cout<<"Released Interface"<<endl;
}

void send_command(libusb_device_handle *dev_handle, int length)
{
	int actual; //used to find out how many bytes were written
	int r; // for return values
	r = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had jsut 1)
	if(r < 0) {
		cout<<"Cannot Claim Interface"<<endl;
		exit(0); 
	}
	cout<<"Claimed Interface"<<endl;
	
	cout<<"Writing Data..."<<endl;
	int i;
	cout<<"Data->";
	for(i = 0; i< length; i++)
	{
		cout<<data[i]<<" ";
	}
	cout<<endl;

	r = libusb_bulk_transfer(dev_handle, (2 | LIBUSB_ENDPOINT_OUT), data, length, &actual, 0); //my device's in endpoint was 129
	if(r == 0 && actual == length) //we read 1 byte successfully
	{
		cout<<"Writing Successful!"<<endl;
	}
	else
		cout<<" Error"<<endl;
	
	r = libusb_release_interface(dev_handle, 0); //release the claimed interface
	if(r!=0) {
		cout<<"Cannot Release Interface"<<endl;
		exit(0); 
	}
	cout<<"Released Interface"<<endl;
}
int main() {
	libusb_device_handle *dev_handle; //a device handle
	libusb_context *ctx = NULL; //a libusb session
	int r; //for return values
	r = libusb_init(&ctx); //initialize the library for the session we just declared
	if(r < 0) {
		cout<<"Init Error "<<r<<endl; //there was an error
		return 1;
	}
	libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation

	dev_handle = libusb_open_device_with_vid_pid(ctx, 3540, 428); //these are vendorID and productID I found for my usb device
	if(dev_handle == NULL)
		cout<<"Cannot open device"<<endl;
	else
		cout<<"Device Opened"<<endl;


	if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
		cout<<"Kernel Driver Active"<<endl;
		if(libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
			cout<<"Kernel Driver Detached!"<<endl;
	}

	data = new unsigned char[2]; //data to write

	//check status (for testing if our code broke)
	data[0]=(unsigned char)27;data[1]=(unsigned char)118;
	send_command(dev_handle, 2);
	print_response(dev_handle, 1);

	//Initilize printer
	data[0]=(unsigned char)27;data[1]=(unsigned char)64; 
	send_command(dev_handle, 2);

	sleep(1);

	//Send characters to buffer
	data[0]=(unsigned char)'H'; 
	send_command(dev_handle, 1);
	data[0]=(unsigned char)'e'; 
	send_command(dev_handle, 1);
	data[0]=(unsigned char)'l'; 
	send_command(dev_handle, 1);
	data[0]=(unsigned char)'l'; 
	send_command(dev_handle, 1);
	data[0]=(unsigned char)'o'; 
	send_command(dev_handle, 1);
	data[0]=(unsigned char)' '; 
	send_command(dev_handle, 1);
	data[0]=(unsigned char)'W'; 
	send_command(dev_handle, 1);
	data[0]=(unsigned char)'o'; 
	send_command(dev_handle, 1);
	data[0]=(unsigned char)'r'; 
	send_command(dev_handle, 1);
	data[0]=(unsigned char)'l'; 
	send_command(dev_handle, 1);
	data[0]=(unsigned char)'d'; 
	send_command(dev_handle, 1);
	data[0]=(unsigned char)'!'; 
	send_command(dev_handle, 1);
	
	data[0]=(unsigned char)10; 
	send_command(dev_handle, 1);
	
	

	libusb_close(dev_handle); //close the device we opened
	libusb_exit(ctx); //needs to be called to end the

	delete[] data; //delete the allocated memory for data
	return 0;
}
