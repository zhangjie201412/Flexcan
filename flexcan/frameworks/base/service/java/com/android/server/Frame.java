package com.android.server;

public class Frame {
	private int can_id;
	private int dlc;
	private int buf[];
	Frame(){
		can_id = 1;
		dlc = 0;
		buf = new int[8];
	}
	public int getID()
	{
		return can_id;
	}
	public void setID(int id){
		this.can_id = id;
	}
	public int getDlc() {
		return this.dlc;
	}
	public void setDlc(int dlc)
	{
		this.dlc = dlc;
	}
	public int[] getBuf()
	{
		return this.buf;
	}
	public void setBuf(int buf[])
	{
		
		for(int i=0;i<buf.length;i++)
			this.buf[i] = buf[i];
	}
	public int getIndexData(int index)
	{
		if(index >= 8)
			return -1;
		else
			return this.buf[index];
	}
}

