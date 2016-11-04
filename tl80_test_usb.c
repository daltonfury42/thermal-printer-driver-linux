#include <iostream>
#include <libusb.h>

using namespace std;

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

	unsigned char *data = new unsigned char[2]; //data to write
	data[0]=(unsigned char)27;data[1]=(unsigned char)118; //some dummy values

	int actual; //used to find out how many bytes were written
	if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
		cout<<"Kernel Driver Active"<<endl;
		if(libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
			cout<<"Kernel Driver Detached!"<<endl;
	}
	r = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had jsut 1)
	if(r < 0) {
		cout<<"Cannot Claim Interface"<<endl;
		return 1;
	}
	cout<<"Claimed Interface"<<endl;
	
	cout<<"Data->"<<(int)data[0]<<(int)data[1]<<"<-"<<endl; 
	cout<<"Writing Data..."<<endl;
	r = libusb_bulk_transfer(dev_handle, (2 | LIBUSB_ENDPOINT_OUT), data, 2, &actual, 0); //my device's out endpoint was 2
	if(r == 0 && actual == 2) //we wrote the 4 bytes successfully
		cout<<"Writing Successful!"<<endl;
	else
		cout<<"Write Error"<<endl;

	//Reading response
	cout<<"Reading Data..."<<endl;
	r = libusb_bulk_transfer(dev_handle, (129 | LIBUSB_ENDPOINT_IN), data, 1, &actual, 0); //my device's in endpoint was 129
	if(r == 0 && actual == 1) //we read 1 byte successfully
	{
		cout<<"Reading Successful!"<<endl;
		cout<<"Data->"<<(int)data[0]<<"<-"<<endl; 
	}
	else
		cout<<"Read Error"<<endl;
	
	
	r = libusb_release_interface(dev_handle, 0); //release the claimed interface
	if(r!=0) {
		cout<<"Cannot Release Interface"<<endl;
		return 1;
	}
	cout<<"Released Interface"<<endl;

	libusb_close(dev_handle); //close the device we opened
	libusb_exit(ctx); //needs to be called to end the

	delete[] data; //delete the allocated memory for data
	return 0;
}
