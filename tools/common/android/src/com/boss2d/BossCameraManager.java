package com.boss2d;
import android.Manifest;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.AsyncTask;
import android.os.Handler;
import android.util.Log;
import java.nio.ByteBuffer;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class BossCameraManager
{
    private int mCameraWidth = 0;
    private int mCameraHeight = 0;
    private Camera mCamera = null;
    private boolean mCameraEnabled = false;
    private int mWorkType = 0;
    private final int mDeliveryWaitCountMax = 50;
    private int mDeliveryWaitCount = mDeliveryWaitCountMax;
    private SurfaceTexture mSurfaceTexture = null;
    private TimerTask mTask = null;
    private Timer mTimer = null;
    private Camera.ShutterCallback mShutterCallback = new Camera.ShutterCallback()
    {
        public void onShutter()
        {
            Log.d("TAG", "### Shutter");
        }
    };
    private Camera.PictureCallback mPictureCallback = new Camera.PictureCallback()
    {
        public void onPictureTaken(byte[] anonymousArrayOfByte, Camera anonymousCamera)
        {
            Log.d("TAG", "### Picture Begin: " + anonymousArrayOfByte);
            if(anonymousArrayOfByte != null)
                OnPictureTaken(anonymousArrayOfByte, anonymousArrayOfByte.length);
            mCamera.stopPreview();
            mCamera.startPreview();
            mDeliveryWaitCount = 0;
            Log.d("TAG", "### Picture End");
        }
    };
    private Camera.PreviewCallback mPreviewCallback = new Camera.PreviewCallback()
    {
        public void onPreviewFrame(byte[] anonymousArrayOfByte, Camera anonymousCamera)
        {
            Log.d("TAG", "### Preview Begin: " + anonymousArrayOfByte);
            if(anonymousArrayOfByte != null)
            {
                Camera.Size size = anonymousCamera.getParameters().getPreviewSize();
                OnPreviewTaken(anonymousArrayOfByte, anonymousArrayOfByte.length, size.width, size.height);
            }
            mCamera.stopPreview();
            mCamera.startPreview();
            mDeliveryWaitCount = 0;
            Log.d("TAG", "### Preview End");
        }
    };

    // Native methods
    private static native void OnPictureTaken(byte[] data, int length);
    private static native void OnPreviewTaken(byte[] data, int length, int width, int height);

    private static BossCameraManager sCameraManager = null;
    public static String info()
    {
        Camera camera = null;
        String collector = new String();
        try
        {
            camera = Camera.open();
            List<Camera.Size> PictureSizes = camera.getParameters().getSupportedPictureSizes();
            for(Camera.Size CurSize : PictureSizes)
                collector += "Size_" + CurSize.width + "x" + CurSize.height + ";";
            List<Integer> PictureFormats = camera.getParameters().getSupportedPictureFormats();
            for(Integer CurFormat : PictureFormats)
            {
                if(CurFormat == ImageFormat.JPEG)  collector += "Format_JPEG;";
                else if(CurFormat == ImageFormat.NV21)  collector += "Format_NV21;";
                else if(CurFormat == ImageFormat.RGB_565)  collector += "Format_RGB_565;";
                else if(CurFormat == ImageFormat.YUV_420_888)  collector += "Format_YUV_420_888;";
                else if(CurFormat == ImageFormat.YUV_444_888)  collector += "Format_YUV_444_888;";
                else if(CurFormat == ImageFormat.YUY2)  collector += "Format_YUY2;";
                else if(CurFormat == ImageFormat.YV12)  collector += "Format_YV12;";
            }
        }
        catch(Exception e)
        {
            Log.d("TAG", "### info Exception(" + e.toString() + ")");
        }
        if(camera != null)
            camera.release();
        return collector;
    }

    public static void init(int texture, int width, int height)
    {
        if(sCameraManager != null) return;
        sCameraManager = new BossCameraManager();
        sCameraManager.mSurfaceTexture = new SurfaceTexture(texture);

        if(sCameraManager.openCamera(width, height))
        {
            sCameraManager.mCameraEnabled = true;
            sCameraManager.mDeliveryWaitCount = 0;
        }
        else return;

        sCameraManager.mTask = new TimerTask()
        {
            @Override
            public void run()
            {
                if(sCameraManager.mCameraEnabled)
                {
                    if(sCameraManager.mDeliveryWaitCount == 0)
                    {
                        try
                        {
                            if(sCameraManager.mWorkType == 1)
                            {
                                Log.d("TAG", "### setOneShotPreviewCallback Begin");
                                sCameraManager.mDeliveryWaitCount = sCameraManager.mDeliveryWaitCountMax;
                                sCameraManager.mCamera.setOneShotPreviewCallback(sCameraManager.mPreviewCallback);
                                Log.d("TAG", "### setOneShotPreviewCallback End");
                            }
                            else if(sCameraManager.mWorkType == 2)
                            {
                                Log.d("TAG", "### takePicture Begin");
                                sCameraManager.mDeliveryWaitCount = sCameraManager.mDeliveryWaitCountMax;
                                sCameraManager.mCamera.takePicture(sCameraManager.mShutterCallback,
                                        null, sCameraManager.mPictureCallback);
                                Log.d("TAG", "### takePicture End");
                            }
                        }
                        catch(Exception e)
                        {
                            Log.d("TAG", "### run Exception: " + e.toString());
                            sCameraManager.mDeliveryWaitCount = 0;
                        }
                    }
                    else if(0 < sCameraManager.mDeliveryWaitCount)
                    {
                        sCameraManager.mDeliveryWaitCount--;
                        if(sCameraManager.mDeliveryWaitCount == 0)
                        {
                            Log.d("TAG", "### Camera reset Begin");
                            sCameraManager.closeCamera();
                            sCameraManager.openCamera();
                            Log.d("TAG", "### Camera reset End");
                        }
                    }
                }
            }
        };
        sCameraManager.mTimer = new Timer();
        sCameraManager.mTimer.schedule(sCameraManager.mTask, 1000, 200);
    }

    public static void quit()
    {
        if(sCameraManager != null)
        {
            sCameraManager.mCameraEnabled = false;
            sCameraManager.closeCamera();
            sCameraManager = null;
        }
    }

    public static void play(int type)
    {
        if(sCameraManager == null) return;
        sCameraManager.mWorkType = type;
    }

    public static void stop()
    {
        if(sCameraManager == null) return;
        sCameraManager.mWorkType = 0;
    }

    public boolean openCamera(int width, int height)
    {
        mCameraWidth = width;
        mCameraHeight = height;
        return openCamera();
    }

    public boolean openCamera()
    {
        try
        {
            mCamera = Camera.open();
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPictureSize(mCameraWidth, mCameraHeight);
            parameters.setFocusMode("continuous-picture");
            mCamera.setParameters(parameters);
            mCamera.autoFocus(new Camera.AutoFocusCallback()
            {
                public void onAutoFocus(boolean anonymousBoolean, Camera anonymousCamera)
                {
                    Log.w("TAG", "### AUTO FOCUS: " + String.valueOf(anonymousBoolean));
                }
            });
            mCamera.setAutoFocusMoveCallback(new Camera.AutoFocusMoveCallback()
            {
                public void onAutoFocusMoving(boolean anonymousBoolean, Camera anonymousCamera)
                {
                    Log.w("TAG", "### AUTO FOCUS Move: " + String.valueOf(anonymousBoolean));
                }
            });
            mCamera.setPreviewTexture(mSurfaceTexture);
            mCamera.startPreview();
        }
        catch(Exception e)
        {
            Log.d("TAG", "### openCamera Exception(" + e.toString() + ")");
            closeCamera();
            return false;
        }
        return true;
    }

    public void closeCamera()
    {
        if(mCamera != null)
        {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
    }

    public void setISO(int value)
    {
        Camera.Parameters parameters = mCamera.getParameters();
        Log.w("TAG", parameters.flatten());
        mCamera.setParameters(parameters);
    }

    public void setShutterSpeed(int value)
    {
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.set("mode", "m");
        parameters.set("shutter-speed", String.valueOf(value));
        mCamera.setParameters(parameters);
    }
}
