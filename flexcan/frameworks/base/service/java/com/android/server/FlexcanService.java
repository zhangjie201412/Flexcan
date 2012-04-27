package com.android.server;
import android.content.Context;
import android.os.IFlexcanService;
import android.util.Slog;

public class FlexcanService extends IFlexcanService.Stub
{
	private static final String TAG="FlexcanService";
	private int data[];
	private Frame frame;
	FlexcanService()
	{
		//System.out.println("---FlexcanService struct func");
		frame = new Frame();
		data = new int [8];
		init_native();
	}
	
	public int set_data(int index, int dat)	//return -1: error | 0: success	
	{
		if(index >= 8)
			return -1;	//error

		data[index] = dat;
		return 0;
	}

	public int get_data(int index)
	{
		if(index >=8)
			return -1;
		else 
			return frame.getIndexData(index);
		
	}

	public int get_dlc()
	{
		return frame.getDlc();
	}
	public int Flexcan_send(int id, int dlc, int extended,int rtr, int infinite, int loopcount)
	{
		return flexcan_native_send(data, id, dlc, extended, rtr, infinite, loopcount);
	}

	public int Flexcan_dump(int id, int mask)
	{
		frame = flexcan_native_dump(id, mask, frame);

		if(frame == null)
			return 1;
		else
			return 0;
	}

	private static native boolean init_native();
	private static native int flexcan_native_send(int data[], int id, int dlc, int extended,int rtr, int infinite, int loopcount);
	private static native Frame flexcan_native_dump(int id, int mask, Frame frame);
};
