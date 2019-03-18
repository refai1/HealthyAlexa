package com.example.matchburn.hmsoftbluetooth;

import android.app.Application;
import android.bluetooth.BluetoothGattCharacteristic;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

/**
 * Created by Matchburn321 on 11/28/2017.
 * Contains the bluetoothLeService that communicates with the board
 */

public class BluetoothApp extends Application {
    private static BluetoothApp sInstance;

    public static BluetoothApp getApplication(){
        return sInstance;
    }

    BluetoothLeService mBLE = null;
    BluetoothGattCharacteristic mBGC = null;

    public void onCreate(){
        super.onCreate();

        sInstance = this;
    }

    public void setBluetoothLe(BluetoothLeService in){
        mBLE = in;
    }
    public void setBluetoothGattCharacteristic(BluetoothGattCharacteristic in){mBGC=in;}
    public BluetoothLeService getService(){
        return mBLE;
    }
    public BluetoothGattCharacteristic getGattCharacteristic(){return mBGC;}

    //Other auxilary functions

    //returns a string containing today's date
    public static String getDateString(){
        Date time = Calendar.getInstance().getTime();
        SimpleDateFormat outputFmt = new SimpleDateFormat("MM-dd-yyyy");
        return outputFmt.format(time);
    }

    //returns a string containing a timestamp in format (HH-mm-ss)
    public static String getTimeString(){
        Date time = Calendar.getInstance().getTime();
        SimpleDateFormat outputFmt = new SimpleDateFormat("HH-mm-ss");
        return outputFmt.format(time);
    }

    public static String getTimeStringWithColons(){
        Date time = Calendar.getInstance().getTime();
        SimpleDateFormat outputFmt = new SimpleDateFormat("HH:mm:ss");
        return outputFmt.format(time);
    }
}
