package com.liujun;

import android.app.Activity;
import android.os.Bundle;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.os.IFlexcanService;
import android.os.ServiceManager;
//import com.android.server.FlexcanService;
import com.android.server.sip.SipService;
import android.os.Handler;
import android.os.Message;

public class Flexcan extends Activity implements android.view.View.OnClickListener{
    private final static String LOG_TAG = "com.liujun.Flexcan";
    private IFlexcanService flexcanService = null;
//    private FlexcanService fs = null;
    private Button button = null;
    private Button button1 = null;
	
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        ///////////////////////////////
        flexcanService = IFlexcanService.Stub.asInterface(
        		ServiceManager.getService("flexcan"));
        
        button = (Button)findViewById(R.id.button);
        button.setOnClickListener(this);
        button1 = (Button)findViewById(R.id.button1);
        button1.setOnClickListener(this);
    }

    Handler handler = new Handler(){
    	public void handleMessage(Message msg){
    	super.handleMessage(msg);
    	}
    };
    public class MyThread implements Runnable{
    	   public void run() {
    	    // TODO Auto-generated method stub
    	    while (true) {
    	     try {
    	      Thread.sleep(1000);

			try {

				int i;
				int ret = 1; 
				int tmp[];
				int dlc = 0;
				tmp = new int [8];
			//	while( dlc == 0 )
			//	{
					ret = flexcanService.Flexcan_dump(25,0);
					dlc = flexcanService.get_dlc();
					Log.i(LOG_TAG, "===> no read!"+dlc);
		//		}
				for(i=0;i<8;i++){
					tmp[i] = flexcanService.get_data(i);
					Log.i(LOG_TAG, "===> dump_data "+i+": "+tmp[i]);
				
				}
				}catch (RemoteException e) {
				// TODO: handle exception
				Log.e(LOG_TAG, "Remote exception while flexcan send!!!");
			}





    	      Message message=new Message();
    	      message.what=1;
    	      handler.sendMessage(message);
	    	    } catch (InterruptedException e) {
    	      // TODO Auto-generated catch block
    	      e.printStackTrace();
    	     }
    	    }
    	   }
    	}



	public void onClick(View v) {
		// TODO Auto-generated method stub
		if(v.equals(button))
		{
			try {
				int i;
				int data[] = {11,22,33,44,55,66,77,88};
				for(i=0;i<8;i++)
					flexcanService.set_data(i,data[i]);
//				fs.data = data;
//				fs.Flexcan_send(25,5,0,0,0,0);
				int ret = flexcanService.Flexcan_send(25,8,0,0,0,1);
			}catch (RemoteException e) {
				// TODO: handle exception
				Log.e(LOG_TAG, "Remote exception while flexcan send!!!");
			}
		}
		if(v.equals(button1))
		{
/*			try {
				int i;
				int ret = 1; 
				int tmp[];
				int dlc = 0;
				tmp = new int [8];
				while( dlc == 0 )
				{
					ret = flexcanService.Flexcan_dump(0,25);
					dlc = flexcanService.get_dlc();
					Log.i(LOG_TAG, "===> no read!"+dlc);
				}
				for(i=0;i<8;i++){
					tmp[i] = flexcanService.get_data(i);
					Log.i(LOG_TAG, "===> dump_data "+i+": "+tmp[i]);
				
				}
			}catch (RemoteException e) {
				// TODO: handle exception
				Log.e(LOG_TAG, "Remote exception while flexcan send!!!");
			}*/
		new Thread(new MyThread()).start();
		}
	}
    
}
