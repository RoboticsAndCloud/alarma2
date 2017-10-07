package com.madsen.peter.alarma2.bt;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;

import java.util.Set;


public class BTUtils {
    public static String[] get_devices(Activity activity) {
        BluetoothAdapter bt = BluetoothAdapter.getDefaultAdapter();
        if (!bt.isEnabled()) {
            return null;
        }

        String[] devices = null;
        Set<BluetoothDevice> pairedDevices = bt.getBondedDevices();
        devices = new String[pairedDevices.size()];

        int i = 0;
        for (BluetoothDevice device : pairedDevices) {
            devices[i++] = device.getName();
        }

        return devices;
    }

    public static boolean isAvailable() {
        return BluetoothAdapter.getDefaultAdapter() != null;
    }

    public static boolean isEnabled() {
        return BluetoothAdapter.getDefaultAdapter().isEnabled();
    }
}
