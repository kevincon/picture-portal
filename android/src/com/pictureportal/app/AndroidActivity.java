package com.pictureportal.app;

import java.io.*;
import java.net.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.app.Activity;
import android.content.Intent;
import android.location.Address;
import android.location.Geocoder;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class AndroidActivity extends Activity {
	private static String server;
	
	private static final int CAPTURE_IMAGE_ACTIVITY_REQUEST_CODE = 100;
	private Uri fileUri;
	private static String img_path;
	private static String img_name;
	private static byte b[];
	private static long fsize;
	
	public static final int MEDIA_TYPE_IMAGE = 1;
	public static final int MEDIA_TYPE_VIDEO = 2;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);       
    }
    
    public void takePicture(View v) {
    	//set server to IP from IP field
    	server = ((EditText)findViewById(R.id.IP)).getText().toString();
    	
    	// create Intent to take a picture and return control to the calling application
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);

        fileUri = getOutputMediaFileUri(MEDIA_TYPE_IMAGE); // create a file to save the image
        if (fileUri == null) {
        	// let user know we can't take a picture because we can't save it
        	Toast.makeText(this, "Sorry, couldn't create a file to save the picture.\n", Toast.LENGTH_SHORT).show();
        	return;
        }
        intent.putExtra(MediaStore.EXTRA_OUTPUT, fileUri); // set the image file name

        // start the image capture Intent
        startActivityForResult(intent, CAPTURE_IMAGE_ACTIVITY_REQUEST_CODE);
    }
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    	Log.d("PicturePortal", "Got to onactivityresult");
        if (requestCode == CAPTURE_IMAGE_ACTIVITY_REQUEST_CODE) {
            if (resultCode == RESULT_OK) {
                // Image captured and saved to fileUri specified in the Intent
            	try {
            		// connection with server
            		Socket serverSocket = new Socket(server, 1337);
            		OutputStream outToServer = serverSocket.getOutputStream();
            		int i;
            		
            		// send image size as byte array of size 8
            		File file = new File(img_path);
        			fsize = file.length();
    				b = ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN).putLong(fsize).array();
        			for(i= 0; i < 8; i++) {  
        				outToServer.write(b[i]);
        			}
        			
        			// send image name as char array
        			char iname[] = img_name.toCharArray();
        			for (i=0; i < iname.length; i++){
        				outToServer.write(iname[i]);
        			}
        			
        			// send image location
        			ExifInterface exif = new ExifInterface(img_path);
        			float[] latlong = new float[2];
        			char loc[];
        			if(exif.getLatLong(latlong)) {  
        				Geocoder geocoder = new Geocoder(this, Locale.getDefault());
        				Address addr = geocoder.getFromLocation(latlong[0], latlong[1], 1).get(0);
        				loc = addr.getLocality().toCharArray();
        			}
        			else {
    			       	loc = "Milky Way".toCharArray();
        			}
        			// send length of location name
       				int loc_len = loc.length;
        			outToServer.write(loc_len);
        			// send location name as char array
        			for (i = 0; i<loc_len; i++){
        				outToServer.write(loc[i]);
        			}
        			
            		// send image byte by byte until eof
            		FileInputStream fis = new FileInputStream(img_path);
            		BufferedInputStream in = new BufferedInputStream(fis);
            		
            		try {
            			while ((i = in.read()) != -1) {  
            				outToServer.write(i);
            			}
            		}
            		catch(Exception e){
            			// toast if we cannot send the image
            			Toast.makeText(this, "Could not send image.\n", Toast.LENGTH_SHORT).show();
            			return;           			
            		}
        			       			
            		serverSocket.close();
            	} catch (Exception e) {
            		// toast if any data to be transmitted to the server fails to transfer
            		Toast.makeText(this, "Could not connect to server.\n", Toast.LENGTH_SHORT).show();
            		return;
            	}
            	// toast the number of bytes sent to double check
            	Toast.makeText(this, Long.toString(fsize), Toast.LENGTH_SHORT).show();
            	
            } else if (resultCode == RESULT_CANCELED) {
                // let user know they canceled the image capture
            	Toast.makeText(this, "Image capture canceled.\n", Toast.LENGTH_SHORT).show();
            	
            } else {
                // Image capture failed, advise user
            	Toast.makeText(this, "Image capture failed\n", Toast.LENGTH_SHORT).show();
            }
        }
    }
	
	/** Create a file Uri for saving an image or video */
    private static Uri getOutputMediaFileUri(int type){
    	  File f = getOutputMediaFile(type);
    	  if (f == null) {
    		  return null;
    	  } else
    		  return Uri.fromFile(f);
    }

    /** Create a File for saving an image or video */
    private static File getOutputMediaFile(int type){
        // To be safe, you should check that the SDCard is mounted
        // using Environment.getExternalStorageState() before doing this.
    	String state = Environment.getExternalStorageState();
    	if(!state.equals(Environment. MEDIA_MOUNTED)) {
    		Log.d("PicturePortal", "Yo dawg, I can't save the image anywhere: " + state);
    		return null;
    	}
    	
        File mediaStorageDir = new File(Environment.getExternalStoragePublicDirectory(
                  Environment.DIRECTORY_PICTURES), "PicturePortal");
        // This location works best if you want the created images to be shared
        // between applications and persist after your app has been uninstalled.

        // Create the storage directory if it does not exist
        if (! mediaStorageDir.exists()){
            if (! mediaStorageDir.mkdirs()){
                Log.d("PicturePortal", "failed to create directory");
                return null;
            }
        }

        // Create a media file name
        String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
        File mediaFile;
        if (type == MEDIA_TYPE_IMAGE){
            img_name = timeStamp + ".jpg";
            img_path = mediaStorageDir.getPath() + File.separator + "IMG_"+ img_name;
            mediaFile = new File(img_path);
        } else {
            return null;
        }

        return mediaFile;
    }
    
}