package com.example.sihan.healthyalexa;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.SystemClock;
import android.support.v7.app.AlertDialog;
import android.text.InputType;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.Viewport;
import com.jjoe64.graphview.series.LineGraphSeries;

import org.junit.Test;
import org.junit.experimental.theories.DataPoint;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import static org.junit.Assert.*;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class MainActivity {
    private final Boolean DEBUG_MODE = false;

    View view;

    TextView showValue;
    TextView showData;
    int counter = 0;
    private String nameChoice;

    long startTime;
    private boolean startedGraphing;

    //Used for recording data
    private File dirpath;
    private FileOutputStream outputStream;
    private String recentTitle;
    private boolean isRecording;

    //Needed for Bluetooth
    private int count; //Prevent from scanning forever
    private boolean readyTo;
    private boolean foundChar;
    private BluetoothAdapter mBluetoothAdapter;
    private boolean mScanning;
    private Handler mHandler;
    private boolean mShouldUnbind;

    private ArrayList<String> deviceList;
    private static final int REQUEST_ENABLE_BT = 1;
    // Stops scanning after 10 seconds.
    private static final long SCAN_PERIOD = 10000;
    private static final String TAG = "MainActivity";

    //Information about board
    public static  String HexiAddress = "00:18:40:0A:00:48";
    public static String HexiServ = "00002a26-0000-1000-8000-00805f9b34fb";
    public  static String HexiChar = "00002a26-0000-1000-8000-00805f9b34fb";

    //Needed after Hexiwear is connected
    private BLEService mBluetoothLeService;
    private ArrayList<ArrayList<BluetoothGattCharacteristic>> mGattCharacteristics =
            new ArrayList<ArrayList<BluetoothGattCharacteristic>>();
    private boolean mConnected = false;
    private BluetoothGattCharacteristic mNotifyCharacteristic;
    private BluetoothGattCharacteristic bluetoothGattCharacteristicHexi;

    private final String LIST_NAME = "NAME";
    private final String LIST_UUID = "UUID";


        // Initializes a Bluetooth adapter.  For API level 18 and above, get a reference to
        // BluetoothAdapter through BluetoothManager.
        final BluetoothManager bluetoothManager =
                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();

        // Checks if Bluetooth is supported on the device.
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, R.string.error_bluetooth_not_supported, Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        //Check permissions
        checkBTAndWritePermissions();


        //Read the hexiwear address files files
        /*
         * Example Formatted:
         * Address = F0:C7:7F:94:CF:97
         * Service UUID = 0000ffe0-0000-1000-8000-00805f9b34fb
         * Characteristic UUID = 0000ffe1-0000-1000-8000-00805f9b34fb
         */
        if(isExternalStorageWritable()){
            File dir =  new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/HEXIWEARhhAPP/SETTINGS");
            if(!dir.exists()) //Create the directory if it does not exist
                dir.mkdir();
            File settings = new File(dir,"settings.txt");
            if(!settings.exists()) {
                try {
                    settings.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                    Log.i(TAG,"Failed to create file");
                }
                FileOutputStream os = null;
                try {
                    os = new FileOutputStream(settings);
                } catch (FileNotFoundException e) {
                    Log.i(TAG,"Failed to create output stream");
                    e.printStackTrace();
                }
                String sampleT = "Address=" + HexiAddress + "\nService UUID=" + HexiServ + "\nCharacteristic UUID=" + HexiChar + "\n";
                try {
                    os.write(sampleT.getBytes());
                }catch (IOException e) {
                    e.printStackTrace();
                }
                try {
                    os.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            //Read the file
            FileInputStream is;
            String stringContent = "";
            try {
                is = new FileInputStream(settings);
                char curChar;
                int charNum, curSet =0;
                Boolean recordFlag = false;
                while( (charNum = is.read()) != -1) //Read until there are no more characters
                {
                    curChar = (char) charNum;
                    if(curChar == '=') { //Record the address input
                        recordFlag = true;
                        curSet++;
                        continue;
                    }
                    if(curChar == '\n') { //Stop recording every new line and add the address to var
                        recordFlag = false;
                        switch (curSet){
                            case 1:
                                HexiAddress = stringContent;
                                break;
                            case 2:
                                HexiServ = stringContent;
                            case 3:
                                HexiChar = stringContent;
                        }
                        stringContent = "";
                    }

                    if(recordFlag && curChar != ' ')
                        stringContent += curChar;
                }
                is.close();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        Log.i(TAG, "HexiAddress: " + HexiAddress + "\nService: " + HexiServ + "\nChar: " + HexiChar + "\n");

        //Create file to write on and starts writing
        if(isExternalStorageWritable()) {
            dirpath = createFile();
            try {
                outputStream = new FileOutputStream(dirpath);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
            isRecording = true;
            //Write a header
            String mess = "Date,Time,Seconds,ITP Current, mA\n";
            try {
                //if(outputStream!=null)
                outputStream.write(mess.getBytes());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        });
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();

        if(mShouldUnbind)
            unbindService(mServiceConnection);
        mBluetoothLeService = null;
        //Close writing file
        try {
            outputStream.close();
            outputStream = null;
        } catch (IOException e) {
            e.printStackTrace();
        }
        Log.i(TAG,"Closed writing file...");
    }

    /* Checks if external storage is available for read and write */
    public boolean isExternalStorageWritable() {
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            return true;
        }
        return false;
    }

    //Check permissions of device
    private void checkBTAndWritePermissions() {
        if(Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP){
            int permissionCheck = this.checkSelfPermission("Manifest.permission.ACCESS_FINE_LOCATION");
            permissionCheck += this.checkSelfPermission("Manifest.permission.ACCESS_COARSE_LOCATION");
            if (permissionCheck != 0) {

                this.requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.WRITE_EXTERNAL_STORAGE }, 1); //Any number
            }
        }else{
            Log.d(TAG, "checkBTandWritePermissions: No need to check permissions. SDK version < LOLLIPOP.");
        }
    }

    @Override //Used for checking if the permissions were accepted and obtained
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case 1: {

                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {

                    // permission was granted, yay! Do the
                    // contacts-related task you need to do.
                } else {

                    // permission denied, boo! Disable the
                    // functionality that depends on this permission.
                    Toast.makeText(MainActivity.this, "Permission denied to read your External storage or Location", Toast.LENGTH_SHORT).show();
                }
                return;
            }

            // other 'case' lines to check for other
            // permissions this app might request
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        // Ensures Bluetooth is enabled on the device.  If Bluetooth is not currently enabled,
        // fire an intent to display a dialog asking the user to grant permission to enable it.
        if (!mBluetoothAdapter.isEnabled() && !readyTo) {
            if (!mBluetoothAdapter.isEnabled()) {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            }
        }
        //Start scanning for devices
        if(!readyTo && BluetoothApp.getApplication().getService()==null)
            scanLeDevice(true);

        if(readyTo || BluetoothApp.getApplication().getService()!=null) {
            foundChar = true;
            //checkIfCharacteristic(BluetoothApp.getApplication().getService().getSupportedGattServices());
            //If restarted, bypass scanning
            if (BluetoothApp.getApplication().getService() != null && mBluetoothLeService == null) {
                bluetoothGattCharacteristicHexi = BluetoothApp.getApplication().getGattCharacteristic();
                mBluetoothLeService = BluetoothApp.getApplication().getService();
                Log.i(TAG, "mBluetoothLeService has been set, null?: " + (mBluetoothLeService == null));
            }
            registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
            if (mBluetoothLeService != null) {
                final boolean result = mBluetoothLeService.connect(HexiAddress);
                Log.d(TAG, "Connect request result=" + result);
            }
        }

    }

    //Create a file to write on
    private File createFile(){
        String dateTime = "[" + BluetoothApp.getDateString() + " " + BluetoothApp.getTimeString() + "]";
        //Toast.makeText(this,"Clicked!",Toast.LENGTH_LONG).show();
        File Root = Environment.getExternalStorageDirectory();
        File dir = new File(Root.getAbsolutePath() + "/HEXIWEARhhAPP");
        if(!dir.exists())
            dir.mkdir();
        recentTitle = "Data" + dateTime + ".txt";
        File file = new File(dir,recentTitle);
        recentTitle = dir + "/" + recentTitle;
        try {
            file.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return file;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // User chose not to enable Bluetooth.
        if (requestCode == REQUEST_ENABLE_BT && resultCode == Activity.RESULT_CANCELED) {
            finish();
            return;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onPause() {
        super.onPause();
        scanLeDevice(false);
        //trackTime.end();
        //isRecording = false;
        //trackTime.interrupt();
    }

    private void scanLeDevice(final boolean enable) {
        if (enable) {
            // Stops scanning after a pre-defined scan period.
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    mScanning = false;
                    mBluetoothAdapter.stopLeScan(mLeScanCallback);
                    invalidateOptionsMenu();
                    //Start scanning again,
                    count++;
                    if(count < 3) {
                        Log.i(TAG,"Did not find Hexiwear device, searching again");
                        scanLeDevice(true);
                    }
                }
            }, SCAN_PERIOD);

            mScanning = true;
            mBluetoothAdapter.startLeScan(mLeScanCallback);
        } else {
            mScanning = false;
            mBluetoothAdapter.stopLeScan(mLeScanCallback);
        }
        invalidateOptionsMenu();
    }

    private void checkDeviceName(BluetoothDevice device){
        //Prevent spam of the log
        if(device.getAddress()!=null&& ! deviceList.contains(device.getAddress())) {
            Log.i(TAG, "Found Device: " + device.getName() + "\n" + device.getAddress());
            deviceList.add(device.getAddress());
            if(device.getAddress().equals(HexiAddress)){
                Log.i(TAG,"Found Hexi!");
                count = 3;
                if (mScanning) {
                    mBluetoothAdapter.stopLeScan(mLeScanCallback);
                    mScanning = false;
                }
                readyTo = true;
                Intent gattServiceIntent = new Intent(this, BLEService.class);
                bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
                registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter());
            }
        }
    }

    // Device scan callback.
    private BluetoothAdapter.LeScanCallback mLeScanCallback =
            new BluetoothAdapter.LeScanCallback() {

                @Override
                public void onLeScan(final BluetoothDevice device, int rssi, byte[] scanRecord) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            checkDeviceName(device);
                        }
                    });
                }
            };

    // Code to manage Service lifecycle.
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BLEService.LocalBinder) service).getService();
            mShouldUnbind = true;
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            // Automatically connects to the device upon successful start-up initialization.
            mBluetoothLeService.connect(HexiAddress);
            //Used so it can later be accessed in another activity
            BluetoothApp.getApplication().setBluetoothLe(mBluetoothLeService);
            Log.i(TAG,"Connected to hexiwear!");
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    private int test_count = 0;
    public void countIN (View v) {
        //Max number: 15 ( 0...9 a b c d e f)
        //REMOVE LAtER
        if(DEBUG_MODE){
            series.appendData(new DataPoint(test_count,test_count / 10.0),updateViewportRecent,200);
            test_count++;
            Log.i(TAG,graph.getViewport().getXAxisBoundsStatus().toString());
        }
        if(counter >= 15){
            return;
        }
        else{
            counter++;
        }
        showValue.setText(Double.toString(intToDouble(counter)));
        sendData(counter);

        //Toast.makeText(getApplicationContext(),String.valueOf(counter),Toast.LENGTH_LONG).show();
    }
    public void countDE (View v) {
        if(counter <= 1){
            return;
        }
        else{
            counter--;
        }
        showValue.setText(Double.toString(intToDouble(counter)));
        sendData(counter);
        //Toast.makeText(getApplicationContext(),String.valueOf(counter),Toast.LENGTH_LONG).show();
    }

    //TESTING on axis bound listener

    public double intToDouble(int num){
        return (double)num/10; //Maybe remove
//        switch(num){
//            case 1:
//                return 0.1;
//            case 2:
//                return 0.2;
//            case 3:
//                return 0.3;
//            case 4:
//                return 0.4;
//            case 5:
//                return 0.5;
//            case 6:
//                return 0.6;
//            case 7:
//                return 0.7;
//            case 8:
//                return 0.8;
//            case 9:
//                return 0.9;
//            case  10:
//                return 1.0;
//            case 11:
//                return 1.1;
//            default:
//                return -1;
//        }
    }

    public String intToStringHex(int n){
        if(n < 0 || n > 15)
            return "-1";
        if(n <= 9)
            return Integer.toString(n);
        //Assuming ASCII encoding
        int ch = 'a' + (n-10);
        return Character.toString((char) ch); //Returns a b c e f
    }


    public void startSensingClick(View v){
        Log.i(TAG,"Start sensing button clicked");
        if(foundChar)
            startGraph();
    }

    public void startGraph(){

        Log.i(TAG,"Starting other graph...");
        //Make sure that the application is actually connected to the device
        if(BluetoothApp.getApplication().getService()==null)
            return;
        //Also close the file if it is writing TODO
        //Rename prompt
        promptInputRenameFile();

//        Intent intent = new Intent(this,SecondaryGraph.class);
//        startActivity(intent);
    }

    private void startSensingGraph(){
        sendData(0);
        Log.i(TAG,"Currently calling Secondary Graph..");
        Intent intent = new Intent(this,SecondaryGraph.class);
        startActivity(intent);
    }

    //Ask the user if they want to rename the file they just wrote
    public void promptRenameFile(){
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage(R.string.prompt_rename);
        builder.setTitle("Rename");

        //Set up the buttons
        builder.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                //Rename file
                String dir = Environment.getExternalStorageDirectory().getAbsolutePath() + "/HEXIWEARhhAPP";
                File originFile = new File(recentTitle);
                File renamedFile = new File(dir,"Inter11.txt");
                originFile.renameTo(renamedFile);
                Log.i(TAG,"Renamed File");
            }
        });
        builder.setNegativeButton("No",null);

        //Creating and showing dialog
        builder.create().show();
    }

    private void promptInputRenameFile(){
        isRecording = false; //Stop recording
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Rename File?");
        final EditText textInput = new EditText(this);
        textInput.setInputType(InputType.TYPE_CLASS_TEXT);
        builder.setView(textInput);

        //Set up the buttons
        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                //Rename file
                String dir = Environment.getExternalStorageDirectory().getAbsolutePath() + "/HEXIWEARhhAPP";
                File originFile = new File(recentTitle);
                String name = textInput.getText().toString();
                File renamedFile = new File(dir,name + ".txt");
                originFile.renameTo(renamedFile);
                Log.i(TAG,"Renamed File");
                startSensingGraph();
            }
        });
        builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                startSensingGraph();
            }
        });

        //Creating and showing dialog
        builder.create().show();
    }

    private void sendData(int dataInt){
        if(!foundChar)
            return;
        Log.i(TAG,"Sent data " + intToStringHex(dataInt));
        bluetoothGattCharacteristicHM_SOFT.setValue(intToStringHex(dataInt));
        mBluetoothLeService.writeCharacteristic(bluetoothGattCharacteristicHM_SOFT);
        mBluetoothLeService.setCharacteristicNotification(bluetoothGattCharacteristicHM_SOFT, true);
        //Toast.makeText(this,"Sent value " + dataInt, Toast.LENGTH_SHORT).show();
    }


    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BLEServicee.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BLEService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BLEService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BLEService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }
    // Handles various events fired by the Service.
    // ACTION_GATT_CONNECTED: connected to a GATT server.
    // ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
    // ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
    // ACTION_DATA_AVAILABLE: received data from the device.  This can be a result of read
    //                        or notification operations.
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (BLEService.ACTION_GATT_CONNECTED.equals(action)) {
                mConnected = true;
                Log.i(TAG,"Connected to Hexiwear!");
                Toast.makeText(getApplicationContext(),"Connected to Hexiwear!",Toast.LENGTH_SHORT).show();
                invalidateOptionsMenu();
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                invalidateOptionsMenu();
            } else if (BLEService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Show all the supported services and characteristics on the user interface.
                Log.i(TAG,"Found Services!");
                checkIfCharacteristic(mBluetoothLeService.getSupportedGattServices());
            } else if (BLEService.ACTION_DATA_AVAILABLE.equals(action)) {
                //Read current
                String returnedVal = intent.getStringExtra(BLEService.EXTRA_DATA);
                //remove the Ma to convert to an actual usable value
                double returnedValDouble;
                showData.setText(returnedVal);
                if(checkIfValidDouble(returnedVal)){
                    returnedValDouble = Double.parseDouble(getValidDouble(returnedVal));
                    if(!startedGraphing){
                        startedGraphing = true;
                        startTime = SystemClock.elapsedRealtime(); //Zero the time
                    }
                    double currentX =(SystemClock.elapsedRealtime() - startTime) / 1000.0;
                    //Log.i(TAG,"Current x: "+Double.toString(currentX));
                    series.appendData(new DataPoint( currentX,returnedValDouble),updateViewportRecent,500);
                    //Record to file
                    if(outputStream!=null && isRecording){
                        //In format of <date>,<time>,<seconds after start>, <Current Value>, <suffix>,\n so it can be read as a csv file
                        String message = BluetoothApp.getDateString() + "," + BluetoothApp.getTimeStringWithColons() + "," + currentX + "," + returnedValDouble + "," +
                                getValueSuffix(returnedVal) + "\n";
                        try {
                            outputStream.write(message.getBytes());
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
        }
    };

    //Check if incoming data is a double by checking if it has a digit
    public static boolean checkIfValidDouble(String s){
        //return s.substring(0,1).equals("0");
        for(int i = 0; i < s.length(); i++)
            if(s.charAt(i) == '.'){
                return (Character.isDigit(s.charAt(i - 1)) && Character.isDigit(s.charAt(i+1)));
            }
        return false;
    }

    private String getValidDouble(String s){
        String returnedString = "";
        for(int i = 0; i < s.length(); i++){
            String current = s.substring(i,i+1);
            if(current.equals(" ") || current.equals("μ") || current.equals("m"))
                break;
            returnedString+=current;
        }
        return returnedString;
    }

    public static String getValueSuffix(String s){
        String current;
        for(int i = 0; i < s.length(); i++){
            current = s.substring(i,i+1);
            if(current.equals("μ"))
                return "μA";
            if(current.equals("m"))
                return "mA";
        }
        return "";
    }

    //Checks to see if it is the data from the board we need (current)
    private void checkIfCharacteristic(List<BluetoothGattService> gattServices){
        if(gattServices==null || foundChar)
            return;
        Log.i(TAG,"Checking characteristics...");
        String tempUUID;
        UUID UUID_HM_SOFT = UUID.fromString(HexiChar);
        //Loop through services
        for(BluetoothGattService gattService : gattServices){
            tempUUID = gattService.getUuid().toString();
            Log.i(TAG,"Service: " + tempUUID);
            if(tempUUID.equals(HexiServ)){
                List<BluetoothGattCharacteristic> gattCharacteristics =
                        gattService.getCharacteristics();
                //Loop through characteristics
                for(BluetoothGattCharacteristic gattCharacteristic : gattCharacteristics){
                    tempUUID = gattCharacteristic.getUuid().toString();
                    Log.i(TAG,"Characteristic: " + tempUUID);
                    if(tempUUID.equals(HexiChar)){
                        Log.i(TAG,"Found Characteristics, Reading....");
                        //Toast.makeText(getApplicationContext(),"Found data!",Toast.LENGTH_SHORT).show();
                        foundChar = true;
                        Log.i(TAG,"Obtained characteristic");
                        bluetoothGattCharacteristicHM_SOFT = gattService.getCharacteristic(UUID_HM_SOFT);
                        //Add to application file
                        BluetoothApp.getApplication().setBluetoothGattCharacteristic(bluetoothGattCharacteristicHM_SOFT);
                        activateCharacteristic(gattCharacteristic);
                    }
                }
            }
        }
    }

    //Start reading the data from the board
    private void activateCharacteristic(BluetoothGattCharacteristic gattChar){
        final BluetoothGattCharacteristic characteristic =
                gattChar;
        final int charaProp = characteristic.getProperties();
        if ((charaProp | BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
            // If there is an active notification on a characteristic, clear
            // it first so it doesn't update the data field on the user interface.
            if (mNotifyCharacteristic != null) {
                mBluetoothLeService.setCharacteristicNotification(
                        mNotifyCharacteristic, false);
                mNotifyCharacteristic = null;
            }
            mBluetoothLeService.readCharacteristic(characteristic);
        }
        if ((charaProp | BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
            mNotifyCharacteristic = characteristic;
            mBluetoothLeService.setCharacteristicNotification(
                    characteristic, true);
        }
        foundChar = true;
    }

    public void backToMain(View view)
    {

    }

}