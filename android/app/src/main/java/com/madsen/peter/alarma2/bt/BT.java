package com.madsen.peter.alarma2.bt;


import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;


public class BT {
    private final static UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); // Standard
    private final static String LOG_NAME = "BT";
    private final static byte END_OF_COMMAND = 10; // newline

    private final BluetoothAdapter mBluetoothAdapter;
    private BluetoothSocket mmSocket;
    private OutputStream mmOutputStream;
    private InputStream mmInputStream;
    private Thread workerThread;
    private byte[] readBuffer;
    private int readBufferPosition;

    private volatile boolean stopWorker;

    private final BlockingQueue<String> replies = new LinkedBlockingQueue<String>();

    private boolean is_connected = false;


    public BT() {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    public void open(String device) throws Exception {
        BluetoothDevice mmDevice = null;
        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
        if (pairedDevices.size() > 0) {
            for (BluetoothDevice btdev : pairedDevices) {
                if (btdev.getName().equals(device)) {
                    Log.d(LOG_NAME, "Found");
                    mmDevice = btdev;
                    break;
                }
            }
        }

        if (mmDevice == null) {
            throw new Exception("Not found");
        }
        // myLabel.setText("Bluetooth Device Found");

        mBluetoothAdapter.cancelDiscovery();

        // //SerialPortService
        // ID
        mmSocket = mmDevice.createRfcommSocketToServiceRecord(uuid);
        mmSocket.connect();

        mmOutputStream = mmSocket.getOutputStream();
        mmInputStream = mmSocket.getInputStream();
        beginListenForData();
    }

    private void beginListenForData() {
        // final Handler handler = new Handler();

        stopWorker = false;
        readBufferPosition = 0;
        readBuffer = new byte[64 * 1024];
        workerThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (!Thread.currentThread().isInterrupted() && !stopWorker) {
                    try {
                        int bytesAvailable = mmInputStream.available();
                        if (bytesAvailable > 0) {
                            byte[] packetBytes = new byte[bytesAvailable];
                            mmInputStream.read(packetBytes);
                            for (int i = 0; i < bytesAvailable; i++) {
                                byte b = packetBytes[i];

                                // regular char
                                if (b >= 32 && b <= 127) {
                                    readBuffer[readBufferPosition++] = b;

                                // anything else is a delimiter
                                } else if (readBufferPosition != 0) {
                                    byte[] encodedBytes = new byte[readBufferPosition];
                                    System.arraycopy(readBuffer, 0,
                                            encodedBytes, 0,
                                            encodedBytes.length);
                                    final String data = new String(
                                            encodedBytes, "US-ASCII").trim();
                                    readBufferPosition = 0;

                                    try {
                                        replies.put(data);
                                    } catch (InterruptedException e) {
                                        // TODO Auto-generated catch block
                                        // e.printStackTrace();
                                    }
                                    Log.d(LOG_NAME, data);

                                    // handler.post(new Runnable() {
                                    // @Override
                                    // public void run() {
                                    // Log.d("received: ", data);
                                    // Toast.makeText(activity, data,
                                    // Toast.LENGTH_SHORT).show();
                                    // }
                                    // });
                                }
                            }
                        }
                    } catch (IOException ex) {
                        stopWorker = true;
                    }
                }
            }
        });

        workerThread.start();

        is_connected = true;
    }

//	public String send(int b) throws Exception {
//		mmOutputStream.write(b);
//		mmOutputStream.write(10);
//		String reply = replies.poll(5, TimeUnit.SECONDS);
//		if(reply == null) {
//			throw new Exception("Timed out.");
//		}
//		return reply;
//	}

    public String send(String s) throws Exception {
        mmOutputStream.write(s.getBytes());
        mmOutputStream.write(END_OF_COMMAND);
        String reply = replies.poll(5, TimeUnit.SECONDS);
        if (reply == null) {
            throw new Exception("Timed out.");
        }
        return reply;
    }

    public void close() {
        stopWorker = true;

        if (mmOutputStream != null) {
            try {
                mmOutputStream.close();
            } catch (IOException ignored) {
            }
        }
        if (mmInputStream != null) {
            try {
                mmInputStream.close();
            } catch (IOException ignored) {
            }
        }
        if (mmSocket != null) {
            try {
                mmSocket.close();
            } catch (IOException ignored) {
            }
        }

        is_connected = false;
    }

    public boolean isConnected() {
        return is_connected;
    }
}
