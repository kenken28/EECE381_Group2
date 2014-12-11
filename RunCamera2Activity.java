package com.example.module2_test1;

import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.content.Context;
import android.content.ContextWrapper;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.media.AudioManager;
import android.media.SoundPool;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.util.Base64;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
//import com.example.module2_test1.MainActivity.MyApplication;

public class RunCamera2Activity extends Activity {
	public static final int MEDIA_TYPE_IMAGE = 1;
	public static final int MEDIA_TYPE_VIDEO = 2;
	
    private Camera mCamera;
    private CameraPreview mPreview;
    private ImageView imgNew;
    private TextView comResult;
    Timer my_timer;
    private Bitmap bmp, resized, resized2;
    private double comResultNum2;
    private int cnt = 0;
    private int[] photoArray = new int[240*320];
    private int countdown = 2;
    private int startFlag = 1;
    private int streamID = 0;
    private static int sum = 0;
    private SoundPool sp;
	private int[] soundIds;
    private String comResultNum,temp1,temp2;
    private static final double sensitivity = 95;
    private static final int ServerPort = 8888;
    private static final String Server_ip="192.168.1.140";
    	
    public ContextWrapper cw;
    Socket socket;
    
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_run_camera2);
		
        // Create an instance of Camera
        mCamera = getCameraInstance();

        // Create our Preview view and set it as the content of our activity.
        mPreview = new CameraPreview(this, mCamera);
        FrameLayout preview = (FrameLayout) findViewById(R.id.camera_preview);
        preview.addView(mPreview);
        
        
        my_timer = new Timer();
        MyTimerTask my_task = new MyTimerTask();
        my_timer.schedule(my_task, 100, 3000);

        
        imgNew = (ImageView)findViewById(R.id.lastPhoto);
        
        comResult = (TextView) findViewById(R.id.text_comparison_result);

        setUpSound();

        startFlag = 1;
	}
	
public void sendString(Bitmap bitmap){
		
		String myData;
		try{
			myData = convertToString(bitmap);
			//String str = "eece 381";
			PrintWriter out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())),true);
			out.println(myData);
			
			//OutputStream outputStream = socket.getOutputStream();
			//DataOutputStream dos = new DataOutputStream(outputStream);
			//Log.d("my data length", Integer.toString(myData.length));
			//dos.write(myData, 0, myData.length);
		}catch(UnknownHostException e){
			e.printStackTrace();
		}catch (IOException e){
			e.printStackTrace();
		}catch (Exception e){
			e.printStackTrace();
		}
	}
	
	class ClientThread implements Runnable{
		@Override
		public void run(){
			try {
				
				                InetAddress serverAddr = InetAddress.getByName(Server_ip);
				
				 
				
				                socket = new Socket(serverAddr, ServerPort);
				
				 
				
				            } catch (UnknownHostException e1) {
				
				                e1.printStackTrace();
				
				            } catch (IOException e1) {
				
				                e1.printStackTrace();
				
				            }
				
				 
				
				        }
				
				 
				
				    }
	

	public class MyTimerTask extends TimerTask {
		public void run() {
			// display the image using the UI Thread
			runOnUiThread(new Runnable() {
				public void run() {
					Log.i("MyTimerTask", Integer.toString(cnt++));
					mCamera.takePicture(null, null, mPicture);
				}
			});

		}	
	}
	
	
	private PictureCallback mPicture = new PictureCallback() {

	    @Override
	    public void onPictureTaken(byte[] data, Camera camera) {
	    	mCamera.startPreview();
	    	
	    	BitmapFactory.Options options = new BitmapFactory.Options();
	    	options.inPreferredConfig=Bitmap.Config.RGB_565;
	    	
	    	bmp = BitmapFactory.decodeByteArray(data, 0, data.length, options);
	    	resized2 = resized;
	    	resized = Bitmap.createScaledBitmap(bmp, 320, 240, true);
	    	imgNew.setImageBitmap(resized);
	    	//
	    	new Thread(new ClientThread()).start();
	    	
	    	String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
	    	String fileName = "IMG_"+timeStamp+".jpeg";
	    	cw = new ContextWrapper(getApplicationContext());
	    	File mydir = cw.getDir("SecuCam",Context.MODE_PRIVATE);
	    	File mypath = new File(mydir,fileName);
	    	
	    	saveToInternalStorage(fileName,mypath,mydir, bmp);
	    	
	    	
	    	
	    	for (int i = 0; i < 240; i++) {
	  	      for (int j = 0; j < 320; j++) {
	  	    	photoArray[j+i*320] = 0x0000FFFF & ARGBtoRGB(resized.getPixel(j, i));
	  	      }
	  	    }
	    	

	    	bmp = null;
	    	System.gc();

	    	
	    	if ( resized != null && resized2 != null ) {
		    	comResultNum2 = photoComparison2(resized, resized2);
		    	String stringResult;
		    	if (comResultNum2 < sensitivity) {
		    		//stringResult = "Alarm";
		    		if (startFlag == 1){
		    			startFlag = 0;
		    		} else {
		    			
		    			sendString(resized);
		    			 sp.play(soundIds[0], 100, 100, 1, 0, 1f);
			    		
		    		}
		    	} else {
		    		//stringResult = "Good";
		    	}
	    		stringResult= Double.toString(comResultNum2);
		        comResult.setText(stringResult);
		        
		        
		    	if ( comResultNum2 < sensitivity && comResultNum2 != 0 && countdown == 0) {
		    		
		    		sendMessageCam();
		    		
		    		countdown = 13;
		    	} else {
		    		if (countdown != 0){
			    		countdown--;
		    		}
		    	}
		    	
	    	}
	    	
	    	
	    	
	    	imgNew.setImageBitmap(resized);
	    	
	    	

	    }
	    
	    
	    
	    
		private String photoComparison(Bitmap img1, Bitmap img2) {
			
			temp1 = getHash(img1);
			 temp2 = getHash(img2);
			 String result = "";
			int diffNum = 0;
			String str1 = temp1;
			String str2 = temp2;
			char[]  arr1;
			char[]  arr2;
			
			arr1 = str1.toCharArray();
			arr2 = str2.toCharArray();
		
			for (int i=0; i < 80; i++){
				if (arr1[i] != arr2[i]) {
					diffNum++;
				}
			}
			if (diffNum >= 3){
				sp.play(soundIds[0], 100, 100, 1, 0, 1f);
				//flag = 0;
				result = "Alarm~!!!!";
				}
			else result = "good";
		
				

			    return result;
			
		}
	    
	};
	
	private int ARGBtoRGB (int ARGB) {
		int RGB = 0x0000;
		int[] result1 = new int[4];
		int[] result2 = new int[3];

		result1[0] = (byte) (ARGB >> 24);
		result1[1] = (byte) (ARGB >> 16);
		result1[2] = (byte) (ARGB >> 8);
		result1[3] = (byte) (ARGB /*>> 0*/);
		
		result2[0] = (result1[1] >> 3);
		result2[1] = (result1[2] >> 2);
		result2[2] = (result1[3] >> 3);
		
		RGB = (result2[0] << 11) | (result2[1] << 5) | (result2[2]);
		
		return RGB;
	}

	public void disableAlarm (View view){
		sp.autoPause();
		
	}
	
	private String saveToInternalStorage(String fileName, File mypath, File mydir,Bitmap bitmapImage){
    	
    	
    	
		
    	FileOutputStream fos;
		try{
			fos = new FileOutputStream(mypath);
			bitmapImage.compress(Bitmap.CompressFormat.JPEG, 100, fos);
	        fos.close();
    	}catch(Exception e){
    		
    		
    	}
        //Log.i(TAG, mydir.getAbsolutePath());
		//Toast toast = Toast.makeText(getApplicationContext(), mydir.getAbsolutePath(),Toast.LENGTH_SHORT);
		//toast.show();
		return mydir.getAbsolutePath();
		
    	
    }
    
    private boolean deleteFromInternalMemory(File mydir,String fileName, Bitmap bitmapImage){
    	
    	File file = new File(mydir, fileName);
    	boolean deleted = file.delete();
    	
    	//Toast toast = Toast.makeText(getApplicationContext(), mydir.getAbsolutePath(),Toast.LENGTH_SHORT);
    	//toast.show();
    	return deleted;
    	
    }
    
    private String convertToString(Bitmap bitmap){
		
	    ByteArrayOutputStream stream = new ByteArrayOutputStream();
	    bitmap.compress(Bitmap.CompressFormat.PNG, 100, stream);
	    byte[] byteArray = stream.toByteArray();
	    String temp = Base64.encodeToString(byteArray, Base64.DEFAULT);
	    return temp;
		
}
    
	
	public void sendMessageCam() {

		MyApplication app = (MyApplication) getApplication();
		
		
		//first four bytes are length of data block 
		int size = photoArray.length*2;
		byte buf[] = new byte[size+4];
		
		buf[0] = (byte)((size & 0xff000000) >> 24);
		buf[1] = (byte)((size & 0x00ff0000) >> 16);
		buf[2] = (byte)((size & 0x0000ff00) >> 8);
		buf[3] = (byte)(size & 0x000000ff);
		
		int i;
		for(i=0; i<photoArray.length; i=i+1){
	         buf[2*i+4] = (byte)((photoArray[i] & 0x0000ff00) >> 8);
	         buf[2*i+5] = (byte)(photoArray[i] & 0x000000ff);
		}

		// Now send through the output stream of the socket
		
		OutputStream out;
		try {
			out = app.sock.getOutputStream();
			try {
				out.write(buf, 0, buf.length);
			} catch (IOException e) {
				e.printStackTrace();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}


	public static double photoComparison2(Bitmap img1, Bitmap img2)
	{
	    int width1 = img1.getWidth();
	    int width2 = img2.getWidth();
	    int height1 = img1.getHeight();
	    int height2 = img2.getHeight();
	    if ((width1 != width2) || (height1 != height2)) {
	    	Log.d("CamTAG", "Image sizes do not match");
	    	return 0;
	    }
	    long diff = 0;
	    long diff2 = 0;
	    for (int i = 0; i < height1; i++) {
	      for (int j = 0; j < width1; j++) {
	    	int rgb1 = img1.getPixel(j, i);
	    	int rgb2 = img2.getPixel(j, i);
	        
	    	int r1 = (rgb1 >> 16) & 0xFF;
	    	int g1 = (rgb1 >>  8) & 0xFF;
	    	int b1 = (rgb1      ) & 0xFF;
	        
	    	int r2 = (rgb2 >> 16) & 0xff;
	    	int g2 = (rgb2 >>  8) & 0xff;
	    	int b2 = (rgb2      ) & 0xff;
	        

	    	
	        if (Math.abs(r1 - r2) > 18) {	        	
	        	diff2++;
	        }
	        
	        if (Math.abs(g1 - g2) > 18) {	        	
	        	diff2++;
	        }
	        
	        if (Math.abs(b1 - b2) > 18) {	        	
	        	diff2++;
	        }
	      }
	    }
	    
	    
	    double n = width1 * height1* 3;
	    double p = diff2 / n;
	    return (1-p)*100;
	    //System.out.println("diff percent: " + (p * 100.0));
	}
	
	
	


		
    
    

 public static String getHash(Bitmap bitmap){  
        Bitmap temp = Bitmap.createScaledBitmap(bitmap, 8, 10, false);  
        int[] grayValues = reduceColor(temp);  
        int average = sum/grayValues.length;  
        String reslut = computeBits(grayValues, average);  
        return reslut;  
    } 


	     
	   private static String computeBits(int[] grayValues, int average) {
		   char[] result = new char[grayValues.length];  
	        for (int i = 0; i < grayValues.length; i++)  
	        {  
	            if (grayValues[i] < average)  
	                result[i] = '0';  
	            else  
	                result[i] = '1';  
	        }  
	        return new String(result);  
}



	private static int[] reduceColor(Bitmap temp) {
        sum = 0;  
        int width = temp.getWidth();  
        int height = temp.getHeight();  
        Log.i("th", "scaled bitmap's width*heith:" + width + "*" + height);  
  
        int[] grayValues = new int[width * height];  
        int[] pix = new int[width * height];  
        temp.getPixels(pix, 0, width, 0, 0, width, height);  
        for (int i = 0; i < width; i++)  
            for (int j = 0; j < height; j++) {  
                int x = j * width + i;  
                int r = (pix[x] >> 16) & 0xff;  
                int g = (pix[x] >> 8) & 0xff;  
                int b = pix[x] & 0xff;  
                int grayValue = (r * 30 + g * 59 + b * 11) / 100;  
                sum+=grayValue;  
                grayValues[x] = grayValue;  
            }  
        return grayValues;
}

	

	private void setUpSound() {
		// Set the hardware buttons to control the sound
		this.setVolumeControlStream(AudioManager.STREAM_MUSIC);
				
		sp = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
		soundIds = new int[10];
		soundIds[0] = sp.load(this, R.raw.alarm, 1);
	
	}


	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.run_camera2, menu);
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
    protected void onPause() {
        super.onPause();
        releaseCamera();              // release the camera immediately on pause event
        my_timer.cancel();
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        reOpenCamera();              // reopen the camera immediately on resume event
    }
    
    private void releaseCamera(){
        if (mCamera != null){
            mCamera.release();        // release the camera for other applications
            mCamera = null;
        }
    }
    
    private void reOpenCamera(){
        if (mCamera == null){
        	mCamera = getCameraInstance();        // reopen the camera for other applications
        }
    }
	
	
	
	/** A safe way to get an instance of the Camera object. */
	public static Camera getCameraInstance(){
	    Camera c = null;
	    try {
	        c = Camera.open(); // attempt to get a Camera instance
	    }
	    catch (Exception e){
	        // Camera is not available (in use or does not exist)
	    }
	    return c; // returns null if camera is unavailable
	}
}
