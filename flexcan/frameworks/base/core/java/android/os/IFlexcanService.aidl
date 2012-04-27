package android.os;


interface IFlexcanService
{
	int Flexcan_send(int id, int dlc, int extended,int rtr, int infinite, int loopcount);
	int Flexcan_dump(int id, int mask);
	int set_data(int index, int dat);
	int get_data(int index);
	int get_dlc();
}
