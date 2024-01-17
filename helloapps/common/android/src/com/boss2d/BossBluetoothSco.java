package com.boss2d;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;

public class BossBluetoothSco extends BroadcastReceiver
{
    public static void init(Activity activity)
    {
        if(self != null) return;
        self = new BossBluetoothSco();
        activity.registerReceiver(self, new IntentFilter(AudioManager.ACTION_SCO_AUDIO_STATE_UPDATED));
    }

    public static Intent open(Activity activity)
    {
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("audio/*");
        return intent;
    }

    public static void play(Context context, Intent intent)
    {
        if(self == null) return;
        Uri uri = intent.getData();
        final int takeFlags = intent.getFlags()
            & (Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
        context.grantUriPermission(context.getPackageName(), uri, takeFlags);
        self.onPlay(context, uri);
    }

    public static void stop()
    {
        if(self == null) return;
        self.onStop();
    }

    public static void setVolume(Context context, float value)
    {
        if(self == null) return;
        self.onSetVolume(context, value);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // 구현부
    ////////////////////////////////////////////////////////////////////////////////
    private static BossBluetoothSco self = null;
    private boolean FirstTime = true;
    private MediaPlayer mMediaPlayer = null;

    public void onPlay(Context context, Uri uri)
    {
        onStop();
        try
        {
            mMediaPlayer = new MediaPlayer();
            mMediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener()
            {
                @Override
                public void onCompletion(MediaPlayer mp)
                {
                    mp.release();
                }
            });
            mMediaPlayer.setDataSource(context, uri);
            mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mMediaPlayer.prepare();
            mMediaPlayer.start();
            Log.d("TAG", "### BossBluetoothSco: onPlay(" + uri.toString() + ")");
        }
        catch(Exception e)
        {
            Log.d("TAG", "### BossBluetoothSco: onPlay(Exception : " + e + ")");
            mMediaPlayer = null;
        }
    }

    public void onStop()
    {
        if(mMediaPlayer != null)
        {
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
    }

    public void onSetVolume(Context context, float value)
    {
        AudioManager Manager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
        final int MaxVolume = Manager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        Manager.setStreamVolume(AudioManager.STREAM_MUSIC, (int) (MaxVolume * value), 0);
        Log.d("TAG", "### BossBluetoothSco: onSetVolume(" + MaxVolume + "x" + value + ")");
    }

    @Override
    public void onReceive(final Context context, final Intent intent)
    {
        AudioManager Manager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
        final String Action = intent.getAction();
        if(Action.equals("android.media.ACTION_SCO_AUDIO_STATE_UPDATED"))
        {
            try
            {
                if(Manager.isBluetoothScoAvailableOffCall())
                {
                    if(FirstTime)
                    {
                        FirstTime = false;
                        if(!Manager.isBluetoothScoOn())
                        {
                            Manager.setMode(0);
                            Manager.setBluetoothScoOn(true);
                            Manager.startBluetoothSco();
                            Manager.setMode(AudioManager.MODE_IN_CALL);
                            Log.d("TAG", "### BossBluetoothSco: onReceive(starting)");
                        }
                        else Log.d("TAG", "### BossBluetoothSco: onReceive(started)");
                    }
                }
                else Log.d("TAG", "### BossBluetoothSco: onReceive(SCO not available)");
            }
            catch(Exception e)
            {
                Log.d("TAG", "### BossBluetoothSco: onReceive(Exception : " + e + ")");
            }
        }
        else if(Action.equals("android.bluetooth.headset.action.STATE_CHANGED"))
        {
            final int Data = intent.getIntExtra("android.bluetooth.headset.extra.STATE", 0x00000000);
            if(Data == 0x00000000) // DISCONNECTED
            {
                Manager.setBluetoothScoOn(false);
                Manager.stopBluetoothSco();
                Manager.setMode(AudioManager.MODE_NORMAL);
                Log.d("TAG", "### BossBluetoothSco: onReceive(DISCONNECTED)");
            }
            else
            {
                Manager.setMode(0);
                Manager.setBluetoothScoOn(true);
                Manager.startBluetoothSco();
                Manager.setMode(AudioManager.MODE_IN_CALL);
                Log.d("TAG", "### BossBluetoothSco: onReceive(CONNECTED)");
            }
        }
        else if(Action.equals("android.bluetooth.headset.action.FORCE_ON"))
        {
            Manager.setMode(0);
            Manager.setBluetoothScoOn(true);
            Manager.startBluetoothSco();
            Manager.setMode(AudioManager.MODE_IN_CALL);
            Log.d("TAG", "### BossBluetoothSco: onReceive(FORCE_ON)");
        }
        else if(Action.equals("android.bluetooth.headset.action.FORCE_OFF"))
        {
            Manager.setBluetoothScoOn(false);
            Manager.stopBluetoothSco();
            Manager.setMode(AudioManager.MODE_NORMAL);
            Log.d("TAG", "### BossBluetoothSco: onReceive(FORCE_OFF)");
        }
        else Log.d("TAG", "### BossBluetoothSco: onReceive(" + Action + ")");
    }
}
