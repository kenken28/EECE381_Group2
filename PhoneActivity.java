package com.example.module2_test1;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ContextWrapper;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pDeviceList;
import android.net.wifi.p2p.WifiP2pManager;
import android.net.wifi.p2p.WifiP2pManager.Channel;
import android.os.Bundle;
import android.os.Handler;
import android.util.Base64;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

public class PhoneActivity extends Activity {
	
	public static final int port = 8888;
	private ServerSocket serverSocket;
	Handler updateConversationHandler;
	Thread serverThread = null;
	 private TextView text;
	 
	 private volatile boolean stop = false;


	
	private WifiP2pManager wifiManager;
	//private WifiManager mManager;
	private ImageView imgNew;

	Channel mChannel;
	BroadcastReceiver mReceiver;
	IntentFilter mIntentFilter;
	
	//Socket client;
	byte[] data;
	public ContextWrapper cw;
	WifiP2pDevice device;
	private TextView Text;
	
	
	

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_phone);
		
		text = (TextView) findViewById(R.id.textTest);
		imgNew = (ImageView) findViewById(R.id.PhotoforPhone);
		
		
		
		//updateConversationHandler = new Handler();
		//this.serverThread = new Thread(new ServerThread());
		//this.serverThread.start();
		
		
		Button button1 = (Button) findViewById(R.id.refresh_button);		
		button1.setOnClickListener(new OnClickListener(){
			public void onClick(View view){
			
				
				updateConversationHandler = new Handler();
				serverThread = new Thread(new ServerThread());
				serverThread.start();
				
				//discoverPeers();
				//requestForPeers();
				//connectToPeers();
				//new PhoneServer().execute();
				//updateConversationHandler = new Handler();
				//serverThread = new Thread(new ServerThread());
				 //serverThread.start();
				//connectToPeers();
				//debug = myClient.doInBackground();
				//Toast toast = Toast.makeText(getApplicationContext(), debug.toString(), Toast.LENGTH_SHORT);
				//toast.show();
				
		    	/*String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
		    	String fileName = "IMG_"+timeStamp+".jpeg";
		    	cw = new ContextWrapper(getApplicationContext());
		    	File mydir = cw.getDir("SecuCam",Context.MODE_PRIVATE);
		    	File mypath = new File(mydir,fileName);
		    	Bitmap bmp = BitmapFactory.decodeByteArray(data, 0, data.length);
		    	imgNew.setImageBitmap(bmp);
		    	saveToInternalStorage(bmp,mypath);
		    	*/
			}
		});
		Button button2 = (Button) findViewById(R.id.stop_button);
		button2.setOnClickListener(new OnClickListener(){
			public void onClick(View view){
				try {
					
					serverSocket.close();
					
					
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			
		});
	}
	
	@Override
	    protected void onStop() {
	        super.onStop();
	        try {
	            serverSocket.close();
	        } catch (IOException e) {
	            e.printStackTrace();
	        }
	    }

	
	class ServerThread implements Runnable {
		 
		        public void run() {
		            Socket socket = null;
		            
		           
		           try {
		
		                serverSocket = new ServerSocket(port);
		
		            } catch (IOException e) {
		
		                e.printStackTrace();
		
		            }
		
		            while (!Thread.currentThread().isInterrupted()) {
		 
		                try {
		 
		                    socket = serverSocket.accept();
		 
		
		                    CommunicationThread commThread = new CommunicationThread(socket);
		
		                    new Thread(commThread).start();
		
		 
		
		                } catch (IOException e) {
		
		                    e.printStackTrace();
		
		                }
		
		            }

		        }
		    
		       
	
	class CommunicationThread implements Runnable {
	
		 
		
		        private Socket clientSocket;
		
		        
		
		        private BufferedReader input;
		        
		        private StringBuilder finalString = new StringBuilder();
		       // private InputStream is;
		        
		       // private DataInputStream input;
		        
		        //private int len;
		
		 
		
		        public CommunicationThread(Socket clientSocket) {
		
		 
		
		            this.clientSocket = clientSocket;
		
		 
		
		            try {
		
		            	//this.is = this.clientSocket.getInputStream();
		
		                //this.input = new DataInputStream(is);
		                
		               //this.len = input.readInt();
		            	this.input = new BufferedReader(new InputStreamReader(this.clientSocket.getInputStream()));
		                
		
		 
		
		            } catch (IOException e) {
		
		                e.printStackTrace();
		
		            }
	
		        }
		
		 
		
		        public void run() {
		
		 
		
		            while (!Thread.currentThread().isInterrupted()) {
		
		 
		
		                try {
		
		 
		                	
		                   String line;
		                   while((line = input.readLine()) !=null){
		                	   finalString.append(line);
		                   }
		                   String read = finalString.toString();
		                	//byte[] data = new byte[this.len];
		                	//if(this.len>0){
		                		//this.input.readFully(data);
		                	//}
		                    
		
		 
		                    
		                    updateConversationHandler.post(new updateUIThread(read));
		
		 
		
		                } catch (IOException e) {
	
		                    e.printStackTrace();
		
		                }
		
		            }
		
		        }
		
		 
		
		    }
	}

	class updateUIThread implements Runnable {
		
		        private Bitmap myPic;
				//private String data;
		 
		
		        public updateUIThread(String myDecodeData) {
		
		            //this.myPic = data;
		        	
		        	this.myPic = StringToBitmap(myDecodeData);
		
		        }
		
		 
		
		        @Override
		
		        public void run() {
		
		        	//Bitmap bmp = BitmapFactory.decodeByteArray(this.myPic, 0, this.myPic.length);
		        	//text.setText(text.getText().toString()+"Client Says: "+ msg + "\n");
		        	
		        	imgNew.setImageBitmap(myPic);
		        	text.setText("Someone is breaking into your house");
		
		        }
		
		    }


	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.phone, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
	
	@Override
	protected void onResume(){
		super.onResume();
		
	}
	
	@Override
	protected void onPause(){
		super.onPause();
		
	}
	
	

	
	
	
	
	public void saveToInternalStorage(Bitmap bitmapImage,File mypath){
		FileOutputStream fos;
		try{
			fos = new FileOutputStream(mypath);
			bitmapImage.compress(Bitmap.CompressFormat.JPEG, 100, fos);
	        fos.close();
    	}catch(Exception e){
    		
    		
    	}
		
	}
	
	
	public Bitmap StringToBitmap(String encodeString){
		try{
			byte[] encodeByte = Base64.decode(encodeString, Base64.DEFAULT);
			Bitmap bitmap = BitmapFactory.decodeByteArray(encodeByte, 0, encodeByte.length);
			return bitmap;
		}catch(Exception e){
			e.printStackTrace();
			return null;
		}
	}
	
	
		
	
	
	
	
}

