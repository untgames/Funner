package com.untgames.funner.facebook_session;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.util.Log;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookSdk;
import com.facebook.appevents.AppEventsLogger;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;

import com.untgames.funner.application.EngineActivity;

public class SessionImpl implements EngineActivity.EngineActivityResultListener
{
  private static final String TAG = "funner.facebook_session.SessionImpl";
  
  //copied from Facebook SDK 4.15.0 facebook/src/main/java/com/facebook/login/LoginManager.java
  private static final String      PUBLISH_PERMISSION_PREFIX = "publish";
  private static final String      MANAGE_PERMISSION_PREFIX  = "manage";
  private static final Set<String> OTHER_PUBLISH_PERMISSIONS = getOtherPublishPermissions();
  
  private EngineActivity  activity;
  private CallbackManager callbackManager;
  
  public SessionImpl (EngineActivity inActivity)
  {
    activity = inActivity;
  	
  	activity.addResultListener (this);

  	FacebookSdk.sdkInitialize(activity.getApplicationContext());

  	callbackManager = CallbackManager.Factory.create();
    
  	LoginManager.getInstance().registerCallback(callbackManager, new FacebookCallback<LoginResult>() {
      @Override
      public void onSuccess(LoginResult loginResult)
      {
         //TODO request publish permissions
         onLoginSucceeded (loginResult.getAccessToken ().getToken ());
      }

      @Override
      public void onCancel()
      {
        onLoginCanceled ();
      }

      @Override
      public void onError(FacebookException exception)
      {
        onLoginFailed (exception.toString ());
      }
    });

    publishInstall();
  }
  
  public boolean handleEngineActivityResult (int requestCode, int resultCode, Intent data)
  {
    callbackManager.onActivityResult(requestCode, resultCode, data);
    
    return false;
  }
  
  //check if we can use sdk login
  public boolean canLogin ()
  {
  	String packageName = activity.getPackageName ();
    int    resId       = activity.getResources ().getIdentifier ("FacebookSdkResourcesVersion", "string", packageName);
    
    if (resId == 0)
    {
    	Log.d (TAG, "Application has no Facebook SDK resources, sdk login unavailable");
    	return false;
    }
    
    String resourcesVersion = activity.getString (resId);
  
    Log.d (TAG, "Check can login. SDK version = " + FacebookSdk.getSdkVersion () + " resourcesVersion = " + resourcesVersion);
  
    if (!FacebookSdk.getSdkVersion ().equals (resourcesVersion))
    	throw new IllegalStateException ("Facebook SDK and Facebook SDK resources version mismatch");
    
  	return true;
  }
  
  //copied from Facebook SDK 4.15.0 facebook/src/main/java/com/facebook/login/LoginManager.java
  private static boolean isPublishPermission(String permission)
  {
    return permission != null &&
        (permission.startsWith(PUBLISH_PERMISSION_PREFIX) ||
            permission.startsWith(MANAGE_PERMISSION_PREFIX) ||
            OTHER_PUBLISH_PERMISSIONS.contains(permission));
  }
  
  private static Set<String> getOtherPublishPermissions()
  {
    HashSet<String> set = new HashSet<String>() {{
        add("ads_management");
        add("create_event");
        add("rsvp_event");
    }};
    return Collections.unmodifiableSet(set);
  }
  
  public void login (String permissions, String previousToken)
  {
  	String [] permissionsArray = permissions.split (",");
  	
  	final List<String> readPermissions  = new ArrayList<String> (permissionsArray.length); 
  	final List<String> writePermissions = new ArrayList<String> (permissionsArray.length); 
  	
  	for (String permission : permissionsArray)
  	{
  		if (isPublishPermission (permission))
  		  writePermissions.add (permission);
  		else
  		  readPermissions.add (permission);
  	}
  	
  	if (!writePermissions.isEmpty ()) //TODO
  		throw new RuntimeException ("Write permissions not supported");
  	
  	Log.d (TAG, "Login started");
  	
  	AccessToken currentAccessToken = AccessToken.getCurrentAccessToken();
  	
    if (currentAccessToken != null)  //we are already logged in
    {
      if (previousToken.equals (currentAccessToken.getToken ()))
      {
        boolean needsNewPermissions = false;

        Set currentPermissions = currentAccessToken.getPermissions();
        
        for (String permission : permissionsArray)
        {
          if (!currentPermissions.contains(permission))
          {
            needsNewPermissions = true;
            break;
          }
        }

        if (!needsNewPermissions)
        {
          //we already have valid token with needed permissions set
          onLoginSucceeded (currentAccessToken.getToken ());

          return;
        }
      }

      logout ();
    }
  	
  	activity.runOnUiThread (new Runnable () {
  		public void run ()
  		{
  			try
  			{
  			  LoginManager.getInstance().logInWithReadPermissions(activity, readPermissions);
  			}
  			catch (Throwable e)
  			{
  				Log.e (TAG, "Exception while logging in: " + e.getMessage () + "\n stack: ");
  				e.printStackTrace ();
  				onLoginFailed (e.getMessage ());
  			}
  		}
  	});
  }
  
  public void logout ()
  {
    LoginManager.getInstance ().logOut ();
  }
  
  public static boolean isFacebookAppInstalled (Context context)
  {
    try
    {
      ApplicationInfo info = context.getPackageManager ().getApplicationInfo ("com.facebook.katana", 0);
  
      return true;
    } 
    catch (Throwable e)
    {
      return false;
    }
  }
  
  public void publishInstall ()
  {
  	Log.d (TAG, "Publishing install");
  
  	activity.runOnUiThread (new Runnable () {
  		public void run ()
  		{
  			try
  			{
  		    AppEventsLogger.activateApp(activity.getApplication());
  			}
  			catch (Throwable e)
  			{
  				Log.e (TAG, "Exception while publishing install: " + e.getMessage () + "\n stack: ");
  				e.printStackTrace ();
  			}
  		}
  	});
  }
  
  private native void onLoginSucceeded (String token);
  private native void onLoginFailed (String error);
  private native void onLoginCanceled ();
}
