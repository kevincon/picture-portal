package com.pictureportal.app;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

public class AndroidActivity extends Activity {
	
	private static final int CAPTURE_IMAGE_ACTIVITY_REQUEST_CODE = 100;
	private Uri fileUri;
	
	public static final int MEDIA_TYPE_IMAGE = 1;
	public static final int MEDIA_TYPE_VIDEO = 2;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);       
    }
    
    public void takePicture(View v) {
    	//TODO 
    	// create Intent to take a picture and return control to the calling application
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);

        fileUri = getOutputMediaFileUri(MEDIA_TYPE_IMAGE); // create a file to save the image
        if (fileUri == null) {
        	//TODO let user know we can't take a picture because we can't save it
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
            	//TODO need to move on to sending image to server
                //Toast.makeText(this, "Image saved to:\n" +
                         //data.getData(), Toast.LENGTH_SHORT).show();
            } else if (resultCode == RESULT_CANCELED) {
                //let user know they cancelled the image capture
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
            mediaFile = new File(mediaStorageDir.getPath() + File.separator +
            "IMG_"+ timeStamp + ".jpg");
        } else {
            return null;
        }

        return mediaFile;
    }
    
}